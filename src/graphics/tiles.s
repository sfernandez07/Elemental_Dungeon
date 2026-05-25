; tiles.s — Datos CHR (8 KB): tabla de patrones 0 (fondo) + tabla 1 (sprites)
;
; Cada tile = 16 bytes: 8 bytes plano-0 + 8 bytes plano-1
; Índice de color por pixel = bit_plano1 * 2 + bit_plano0
;   00 = color 0 (fondo transparente/negro)
;   01 = color 1 (paredes)
;   10 = color 2 (puntos)
;   11 = color 3 (power pellet / Pac-Man)

.segment "CHARS"

; ============================================================
; Tabla de patrones 0 ($0000–$0FFF) — tiles de fondo
; ============================================================

; Tile $00 — Suelo vacío (todo negro)
.byte $00,$00,$00,$00,$00,$00,$00,$00   ; plano 0
.byte $00,$00,$00,$00,$00,$00,$00,$00   ; plano 1

; Tile $01 — Pared sólida (color 1 = azul)
.byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF   ; plano 0
.byte $00,$00,$00,$00,$00,$00,$00,$00   ; plano 1

; Tile $02 — Punto pequeño (color 2 = blanco, 2×2 px centrado)
;  . . . . . . . .
;  . . . . . . . .
;  . . . . . . . .
;  . . . 1 1 . . .
;  . . . 1 1 . . .
;  . . . . . . . .
;  . . . . . . . .
;  . . . . . . . .
.byte $00,$00,$00,$00,$00,$00,$00,$00   ; plano 0
.byte $00,$00,$00,$18,$18,$00,$00,$00   ; plano 1

; Tile $03 — Power pellet (color 3 = amarillo, círculo 6 px)
;  . . . . . . . .
;  . . . 1 1 . . .
;  . . 1 1 1 1 . .
;  . 1 1 1 1 1 1 .
;  . 1 1 1 1 1 1 .
;  . . 1 1 1 1 . .
;  . . . 1 1 . . .
;  . . . . . . . .
.byte $00,$18,$3C,$7E,$7E,$3C,$18,$00   ; plano 0
.byte $00,$18,$3C,$7E,$7E,$3C,$18,$00   ; plano 1

; Tiles $04–$0D — Dígitos '0'–'9'  (color 2 = blanco: plano0=0, plano1=bitmap)

; Tile $04 — '0'
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $3C,$42,$46,$4A,$52,$62,$42,$3C

; Tile $05 — '1'
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $18,$38,$18,$18,$18,$18,$18,$7E

; Tile $06 — '2'
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $3C,$42,$02,$04,$18,$20,$40,$7E

; Tile $07 — '3'
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $3C,$42,$02,$1C,$02,$42,$42,$3C

; Tile $08 — '4'
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $04,$0C,$14,$24,$44,$7E,$04,$04

; Tile $09 — '5'
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $7E,$40,$40,$7C,$02,$02,$42,$3C

; Tile $0A — '6'
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $3C,$40,$40,$7C,$42,$42,$42,$3C

; Tile $0B — '7'
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $7E,$02,$04,$08,$10,$10,$10,$10

; Tile $0C — '8'
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $3C,$42,$42,$3C,$42,$42,$42,$3C

; Tile $0D — '9'
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $3C,$42,$42,$3E,$02,$02,$42,$3C

; Tile $0E — Teletransportador (color 3 = amarillo: plano0=plano1=bitmap)
;  Diamante/estrella que indica un portal
.byte $00,$18,$24,$42,$42,$24,$18,$00   ; plano 0
.byte $00,$18,$24,$42,$42,$24,$18,$00   ; plano 1

; Tile $0F — Pared temporal activa (color 3 = amarillo, patrón ajedrez)
;  Visualmente distinta de la pared sólida
.byte $CC,$33,$CC,$33,$CC,$33,$CC,$33   ; plano 0
.byte $CC,$33,$CC,$33,$CC,$33,$CC,$33   ; plano 1

; Tile $10 — Trampa de velocidad (color 2 = blanco: plano0=0, plano1=bitmap)
;  Símbolo de reloj de arena / ralentización
.byte $00,$00,$00,$00,$00,$00,$00,$00   ; plano 0
.byte $00,$7E,$24,$18,$18,$24,$7E,$00   ; plano 1

; Tile $11 — Bomba (color 3 = amarillo: plano0=plano1=bitmap)
;  Círculo con mecha
.byte $06,$0C,$3C,$7E,$FF,$7E,$3C,$00   ; plano 0
.byte $06,$0C,$3C,$7E,$FF,$7E,$3C,$00   ; plano 1

; Tiles $12–$FF — reservados
.res (238 * 16), $00

; ============================================================
; Tabla de patrones 1 ($1000–$1FFF) — sprites
; ============================================================

; Sprite tile $00 — Pac-Man boca abierta mirando a la derecha (color 3)
;  . . 1 1 1 1 . .
;  . 1 1 1 1 1 1 .
;  1 1 1 1 1 1 1 .   <- boca arriba
;  1 1 1 1 . . . .   <- abertura de la boca
;  1 1 1 1 . . . .
;  1 1 1 1 1 1 1 .   <- boca abajo
;  . 1 1 1 1 1 1 .
;  . . 1 1 1 1 . .
.byte $3C,$7E,$FE,$F8,$F8,$FE,$7E,$3C   ; plano 0
.byte $3C,$7E,$FE,$F8,$F8,$FE,$7E,$3C   ; plano 1

; Sprite tile $01 — Pac-Man boca cerrada (círculo, color 3)
.byte $3C,$7E,$FF,$FF,$FF,$FF,$7E,$3C   ; plano 0
.byte $3C,$7E,$FF,$FF,$FF,$FF,$7E,$3C   ; plano 1

; Sprite tile $02 — Pac-Man boca abierta mirando a la izquierda
.byte $3C,$7E,$7F,$1F,$1F,$7F,$7E,$3C   ; plano 0
.byte $3C,$7E,$7F,$1F,$1F,$7F,$7E,$3C   ; plano 1

; Sprite tile $03 — Pac-Man boca abierta mirando arriba
.byte $3C,$7E,$FF,$FF,$E7,$C3,$7E,$3C   ; plano 0
.byte $3C,$7E,$FF,$FF,$E7,$C3,$7E,$3C   ; plano 1

; Sprite tile $04 — Pac-Man boca abierta mirando abajo
.byte $3C,$C3,$E7,$FF,$FF,$7E,$7E,$3C   ; plano 0
.byte $3C,$C3,$E7,$FF,$FF,$7E,$7E,$3C   ; plano 1

; Sprite tile $05 — Fantasma cuerpo (color 1, azul claro)
;  . . 1 1 1 1 . .
;  . 1 1 1 1 1 1 .
;  1 1 1 1 1 1 1 1
;  1 1 1 1 1 1 1 1
;  1 1 1 1 1 1 1 1
;  1 1 1 1 1 1 1 1
;  1 1 1 1 1 1 1 1
;  1 . 1 . . 1 . 1   <- falda
.byte $3C,$7E,$FF,$FF,$FF,$FF,$FF,$A5   ; plano 0
.byte $00,$00,$00,$00,$00,$00,$00,$00   ; plano 1

; Sprite tile $06 — Fantasma ojos (color 3, blanco)
;  . . . . . . . .
;  . . 1 1 . 1 1 .
;  . . 1 1 . 1 1 .
;  . . . . . . . .
;  . . . . . . . .
;  . . . . . . . .
;  . . . . . . . .
;  . . . . . . . .
.byte $00,$66,$66,$00,$00,$00,$00,$00   ; plano 0
.byte $00,$66,$66,$00,$00,$00,$00,$00   ; plano 1

; Sprite tile $07 — Fantasma asustado (color 2, azul oscuro)
.byte $3C,$7E,$DB,$FF,$FF,$DB,$FF,$A5   ; plano 0
.byte $00,$00,$00,$00,$00,$00,$00,$00   ; plano 1

; Tiles $08–$FF — reservados para animaciones y UI
.res (248 * 16), $00
