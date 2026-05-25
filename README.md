# NES Pac-Man — TFG

Pac-Man modificado para NES desarrollado en C con el compilador **cc65**.  
Incluye obstáculos originales: teletransportadores, paredes temporales, trampas de velocidad y bombas.

---

## Compilar y ejecutar

```bash
# Ajusta CC65_HOME a tu instalación de cc65
make CC65_HOME=C:/cc65

# Lanzar en FCEUX
make run CC65_HOME=C:/cc65
```

Salida: `build/pacman_nes.nes`

---

## Estructura del proyecto

```
NES_Game/
├── Makefile          # Sistema de build (cc65 + ld65)
├── nes.cfg           # Linker script: mapa de memoria NROM
├── lib/              # Librería neslib de Shiru
├── src/
│   ├── core/         # Arranque y bucle principal
│   ├── graphics/     # Tiles CHR y utilidades PPU
│   ├── world/        # Laberinto: datos y renderizado
│   └── entities/     # Pac-Man, fantasmas y obstáculos (Fases 2–5)
└── assets/           # Arte fuente (herramientas externas: YY-CHR, etc.)
```

---

## Módulos

### `lib/` — Librería neslib

| Archivo | Descripción |
|---------|-------------|
| `neslib.h` | Declaraciones públicas de neslib: paleta, PPU, OAM, pad, VRAM, audio y utilidades. Incluye un guard `#ifndef __CC65__` que define `__fastcall__` como vacío para que IntelliSense lo acepte sin errores. |
| `neslib.s` | Implementación en ensamblador 6502 de todas las funciones de neslib: handler NMI, actualización de OAM por DMA, sistema de paleta con brillo virtual, lectura de pad con corrección de errores, actualización de VRAM por VBlank, scroll, generador aleatorio y utilidades de memoria. |
| `famitone2.s` | Stub del motor de audio FamiTone2. Define las variables de zero page que neslib necesita internamente (punteros, contadores de frame, estado del pad, scroll, etc.), las constantes `OAM_BUF = $0200` y `PAL_BUF = $0300`, y versiones vacías de las funciones de música/SFX. Se reemplazará por el famitone2.s completo en la Fase 7 (audio). |

---

### `src/core/` — Arranque y bucle principal

| Archivo | Descripción |
|---------|-------------|
| `crt0.s` | Startup de la NES. Contiene la **cabecera iNES** (mapper 0, 32 KB PRG, 8 KB CHR), el **handler de reset** (deshabilita IRQs, limpia los 2 KB de RAM, espera dos VBlanks hasta que la PPU se estabiliza, copia DATA a RAM y pone a cero BSS, inicializa `PPU_CTRL_VAR` y `NTSC_MODE` de neslib, habilita NMI y llama a `main()`), y la **tabla de vectores** en `$FFFA` apuntando a los handlers de neslib. |
| `nes.h` | Definiciones de los **registros hardware** de la NES accesibles desde C: registros PPU (`$2000`–`$2007`, `$4014`), registros APU (`$4000`–`$4017`) y sus bits de control. No redefine botones ni macros que ya provee `neslib.h`. |
| `main.c` | Punto de entrada del juego. En la **inicialización**: deshabilita renderizado (`ppu_off`), carga paleta de 32 bytes, limpia el nametable, llama a `map_render()` y habilita renderizado (`ppu_on_all`). El **bucle principal** espera cada frame con `ppu_wait_frame()` y lee el pad con `pad_poll(0)`. La lógica de juego se añadirá a partir de la Fase 2. |

---

### `src/graphics/` — Tiles CHR y utilidades gráficas

| Archivo | Descripción |
|---------|-------------|
| `tiles.s` | Datos CHR de 8 KB en el segmento `CHARS`. Tabla de patrones 0 (`$0000`–`$0FFF`, fondo): `$00` suelo vacío, `$01` pared azul sólida, `$02` punto pequeño, `$03` power pellet circular. Tabla de patrones 1 (`$1000`–`$1FFF`, sprites): Pac-Man en cuatro direcciones (`$00`–`$04`), cuerpo y ojos de fantasma (`$05`–`$06`), fantasma asustado (`$07`). |
| `ppu.h` | Cabecera de compatibilidad: re-exporta `neslib.h`. Permite que módulos futuros incluyan `ppu.h` sin cambios si se añaden helpers propios. |
| `ppu.c` | Vacío. Las funciones PPU previas a neslib (`ppu_write`, `ppu_fill`, `ppu_load_palette`) fueron reemplazadas por `vram_adr`, `vram_fill` y `pal_all` de neslib. |

---

### `src/world/` — Laberinto

| Archivo | Descripción |
|---------|-------------|
| `map.h` | Constantes del laberinto (`MAP_COLS = 28`, `MAP_ROWS = 28`, offsets en nametable, `MAP_TOTAL_DOTS = 240`), índices de tile (`TILE_EMPTY/WALL/DOT/PELLET`), declaración de `MAP_DATA[28][28]` y de `map_render()`. |
| `map.c` | Array `MAP_DATA[28][28]` en ROM: laberinto simétrico estilo Pac-Man clásico con cuatro power pellets en las esquinas interiores y zona central para la casa de los fantasmas (filas 10–16). `map_render()` usa `vram_adr()` + `vram_write()` de neslib para volcar cada fila al nametable en su posición correcta (`NTADR_A`). |

---

### `src/entities/` — Entidades del juego *(Fases 2–5)*

Vacío actualmente. Contendrá:

| Archivo futuro | Contenido |
|----------------|-----------|
| `player.c/.h` | Estructura `Player`, movimiento tile-based, animación y colisión con paredes. |
| `ghost.c/.h` | Estructura `Ghost`, máquina de estados (NORMAL / SCARED / DEAD), IA perseguidora (BFS) e IA patrullera. |
| `obstacles.c/.h` | Teletransportadores, paredes temporales, trampas de velocidad y bombas. |
| `sound.c/.h` | Efectos de sonido via APU con FamiTone2 completo. |

---

## Mapa de memoria NES

| Rango | Uso |
|-------|-----|
| `$0000–$00FF` | Zero page — variables internas de neslib y runtime cc65 |
| `$0100–$01FF` | Pila 6502 |
| `$0200–$02FF` | `OAM_BUF` — buffer de sprites (DMA a PPU en cada VBlank) |
| `$0300–$031F` | `PAL_BUF` — buffer de paleta (32 bytes) |
| `$0320–$07FF` | BSS / DATA — variables y estado del juego |
| `$8000–$FFFF` | PRG-ROM (32 KB) — código y datos constantes |
| CHR `$0000–$1FFF` | CHR-ROM (8 KB) — tiles de fondo y sprites |

---

## Restricciones técnicas de la NES

- **CPU:** 6502 a 1.79 MHz — evitar operaciones costosas en el bucle principal.
- **RAM:** 2 KB totales — estructuras de datos mínimas.
- **Sprites:** máximo 64 en pantalla; máximo 8 por línea horizontal.
- **Paleta:** 4 subpaletas de 3 colores para fondo + 4 para sprites.
- **VRAM:** solo accesible de forma segura durante el VBlank (~2 273 ciclos).
- **CHR-ROM:** 8 KB fijos; tiles de fondo y sprites comparten el espacio.
