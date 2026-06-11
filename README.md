# Elemental Dungeon — TFG

Juego original para la consola NES (Nintendo Entertainment System) desarrollado íntegramente en C con el compilador **cc65** y ensamblador 6502. El jugador controla a un caballero que debe recorrer mazmorras elementales recogiendo puntos y esquivando esqueletos enemigos.

---

## Características del juego

- Laberinto de **28 × 28 tiles** con 240 puntos y 4 power pellets parpadeantes
- **Caballero** con movimiento tile-based, animación en 4 direcciones y detección de colisiones
- **4 esqueletos enemigos** con dos inteligencias artificiales diferenciadas:
  - *Perseguidor (Chaser)*: minimiza la distancia Manhattan hacia el héroe
  - *Patrullero (Patroller)*: recorre el mapa con la regla de la mano derecha
- **Modo asustado** activado por power pellet, con parpadeo al quedar < 1 segundo
- **Sistema de puntuación** (puntos, pellets, esqueletos y bomba)
- **5 mundos temáticos** con paletas y obstáculos propios:
  | Mundo | Elemento | Obstáculo exclusivo |
  |-------|----------|---------------------|
  | Nivel 1 | Agua | Burbujas — congelan al héroe 1 s |
  | Nivel 2 | Viento | Remolinos — invierten los controles 5 s |
  | Nivel 3 | Bosque | Árboles — congelan al héroe 1 s |
  | Nivel 4 | Fuego | Volcanes — erupciones de lava que bloquean el paso |
  | Nivel 5 | Rayo | Rayos — muerte instantánea al pisarlos |
- **Obstáculos comunes** en todos los niveles:
  | Obstáculo | Comportamiento |
  |-----------|----------------|
  | Teletransportador | Par de portales A ↔ B; cooldown de 16 frames |
  | Pared temporal | Toggle cada 2 s; bloquea a héroes y enemigos |
  | Trampa de velocidad | Ralentiza al héroe al 50 % |
  | Bomba | Destruye todos los esqueletos visibles durante ~3 s |
- **Sistema de vidas** (3 vidas; icono pellet en HUD)
- **Música de fondo** en bucle (canal Triangle + Noise del APU) con pausa y reanudación
- **Pantallas de intro** por nivel con símbolo temático y borde decorativo
- **Máquina de estados** completa: Título → Intro nivel → Jugando → Muerto → Game Over / Victoria → Título
- **Audio**: música de fondo + 5 efectos de sonido sobre el canal Pulse 1 del APU

---

## Controles

| Botón | Acción |
|-------|--------|
| ← ↑ → ↓ | Mover al héroe |
| **Start** | Iniciar partida / Pausar / Continuar |

---

## Compilar y ejecutar

### Requisitos

- [cc65](https://cc65.github.io/) instalado en `C:\cc65` (ajustable en `build.bat`)
- Emulador NES NTSC: [FCEUX](https://fceux.com/) o [Mesen](https://www.mesen.ca/)

### Build (Windows)

```bat
build.bat
```

El ROM compilado queda en `build/elemental_dungeon.nes`.

---

## Estructura del proyecto

```
NES_Game/
├── build.bat                 ← Script de compilación (cc65 → ca65 → ld65)
├── nes.cfg                   ← Linker script NROM (32 KB PRG, 8 KB CHR)
├── lib/
│   ├── neslib.h / neslib.s   ← Biblioteca hardware NES (Shiru, dominio público)
│   └── famitone2.s           ← Variables de zero page y stubs de audio
└── src/
    ├── core/
    │   ├── crt0.s            ← Cabecera iNES, reset, vectores NMI/IRQ/RESET
    │   ├── main.c            ← Bucle principal (~60 Hz NTSC)
    │   ├── game.h / game.c   ← Máquina de estados, vidas, pantallas de texto
    │   ├── sound.h / sound.c ← Driver APU (música Triangle+Noise + 5 SFX Pulse 1)
    │   └── text.h / text.c   ← Escritura de texto en nametable
    ├── world/
    │   ├── map.h / map.c     ← Laberinto ROM + estado RAM mutable, dot counter
    │   └── score.h / score.c ← Puntuación BCD y actualización VRAM por HUD
    ├── entities/
    │   ├── player.h / player.c   ← Héroe: movimiento, colisión, animación
    │   ├── ghost.h / ghost.c     ← Esqueletos: IA, colisión, modo asustado
    │   └── obstacles.h / obstacles.c ← Obstáculos por nivel y comunes
    └── graphics/
        └── tiles.s           ← CHR-ROM 8 KB (tiles BG + sprites)
```

---

## Arquitectura técnica

### Mapa de memoria NES

| Rango | Uso |
|-------|-----|
| `$0000–$00FF` | Zero page — variables internas de neslib y runtime cc65 |
| `$0100–$01FF` | Pila 6502 |
| `$0200–$02FF` | `OAM_BUF` — buffer de sprites (DMA a PPU en cada VBlank) |
| `$0300–$031F` | `PAL_BUF` — buffer de paleta (32 bytes) |
| `$0320–$07FF` | BSS / DATA — estado del juego |
| `$8000–$FFFF` | PRG-ROM (32 KB) — código C compilado + datos constantes |
| CHR `$0000–$1FFF` | CHR-ROM (8 KB) — tabla BG (`$0000`) y tabla Sprites (`$1000`) |

### Uso de RAM (estimado)

| Estructura | Tamaño |
|------------|--------|
| `map_state[28][28]` (estado mutable del laberinto) | 784 B |
| `Ghost ghosts[4]` | 44 B |
| `Player player` | 12 B |
| `vram_buf[160]` | 160 B |
| Variables de juego y flags | ~30 B |
| Stack + runtime cc65 | ~128 B |
| **Total** | **~1 158 B / 2 048 B** |

### Sistema de actualización VRAM

Las escrituras a VRAM solo son seguras durante el VBlank (~2 273 ciclos).
Se usa el sistema `set_vram_update` de neslib con un buffer de 160 bytes:

```
score(8) + vidas(6) + dot(3) + puerta(3) + pellets_blink(12) + obstáculos(≤86) + EOF(1)
```

### CHR-ROM — tiles definidos

| Rango | Contenido |
|-------|-----------|
| `$00` | Suelo vacío |
| `$01` | Pared (marco color1, relleno color3) |
| `$02` | Punto 4×4 px (color 2) |
| `$03` | Power pellet 6×6 px (color 3, parpadea) |
| `$04–$0D` | Dígitos '0'–'9' (color 3, brillante) |
| `$0E–$11` | Teleportador, pared temporal, trampa velocidad, bomba |
| `$24–$25` | Puerta cerrada / abierta |
| `$26–$2B` | Obstáculos temáticos: volcán, lava×2, árbol, remolino, burbuja, rayo×2 |
| `$2C–$47` | Alfabeto A–Z, '!', '-' (color 3, brillante) |
| `$1000–$107F` | Sprites: héroe ×6, esqueleto ×4, explosión ×2 |

---

## Restricciones técnicas de la NES

- **CPU:** Ricoh 2A03 (6502 @ 1.79 MHz NTSC) — sin multiplicación ni división hardware
- **RAM:** 2 KB totales — el mayor consumidor es `map_state` con 784 B
- **Sprites:** máximo 64 en pantalla; máximo 8 por línea horizontal
- **Paleta:** 4 subpaletas BG + 4 subpaletas Sprite, cada una de 3 colores + transparente
- **VBlank:** única ventana segura para escribir en PPU (~2 273 ciclos a 1.79 MHz)
- **Mapper:** NROM (0) — sin bank switching; todo el código cabe en 32 KB PRG

---

## Créditos

- **neslib** — Shiru (dominio público)
- **FamiTone2** — Shiru (dominio público)
- **Compilador cc65** — licencia zlib
