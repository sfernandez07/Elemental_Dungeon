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

/* Datos del laberinto en ROM (28×28 bytes) */
extern const uint8_t MAP_DATA[MAP_ROWS][MAP_COLS];

/* Estado mutable del laberinto en RAM (puntos ya comidos = TILE_EMPTY) */
extern unsigned char map_state[MAP_ROWS][MAP_COLS];

/* Contador de puntos y pellets que quedan; 0 = victoria */
extern unsigned char dots_remaining;

/* Total de puntos del laberinto (precalculado para saber cuándo ganamos) */
#define MAP_TOTAL_DOTS  244   /* 240 dots + 4 pellets */

/* Inicializa map_state copiando MAP_DATA y cuenta dots_remaining. */
void map_init(void);

/* Copia todos los tiles del laberinto al nametable de la PPU.
   Llamar solo con el renderizado deshabilitado. */
void map_render(void);

/* Intenta comer el tile en (tx,ty). Devuelve el tipo de tile encontrado
   (TILE_DOT, TILE_PELLET) o TILE_EMPTY si ya estaba vacío/es pared. */
unsigned char map_eat_dot(unsigned char tx, unsigned char ty);

#endif
