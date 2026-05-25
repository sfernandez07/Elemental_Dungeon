; crt0.s — Startup de NES adaptado a neslib
;
; Responsabilidades:
;   1. Cabecera iNES (16 bytes)
;   2. Handler de reset: inicializa hardware + RAM + neslib + llama a _main
;   3. Tabla de vectores: NMI e IRQ de neslib, RESET propio

; Símbolos externos
.import  _main
.import  nmi, irq                  ; handlers de neslib.s (via famitone2.s)
.importzp PPU_CTRL_VAR             ; var interna de neslib (en famitone2.s)
.importzp NTSC_MODE                ; var interna de neslib (en famitone2.s)

; Símbolos del linker para DATA y BSS
.import __DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__
.import __BSS_RUN__,  __BSS_SIZE__

; Registros hardware (solo los que necesita el startup)
PPU_CTRL   = $2000
PPU_MASK   = $2001
PPU_STATUS = $2002
PPU_OAM_DMA= $4014

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
    txs                 ; inicializar pila
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

    ; Copiar segmento DATA de ROM a RAM
    ldy #0
    ldx #<(__DATA_SIZE__)
    beq @bss
@copydata:
    lda __DATA_LOAD__, y
    sta __DATA_RUN__,  y
    iny
    dex
    bne @copydata

    ; Poner a cero el segmento BSS
@bss:
    lda #0
    ldx #<(__BSS_SIZE__)
    beq @neslib_init
@clearbss:
    sta __BSS_RUN__, y
    iny
    dex
    bne @clearbss

@neslib_init:
    ; Modo NTSC (1). Para detectar PAL automáticamente, haría falta
    ; un contador de frames; de momento lo fijamos como NTSC.
    lda #1
    sta NTSC_MODE

    ; Configurar PPU_CTRL: NMI activo + sprites en tabla de patrones 1 ($1000)
    ; $88 = %10001000 (NMI enable | sprite table 1)
    lda #$88
    sta PPU_CTRL_VAR    ; neslib trackea este valor en ZP
    sta PPU_CTRL        ; activar NMI → a partir de aquí neslib gestiona los frames

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
