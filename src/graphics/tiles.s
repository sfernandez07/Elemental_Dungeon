; tiles.s — Datos CHR (8 KB): tabla de patrones 0 (fondo) + tabla 1 (sprites)
;
; Cada tile = 16 bytes: 8 bytes plano-0 + 8 bytes plano-1
; Índice de color por pixel = bit_plano1 * 2 + bit_plano0
;   00 = color 0 (fondo/transparente)
;   01 = color 1 (paredes / cuerpo fantasma)
;   10 = color 2 (puntos / foliaje)
;   11 = color 3 (pellet / jugador / rayo activo)

.segment "CHARS"

; ============================================================
; Tabla de patrones 0 ($0000–$0FFF) — tiles de fondo
; ============================================================

; Tile $00 — Suelo vacío
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $00,$00,$00,$00,$00,$00,$00,$00

; Tile $01 — Pared (marco color1, relleno color3)
.byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF   ; plane0: todos encendidos
.byte $00,$7E,$7E,$7E,$7E,$7E,$7E,$00   ; plane1: solo interior (borde=color1, centro=color3)

; Tile $02 — Punto (color 2, círculo 4×4 centrado)
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $00,$00,$18,$3C,$3C,$18,$00,$00

; Tile $03 — Power pellet (color 3, círculo 6 px)
.byte $00,$18,$3C,$7E,$7E,$3C,$18,$00
.byte $00,$18,$3C,$7E,$7E,$3C,$18,$00

; ---- Tiles $04–$0D  Dígitos '0'–'9' (color 2) ----

; Tile $04 — '0' (color 3 — ambos planos = brillante)
.byte $3C,$42,$46,$4A,$52,$62,$42,$3C
.byte $3C,$42,$46,$4A,$52,$62,$42,$3C

; Tile $05 — '1'
.byte $18,$38,$18,$18,$18,$18,$18,$7E
.byte $18,$38,$18,$18,$18,$18,$18,$7E

; Tile $06 — '2'
.byte $3C,$42,$02,$04,$18,$20,$40,$7E
.byte $3C,$42,$02,$04,$18,$20,$40,$7E

; Tile $07 — '3'
.byte $3C,$42,$02,$1C,$02,$42,$42,$3C
.byte $3C,$42,$02,$1C,$02,$42,$42,$3C

; Tile $08 — '4'
.byte $04,$0C,$14,$24,$44,$7E,$04,$04
.byte $04,$0C,$14,$24,$44,$7E,$04,$04

; Tile $09 — '5'
.byte $7E,$40,$40,$7C,$02,$02,$42,$3C
.byte $7E,$40,$40,$7C,$02,$02,$42,$3C

; Tile $0A — '6'
.byte $3C,$40,$40,$7C,$42,$42,$42,$3C
.byte $3C,$40,$40,$7C,$42,$42,$42,$3C

; Tile $0B — '7'
.byte $7E,$02,$04,$08,$10,$10,$10,$10
.byte $7E,$02,$04,$08,$10,$10,$10,$10

; Tile $0C — '8'
.byte $3C,$42,$42,$3C,$42,$42,$42,$3C
.byte $3C,$42,$42,$3C,$42,$42,$42,$3C

; Tile $0D — '9'
.byte $3C,$42,$42,$3E,$02,$02,$42,$3C
.byte $3C,$42,$42,$3E,$02,$02,$42,$3C

; Tile $0E — Teletransportador (color 3, diamante giratorio)
.byte $00,$18,$3C,$7E,$7E,$3C,$18,$00
.byte $00,$18,$24,$42,$42,$24,$18,$00

; Tile $0F — Pared temporal activa (color 3, ajedrez)
.byte $CC,$33,$CC,$33,$CC,$33,$CC,$33
.byte $CC,$33,$CC,$33,$CC,$33,$CC,$33

; Tile $10 — Trampa de velocidad (color 2, reloj de arena)
.byte $00,$00,$00,$00,$00,$00,$00,$00
.byte $00,$7E,$24,$18,$18,$24,$7E,$00

; Tile $11 — Bomba (color 3, esfera con mecha)
;  Mecha: bit 6 del tile (2 px arriba-derecha)
;  Cuerpo: círculo relleno
.byte $02,$06,$1E,$7E,$FF,$7E,$3C,$00
.byte $02,$06,$1E,$7E,$FF,$7E,$3C,$00

; Tiles $12–$23 — reservados (letras antiguas, sustituidas por $2C–$47)
.res (18 * 16), $00

; Tile $24 — Puerta cerrada (reja vertical)
.byte $FF,$92,$FF,$92,$FF,$92,$FF,$92
.byte $00,$00,$00,$00,$00,$00,$00,$00

; Tile $25 — Puerta abierta (arco con jambas)
.byte $FF,$81,$81,$81,$81,$81,$81,$00
.byte $00,$00,$00,$00,$00,$00,$00,$00

; Tile $26 — TILE_VOLCANO
; color 2 (naranja) en boca, color 3 (amarillo) en núcleo, color 1 en cuerpo
.byte $00,$3C,$56,$C3,$FF,$FF,$FF,$FF   ; plano 0
.byte $18,$3C,$00,$00,$00,$00,$00,$00   ; plano 1

; Tile $27 — TILE_LAVA frame A (burbuja patrón 1)
; base naranja (color 2), burbujas amarillas (color 3)
.byte $18,$00,$24,$00,$18,$00,$42,$00   ; plano 0 — burbujas
.byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF   ; plano 1 — base completa

; Tile $28 — TILE_TREE
; foliaje color 2 (verde brillante), tronco color 1 (verde oscuro)
.byte $00,$00,$00,$00,$18,$18,$3C,$00   ; plano 0 — tronco
.byte $18,$3C,$7E,$FF,$00,$00,$00,$00   ; plano 1 — follaje

; Tile $29 — TILE_WHIRLWIND (embudo de tornado)
; color 1, forma que se estrecha hacia abajo
.byte $FF,$7E,$3C,$18,$18,$3C,$08,$08   ; plano 0
.byte $00,$00,$00,$00,$00,$00,$00,$00   ; plano 1

; Tile $2A — TILE_BUBBLE (burbuja circular con destello)
; anillo color 1, destello color 2 arriba-izquierda
.byte $3C,$42,$81,$81,$81,$81,$42,$3C   ; plano 0 — anillo
.byte $00,$30,$00,$00,$00,$00,$00,$00   ; plano 1 — destello

; Tile $2B — TILE_LIGHTNING frame A (rayo zigzag, color 3 brillante)
.byte $38,$60,$78,$18,$0C,$08,$08,$00   ; plano 0
.byte $38,$60,$78,$18,$0C,$08,$08,$00   ; plano 1

; ---- Tiles $2C–$45: alfabeto A–Z (draw_text usa BASE + ch - 'A') ----

; ---- Letras A–Z y símbolos (color 3 — ambos planos = brillante) ----

; Tile $2C — 'A'
.byte $38,$44,$44,$7C,$44,$44,$44,$00
.byte $38,$44,$44,$7C,$44,$44,$44,$00

; Tile $2D — 'B'
.byte $78,$44,$44,$78,$44,$44,$78,$00
.byte $78,$44,$44,$78,$44,$44,$78,$00

; Tile $2E — 'C'
.byte $38,$44,$40,$40,$40,$44,$38,$00
.byte $38,$44,$40,$40,$40,$44,$38,$00

; Tile $2F — 'D'
.byte $78,$44,$44,$44,$44,$44,$78,$00
.byte $78,$44,$44,$44,$44,$44,$78,$00

; Tile $30 — 'E'
.byte $7C,$40,$40,$78,$40,$40,$7C,$00
.byte $7C,$40,$40,$78,$40,$40,$7C,$00

; Tile $31 — 'F'
.byte $7C,$40,$40,$78,$40,$40,$40,$00
.byte $7C,$40,$40,$78,$40,$40,$40,$00

; Tile $32 — 'G'
.byte $38,$44,$40,$4C,$44,$44,$38,$00
.byte $38,$44,$40,$4C,$44,$44,$38,$00

; Tile $33 — 'H'
.byte $44,$44,$44,$7C,$44,$44,$44,$00
.byte $44,$44,$44,$7C,$44,$44,$44,$00

; Tile $34 — 'I'
.byte $7C,$10,$10,$10,$10,$10,$7C,$00
.byte $7C,$10,$10,$10,$10,$10,$7C,$00

; Tile $35 — 'J'
.byte $1C,$04,$04,$04,$04,$44,$38,$00
.byte $1C,$04,$04,$04,$04,$44,$38,$00

; Tile $36 — 'K'
.byte $44,$48,$50,$60,$50,$48,$44,$00
.byte $44,$48,$50,$60,$50,$48,$44,$00

; Tile $37 — 'L'
.byte $40,$40,$40,$40,$40,$40,$7C,$00
.byte $40,$40,$40,$40,$40,$40,$7C,$00

; Tile $38 — 'M'
.byte $44,$6C,$54,$44,$44,$44,$44,$00
.byte $44,$6C,$54,$44,$44,$44,$44,$00

; Tile $39 — 'N'
.byte $44,$64,$54,$4C,$44,$44,$44,$00
.byte $44,$64,$54,$4C,$44,$44,$44,$00

; Tile $3A — 'O'
.byte $38,$44,$44,$44,$44,$44,$38,$00
.byte $38,$44,$44,$44,$44,$44,$38,$00

; Tile $3B — 'P'
.byte $78,$44,$44,$78,$40,$40,$40,$00
.byte $78,$44,$44,$78,$40,$40,$40,$00

; Tile $3C — 'Q'
.byte $38,$44,$44,$44,$54,$48,$34,$00
.byte $38,$44,$44,$44,$54,$48,$34,$00

; Tile $3D — 'R'
.byte $78,$44,$44,$78,$50,$48,$44,$00
.byte $78,$44,$44,$78,$50,$48,$44,$00

; Tile $3E — 'S'
.byte $38,$44,$40,$38,$04,$44,$38,$00
.byte $38,$44,$40,$38,$04,$44,$38,$00

; Tile $3F — 'T'
.byte $7C,$10,$10,$10,$10,$10,$10,$00
.byte $7C,$10,$10,$10,$10,$10,$10,$00

; Tile $40 — 'U'
.byte $44,$44,$44,$44,$44,$44,$38,$00
.byte $44,$44,$44,$44,$44,$44,$38,$00

; Tile $41 — 'V'
.byte $44,$44,$44,$44,$28,$28,$10,$00
.byte $44,$44,$44,$44,$28,$28,$10,$00

; Tile $42 — 'W'
.byte $44,$44,$44,$54,$54,$6C,$44,$00
.byte $44,$44,$44,$54,$54,$6C,$44,$00

; Tile $43 — 'X'
.byte $44,$44,$28,$10,$28,$44,$44,$00
.byte $44,$44,$28,$10,$28,$44,$44,$00

; Tile $44 — 'Y'
.byte $44,$44,$28,$10,$10,$10,$10,$00
.byte $44,$44,$28,$10,$10,$10,$10,$00

; Tile $45 — 'Z'
.byte $7C,$04,$08,$10,$20,$40,$7C,$00
.byte $7C,$04,$08,$10,$20,$40,$7C,$00

; Tile $46 — '!'
.byte $10,$10,$10,$10,$10,$00,$10,$00
.byte $10,$10,$10,$10,$10,$00,$10,$00

; Tile $47 — '-'
.byte $00,$00,$00,$7C,$00,$00,$00,$00
.byte $00,$00,$00,$7C,$00,$00,$00,$00

; Tile $48 — TILE_LAVA_B frame B (burbujas desplazadas, misma base)
.byte $00,$18,$00,$42,$00,$24,$00,$18   ; plano 0 — burbujas desplazadas
.byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF   ; plano 1 — base completa

; Tile $49 — TILE_LIGHTNING_B frame B (rayo tenue, color 1)
.byte $38,$60,$78,$18,$0C,$08,$08,$00   ; plano 0 — mismo zigzag
.byte $00,$00,$00,$00,$00,$00,$00,$00   ; plano 1 — color 1 (tenue)

; Tiles $4A–$FF — reservados
.res (182 * 16), $00

; ============================================================
; Tabla de patrones 1 ($1000–$1FFF) — sprites
; ============================================================
; Paleta 0 (jugador): color 3 = blanco/plata (armadura)
; Paleta 1 (esqueleto rojo):  color 1 = rojo
; Paleta 2 (esqueleto cian):  color 1 = cian
; Paleta 3 (asustado):        color 1 = azul oscuro
;
; Jugador: plano0 = plano1 = bitmap (color 3 = plata)
; Esqueleto: plano0 = bitmap (color 1), plano1 = $00
; Nota: LEFT usa tile $00/$01 con flip horizontal (attr bit 6)

; ---- Jugador (caballero con armadura) ----

; ---- Jugador (caballero con armadura plateada, cinturón y botas doradas) ----
; Paleta 0: negro / dorado($28) / rojo($16) / blanco-plata($30)
;   p0=1, p1=1 → color3 (plata)   p0=1, p1=0 → color1 (dorado)

; Sprite $00 — Caballero HORIZONTAL frame A
.byte $38,$7E,$FF,$7F,$3C,$44,$44,$66   ; plane0
.byte $38,$7E,$FF,$7E,$00,$44,$44,$00   ; plane1: cinturón y botas → dorado

; Sprite $01 — Caballero HORIZONTAL frame B
.byte $38,$7E,$FE,$7E,$3C,$28,$28,$6C   ; plane0
.byte $38,$7E,$FE,$7E,$00,$28,$28,$00   ; plane1: cinturón y botas → dorado

; Sprite $02 — Caballero ARRIBA frame A
.byte $18,$3C,$7E,$DB,$7E,$44,$44,$C6   ; plane0
.byte $18,$3C,$7E,$DB,$7E,$44,$44,$00   ; plane1: botas doradas

; Sprite $03 — Caballero ARRIBA frame B
.byte $18,$3C,$7E,$FF,$7E,$28,$28,$6C   ; plane0
.byte $18,$3C,$7E,$FF,$7E,$28,$28,$00   ; plane1: botas doradas

; Sprite $04 — Caballero ABAJO frame A
.byte $66,$7E,$FF,$7E,$3C,$44,$44,$C6   ; plane0
.byte $66,$7E,$FF,$7E,$00,$44,$44,$00   ; plane1: cinturón y botas → dorado

; Sprite $05 — Caballero ABAJO frame B
.byte $66,$7E,$7E,$FF,$3C,$28,$28,$6C   ; plane0
.byte $66,$7E,$7E,$FF,$00,$28,$28,$00   ; plane1: cinturón y botas → dorado

; ---- Esqueletos enemigos ----
; Paleta 1/2: negro / rojo o cian (color1) / blanco (color2) / cian o rojo (color3)
;   ojos: p0=1 p1=1 → color3 (brillo cian en pal1, rojo en pal2)
;   huesos: p0=1 p1=0 → color1

; Sprite $06 — Esqueleto NORMAL frame A (ojos brillantes)
.byte $3C,$7E,$FF,$7E,$AA,$3C,$A5,$81   ; plane0: cuencas rellenas ($DB→$FF)
.byte $00,$00,$24,$00,$00,$00,$00,$00   ; plane1: solo píxeles de ojo → color3

; Sprite $07 — Esqueleto NORMAL frame B (ojos brillantes)
.byte $3C,$7E,$FF,$7E,$55,$3C,$5A,$42   ; plane0: cuencas rellenas
.byte $00,$00,$24,$00,$00,$00,$00,$00   ; plane1: solo píxeles de ojo → color3

; Sprite $08 — Esqueleto ASUSTADO frame A
; (encogido, sin cuencas visibles, pies juntos)
;  . . X X X X . .
;  . X X X X X X .   cara tapada
;  . X X X X X X .
;  . X X X X X X .
;  . . X . . X . .   cuerpo encogido
;  . . X X X X . .
;  . . X . . X . .   piernas encogidas
;  . . . X X . . .   pies juntos
.byte $3C,$7E,$7E,$7E,$24,$3C,$24,$18
.byte $00,$00,$00,$00,$00,$00,$00,$00

; Sprite $09 — Esqueleto ASUSTADO frame B (parpadeo advertencia)
; (cuencas asomando — transición a estado normal)
;  . . X X X X . .
;  . X X . . X X .   cuencas asomando
;  . X X X X X X .
;  . X X X X X X .
;  . . X . . X . .
;  . . X X X X . .
;  . . X . . X . .
;  . . . X X . . .
.byte $3C,$66,$7E,$7E,$24,$3C,$24,$18
.byte $00,$00,$00,$00,$00,$00,$00,$00

; Sprite $0A — Muerte explosión frame A (cruz diagonal, color 3)
; X . . . . . . X
; . X . . . . X .
; . . X . . X . .
; . . . X X . . .
; . . . X X . . .
; . . X . . X . .
; . X . . . . X .
; X . . . . . . X
.byte $81,$42,$24,$18,$18,$24,$42,$81
.byte $81,$42,$24,$18,$18,$24,$42,$81

; Sprite $0B — Muerte explosión frame B (rombo, color 3)
; . . . . . . . .
; . . . X X . . .
; . . X X X X . .
; . X X . . X X .
; . X X . . X X .
; . . X X X X . .
; . . . X X . . .
; . . . . . . . .
.byte $00,$18,$3C,$66,$66,$3C,$18,$00
.byte $00,$18,$3C,$66,$66,$3C,$18,$00

; Tiles $0C–$FF — reservados
.res (244 * 16), $00
