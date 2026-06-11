; famitone2.s — Stub de FamiTone2 para neslib
;
; Stub mínimo que define:
;   - Las variables de zero page que neslib usa internamente
;   - Los buffers OAM y paleta en RAM fija
;   - Funciones FamiTone vacías (música e SFX deshabilitados hasta Fase 7)
;
; Cuando implementes audio (Fase 7), reemplaza este stub por el
; famitone2.s original de Shiru: https://shiru.untergrund.net/files/src/famitone2.zip

; ---- Flags de compilación ----
FT_SFX_ENABLE   = 0   ; 0 = sin SFX por ahora
FT_DPCM_ENABLE  = 0
FT_PAL_SUPPORT  = 0

; ---- Registros de hardware NES ----
PPU_CTRL    = $2000
PPU_MASK    = $2001
PPU_STATUS  = $2002
PPU_OAM_ADR = $2003
PPU_OAM_DATA= $2004
PPU_SCROLL  = $2005
PPU_ADDR    = $2006
PPU_DATA    = $2007
PPU_OAM_DMA = $4014
CTRL_PORT1  = $4016
CTRL_PORT2  = $4017

; ---- Direcciones fijas de RAM ----
; El handler NMI de neslib hace: lda #>OAM_BUF → debe ser $02
OAM_BUF = $0200   ; 256 bytes: buffer DMA de sprites
PAL_BUF = $0300   ; 32 bytes:  buffer de paleta

; ---- Variables de zero page ----
; (neslib.s las accede con prefijo '<', por lo que deben estar en ZP)
; El linker asigna las direcciones secuencialmente a partir del segmento ZEROPAGE.

.segment "ZEROPAGE"

PTR:              .res 2   ; puntero genérico (pal_copy, vram_unrle, etc.)
TEMP:             .res 4   ; temporales (vram_read, vram_write, memcpy, scroll)
LEN:              .res 2   ; longitud (vram_fill, memcpy)
SRC:              .res 2   ; puntero origen (memcpy)
DST:              .res 2   ; puntero destino (memfill, memcpy)
RLE_LOW:          .res 1   ; puntero RLE: byte bajo
RLE_HIGH:         .res 1   ; puntero RLE: byte alto
RLE_TAG:          .res 1   ; byte de escape RLE
RLE_BYTE:         .res 1   ; último byte descomprimido

SCROLL_X:         .res 1
SCROLL_Y:         .res 1
SCROLL_X1:        .res 1   ; scroll X para split (función split())
SCRX:             .res 1   ; scroll X acumulado (función scroll())
SCRY:             .res 1   ; scroll Y acumulado (función scroll())

PPU_CTRL_VAR:     .res 1   ; copia de PPU_CTRL ($2000)
PPU_CTRL_VAR1:    .res 1   ; copia de PPU_CTRL para split
PPU_MASK_VAR:     .res 1   ; copia de PPU_MASK ($2001)

PAL_BG_PTR:       .res 2   ; puntero a paleta de fondo activa
PAL_SPR_PTR:      .res 2   ; puntero a paleta de sprites activa

FRAME_CNT1:       .res 1   ; contador de frames (NTSC/PAL)
FRAME_CNT2:       .res 1   ; contador de subframes PAL

VRAM_UPDATE:      .res 1   ; flag: aplicar buffer VRAM en siguiente NMI
NAME_UPD_ADR:     .res 2   ; puntero al buffer de actualización de VRAM
NAME_UPD_ENABLE:  .res 1   ; flag: buffer de VRAM activo

PAL_UPDATE:       .res 1   ; flag: paleta modificada, actualizar en NMI
RAND_SEED:        .res 2   ; semilla para rand8/rand16

PAD_BUF:          .res 3   ; triple lectura del pad para corrección de errores
PAD_STATE:        .res 2   ; estado actual de los dos pads
PAD_STATEP:       .res 2   ; estado anterior de los pads
PAD_STATET:       .res 2   ; estado de "trigger" (flanco ascendente)

NTSC_MODE:        .res 1   ; 0 = PAL, 1 = NTSC

; Exportamos lo que crt0.s necesita inicializar
.exportzp PPU_CTRL_VAR
.exportzp NTSC_MODE

; ---- Stubs de funciones FamiTone ----
; Cuando se active el audio, estos se reemplazarán por la implementación real.

.segment "CODE"

FamiToneUpdate:
FamiToneMusicPlay:
FamiToneMusicStop:
FamiToneMusicPause:
    rts

; FamiToneSfxPlay no se referencia si FT_SFX_ENABLE = 0
