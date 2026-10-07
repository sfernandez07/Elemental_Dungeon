#ifndef MAP_H
#define MAP_H

#include <stdint.h>

/* Dimensiones del laberinto en tiles */
#define MAP_COLS       28
#define MAP_ROWS       28

/* Posición del laberinto dentro del nametable (32×30 tiles) */
#define MAP_X_OFFSET    2   /* columna inicial en el nametable */
#define MAP_Y_OFFSET    1   /* fila inicial en el nametable    */

/* Índices de tile — fondo (CHR tabla 0) */
#define TILE_EMPTY     0x00
#define TILE_WALL      0x01
#define TILE_DOT       0x02
#define TILE_PELLET    0x03
/* Tiles de obstáculos (Fase 5) */
#define TILE_TELEPORT  0x0E   /* teletransportador (par A ↔ B) */
#define TILE_TEMP_WALL 0x0F   /* pared temporal (activa = bloqueo, inactiva = paso) */
#define TILE_SLOW_TRAP 0x10   /* trampa de velocidad */
#define TILE_BOMB      0x11   /* bomba recogible */

/* Tiles de obstáculos por mapa (Fases 1-5) */
#define TILE_VOLCANO   0x26   /* volcán en pared (bloquea como muro) */
#define TILE_LAVA      0x27   /* lava activa (bloquea jugador y fantasmas) */
#define TILE_TREE      0x28   /* árbol: congela jugador 60 frames al pisar */
#define TILE_WHIRLWIND 0x29   /* remolino: invierte controles 300 frames al pisar */
#define TILE_BUBBLE    0x2A   /* burbuja: congela jugador 60 frames al pisar */
#define TILE_LIGHTNING 0x2B   /* rayo: muerte instantánea al pisar */
#define TILE_LAVA_B    0x48   /* lava frame B (animación de burbujas) */
#define TILE_LIGHTNING_B 0x49 /* rayo frame B (parpadeo tenue) */

/* Puerta de salida */
#define TILE_DOOR_CLOSED  0x24   /* visual inicial; en map_state se almacena como TILE_WALL */
#define TILE_DOOR_OPEN    0x25   /* pasable por el jugador, bloquea a los enemigos */
#define DOOR_TX  13
#define DOOR_TY  27

/* Posiciones de los obstáculos (coordenadas de tile) */
#define TELE_A_TX   1
#define TELE_A_TY   5
#define TELE_B_TX  26
#define TELE_B_TY   5

#define TEMPWALL_TX0  12
#define TEMPWALL_TY0   9
#define TEMPWALL_TX1  15
#define TEMPWALL_TY1   9

#define BOMB_TX  12
#define BOMB_TY  17

/* Conversión tile ↔ píxel de pantalla */
#define TX_TO_PX(tx)  ((unsigned char)(((tx) + MAP_X_OFFSET) << 3))
#define TY_TO_PY(ty)  ((unsigned char)(((ty) + MAP_Y_OFFSET) << 3))
#define PX_TO_TX(px)  (((unsigned char)(px) >> 3) - MAP_X_OFFSET)
#define PY_TO_TY(py)  (((unsigned char)(py) >> 3) - MAP_Y_OFFSET)

/* Estado mutable del laberinto en RAM (puntos ya comidos = TILE_EMPTY) */
extern unsigned char map_state[MAP_ROWS][MAP_COLS];

/* Contador de puntos y pellets que quedan */
extern unsigned int  dots_remaining;

/* Estado de la puerta de salida */
extern unsigned char door_open;    /* 1 = puerta abierta (todos los puntos comidos) */
extern unsigned char door_dirty;   /* 1 = hay que actualizar el tile en VRAM */

/* Número de niveles disponibles */
#define LEVEL_COUNT     5

/* Nivel actualmente cargado (0–LEVEL_COUNT-1) */
extern unsigned char current_level;

/* Inicializa map_state copiando MAP_DATA y cuenta dots_remaining. */
void map_init(void);

/* Copia todos los tiles del laberinto al nametable de la PPU.
   Llamar solo con el renderizado deshabilitado. */
void map_render(void);

/* Intenta comer el tile en (tx,ty). Devuelve el tipo de tile encontrado
   (TILE_DOT, TILE_PELLET) o TILE_EMPTY si ya estaba vacío/es pared. */
unsigned char map_eat_dot(unsigned char tx, unsigned char ty);

/* Abre la puerta de salida: actualiza map_state, door_open y door_dirty. */
void map_open_door(void);

#endif
