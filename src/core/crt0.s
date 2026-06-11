; crt0.s — Startup de NES adaptado a neslib
;
; Responsabilidades:
;   1. Cabecera iNES (16 bytes)
;   2. Handler de reset: inicializa hardware + RAM + neslib + llama a _main
;   3. Tabla de vectores: NMI e IRQ de neslib, RESET propio

; Indicamos al linker que este módulo ES el startup (evita que ld65 pull nes.lib/crt0.o)
.export __STARTUP__ : absolute = 1

; Símbolos externos de neslib/famitone2
.import  _main
.import  nmi, irq                  ; handlers de neslib.s
.import  _pal_bright, _pal_clear   ; inicializar paleta antes de NMI
.import  _oam_clear                ; inicializar OAM antes de NMI
.importzp PPU_CTRL_VAR             ; var interna de neslib (en famitone2.s)
.importzp NTSC_MODE                ; var interna de neslib (en famitone2.s)
.importzp sp                       ; cc65 software stack pointer ($0000-$0001)

; Símbolos del linker
.import __DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__
.import __BSS_RUN__,  __BSS_SIZE__
.import __RAM_START__, __RAM_SIZE__

; Registros hardware (solo los que necesita el startup)
PPU_CTRL   = $2000
PPU_MASK   = $2001
PPU_STATUS = $2002
PPU_ADDR   = $2006
PPU_DATA   = $2007

; ----------------------------------------------------------------
; Cabecera iNES (segmento HEADER → primeros 16 bytes del ROM)
; ----------------------------------------------------------------
.segment "HEADER"
    .byte $4E, $45, $53, $1A    ; "NES" + marcador iNES
    .byte 2                      ; 2 × 16 KB PRG-ROM = 32 KB
    .byte 1                      ; 1 × 8 KB CHR-ROM
    .byte $00                    ; Mapper 0, espejo horizontal
    .byte $00
    .byte $00, $00, $00, $00
    .byte $00, $00, $00, $00

; ----------------------------------------------------------------
; STARTUP — debe ser el primer código en PRG-ROM
; ----------------------------------------------------------------
.segment "STARTUP"

reset:
    sei                 ; deshabilitar IRQs
    cld                 ; modo decimal desactivado
    ldx #$40
    stx $4017           ; deshabilitar frame IRQ del APU
    ldx #$FF
    txs                 ; inicializar pila hardware 6502
    inx                 ; X = 0
    stx PPU_CTRL        ; deshabilitar NMI
    stx PPU_MASK        ; deshabilitar renderizado
    stx $4010           ; deshabilitar DMC IRQ

    ; Esperar primer VBlank
@vb1:
    bit PPU_STATUS
    bpl @vb1

    ; Limpiar toda la RAM ($0000–$07FF)
    txa
@clrmem:
    sta $000, x
    sta $100, x
    sta $200, x
    sta $300, x
    sta $400, x
    sta $500, x
    sta $600, x
    sta $700, x
    inx
    bne @clrmem

    ; Esperar segundo VBlank (PPU estabilizada)
@vb2:
    bit PPU_STATUS
    bpl @vb2

    ; ---- Limpiar paleta PPU a $0F (negro) antes de habilitar NMI ----
    lda #$3F
    sta PPU_ADDR
    stx PPU_ADDR        ; X = 0 → PPU_ADDR = $3F00
    lda #$0F
    ldx #$20
@clearpal:
    sta PPU_DATA
    dex
    bne @clearpal
    ldx #0              ; restaurar X=0

    ; ---- Copiar segmento DATA de ROM a RAM ----
    ldy #0
    ldx #<(__DATA_SIZE__)
    beq @bss
@copydata:
    lda __DATA_LOAD__, y
    sta __DATA_RUN__,  y
    iny
    dex
    bne @copydata

    ; ---- Poner a cero el segmento BSS ----
@bss:
    lda #0
    ldy #0              ; siempre empezar desde el inicio del segmento BSS
    ldx #<(__BSS_SIZE__)
    beq @setup
@clearbss:
    sta __BSS_RUN__, y
    iny
    dex
    bne @clearbss

@setup:
    ; ---- Configurar stack de C (sp) ----
    lda #<(__RAM_START__+__RAM_SIZE__)
    sta sp
    lda #>(__RAM_START__+__RAM_SIZE__)
    sta sp+1

    ; ---- Inicializar neslib antes de habilitar NMI ----
    ; Esto asegura que PAL_BG_PTR/PAL_SPR_PTR apunten a la tabla de brillo
    ; normal, y que PAL_BUF y OAM_BUF estén en un estado válido.
    lda #4
    jsr _pal_bright     ; PAL_BG_PTR = PAL_SPR_PTR = palBrightTable4 (normal)
    jsr _pal_clear      ; PAL_BUF = $0F (negro) en las 32 entradas
    jsr _oam_clear      ; OAM_BUF[Y] = $FF (sprites ocultos)

    ; ---- Modo NTSC fijo ----
    lda #1
    sta NTSC_MODE

    ; ---- Configurar PPU_CTRL: NMI activo + sprites en $1000 ----
    ; $88 = %10001000 (NMI enable | sprite table 1)
    lda #$88
    sta PPU_CTRL_VAR    ; neslib trackea este valor en ZP
    sta PPU_CTRL        ; activar NMI

    jsr _main

    ; Si main() retorna (no debería), reiniciar
    jmp reset

; ----------------------------------------------------------------
; Tabla de vectores NES ($FFFA–$FFFF)
; ----------------------------------------------------------------
.segment "VECTORS"
    .word nmi           ; NMI   ($FFFA) — handler de neslib
    .word reset         ; RESET ($FFFC) — nuestro startup
    .word irq           ; IRQ   ($FFFE) — handler de neslib (solo RTI)
