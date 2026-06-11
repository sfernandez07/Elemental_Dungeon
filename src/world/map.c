#include "neslib.h"
#include "map.h"

#define W TILE_WALL
#define D TILE_DOT
#define P TILE_PELLET
#define E TILE_EMPTY
#define T TILE_TELEPORT
#define S TILE_SLOW_TRAP
#define B TILE_BOMB
#define O TILE_DOOR_CLOSED
#define V TILE_VOLCANO
#define R TILE_TREE
#define WW TILE_WHIRLWIND
#define BU TILE_BUBBLE

/* ================================================================== */
/* Nivel 0 — Agua (mapa original)                                     */
/* ================================================================== */
static const uint8_t MAP_0[MAP_ROWS][MAP_COLS] = {
/*col 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27*/
/*00*/{W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
/*01*/{W, D, D, D, D, D, BU, D, D, D, D, D, D, W, W, D, D, D, D, D, D, BU, D, D, D, D, D, W},
/*02*/{W, D, W, W, W, W, D, W, W, W, W, W, D, W, W, D, W, W, W, W, W, D, W, W, W, W, D, W},
/*03*/{W, P, W, E, E, W, D, W, E, E, E, W, D, W, W, D, W, E, E, E, W, D, W, E, E, W, P, W},
/*04*/{W, D, W, W, W, W, D, W, W, W, W, W, D, W, W, D, W, W, W, W, W, D, W, W, W, W, D, W},
/*05*/{W, T, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, T, W},
/*06*/{W, D, W, W, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, W, W, D, W},
/*07*/{W, D, W, W, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, W, W, D, W},
/*08*/{W, D, D, S, D, D, D, W, W, D, D, D, D, W, W, D, D, D, D, W, W, D, D, D, S, D, D, W},
/*09*/{W, W, W, W, W, W, D, W, W, W, W, W, E, W, W, E, W, W, W, W, W, D, W, W, W, W, W, W},
/*10*/{E, E, E, E, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, E, E, E, E},
/*11*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*12*/{E, E, E, E, W, W, D, E, E, E, W, E, E, E, E, E, E, W, E, E, E, D, W, W, E, E, E, E},
/*13*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*14*/{W, W, W, W, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, W, W, W, W},
/*15*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*16*/{E, E, E, E, W, W, D, W, W, E, E, E, E, W, W, E, E, E, E, W, W, D, W, W, E, E, E, E},
/*17*/{W, D, D, D, D, D, D, D, D, D, D, D, B, W, W, D, D, D, D, D, D, D, D, D, D, D, D, W},
/*18*/{W, D, W, W, W, W, D, W, W, W, W, W, D, W, W, D, W, W, W, W, W, D, W, W, W, W, D, W},
/*19*/{W, D, W, W, W, W, D, W, W, W, W, W, D, W, W, D, W, W, W, W, W, D, W, W, W, W, D, W},
/*20*/{W, P, D, D, W, W, D, D, D, D, D, D, D, E, E, D, D, D, D, D, D, D, W, W, D, D, P, W},
/*21*/{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/*22*/{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/*23*/{W, D, D, S, D, D, D, W, W, D, D, D, D, W, W, D, D, D, D, W, W, D, D, D, S, D, D, W},
/*24*/{W, D, W, W, W, W, W, W, W, W, W, W, D, W, W, D, W, W, W, W, W, W, W, W, W, W, D, W},
/*25*/{W, D, W, W, W, W, W, W, W, W, W, W, D, W, W, D, W, W, W, W, W, W, W, W, W, W, D, W},
/*26*/{W, D, D, D, D, D, BU, D, D, D, D, D, D, D, D, D, D, D, D, D, D, BU, D, D, D, D, D, W},
/*27*/{W, W, W, W, W, W, W, W, W, W, W, W, W, O, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
};

/* ================================================================== */
/* Nivel 1 — Viento: pasillos abiertos, pocos muros, simétrico        */
/* ================================================================== */
static const uint8_t MAP_1[MAP_ROWS][MAP_COLS] = {
/*col 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27*/
/*00*/{W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
/*01*/{W, D, D, D, D, D, D, D, D, D, D, D, D, WW, D, D, D, D, D, D, D, D, D, D, D, D, D, W},
/*02*/{W, D, W, W, D, W, D, W, W, D, W, D, W, D, W, D, W, D, W, D, W, W, D, W, D, W, D, W},
/*03*/{W, P, W, E, D, W, D, E, W, D, W, E, D, E, W, E, D, W, E, D, W, E, D, W, D, E, P, W},
/*04*/{W, D, W, W, D, W, D, W, W, D, W, D, W, D, W, D, W, D, W, D, W, W, D, W, D, W, D, W},
/*05*/{W, T, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, T, W},
/*06*/{W, D, W, W, D, W, D, W, W, D, W, D, W, D, W, D, W, D, W, D, W, W, D, W, D, W, D, W},
/*07*/{W, D, W, E, D, W, D, E, W, D, W, E, D, E, D, E, D, W, E, D, W, E, D, W, D, E, D, W},
/*08*/{W, D, D, S, D, D, D, D, WW, D, D, D, D, D, D, D, D, D, D, WW, D, D, D, D, D, S, D, W},
/*09*/{W, W, W, W, W, W, D, W, W, W, W, W, E, W, W, E, W, W, W, W, W, W, D, W, W, W, W, W},
/*10*/{E, E, E, E, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, E, E, E, E},
/*11*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*12*/{E, E, E, E, W, W, D, E, E, E, W, E, E, E, E, E, E, W, E, E, E, D, W, W, E, E, E, E},
/*13*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*14*/{W, W, W, W, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, W, W, W, W},
/*15*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*16*/{E, E, E, E, W, W, D, W, W, E, E, E, E, W, W, E, E, E, E, W, W, D, W, W, E, E, E, E},
/*17*/{W, D, D, D, D, D, D, D, D, WW, D, D, B, W, W, D, D, D, D, D, D, D, D, D, D, D, D, W},
/*18*/{W, D, W, W, D, W, D, W, W, D, W, D, W, D, W, D, W, D, W, D, W, W, D, W, D, W, D, W},
/*19*/{W, D, W, E, D, W, D, E, W, D, W, E, D, E, D, E, D, W, E, D, W, E, D, W, D, E, D, W},
/*20*/{W, P, D, D, W, W, D, D, D, D, D, D, D, E, E, D, D, D, D, D, D, D, W, W, D, D, P, W},
/*21*/{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/*22*/{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/*23*/{W, D, D, S, D, D, D, W, W, D, D, D, D, W, W, D, D, D, D, W, W, D, D, D, S, D, D, W},
/*24*/{W, D, W, W, W, W, W, W, W, W, W, W, D, W, W, D, W, W, W, W, W, W, W, W, W, W, D, W},
/*25*/{W, D, W, E, E, E, E, E, E, E, E, D, W, W, W, W, D, E, E, E, E, E, E, E, E, W, D, W},
/*26*/{W, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, W},
/*27*/{W, W, W, W, W, W, W, W, W, W, W, W, W, O, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
};

/* ================================================================== */
/* Nivel 2 — Bosque: laberinto denso, muchas celdas pequeñas          */
/* ================================================================== */
static const uint8_t MAP_2[MAP_ROWS][MAP_COLS] = {
/*col 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27*/
/*00*/{W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
/*01*/{W, D, D, D, W,  R, D, D, W, D, D, D, D, D, D, D, D, D, D, W, D, D,  R, W, D, D, D, W},
/*02*/{W, D, W, D, W, D, W, D, W, D, W, W, D, W, W, D, W, W, D, W, D, W, D, W, D, W, D, W},
/*03*/{W, P, D, D, D, D, W, D, D, D, W, D, D, W, E, W, D, D, W, D, D, D, D, D, D, D, P, W},
/*04*/{W, W, W, D, W, W, W, W, D, W, W, D, W, W, W, W, D, W, W, D, W, W, W, W, D, W, W, W},
/*05*/{W, T, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, T, W},
/*06*/{W, D, W, W, D, W, W, D, W, W, D, W, W, D, W, D, W, W, D, W, W, D, W, W, D, W, D, W},
/*07*/{W, D, W, D, D, W, D, D, W, D, D, W, D, D, W, D, D, W, D, D, W, D, D, W, D, W, D, W},
/*08*/{W, D, D, S, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, S, D, W},
/*09*/{W, W, W, W, W, W, D, W, W, W, W, W, E, W, W, E, W, W, W, W, W, W, D, W, W, W, W, W},
/*10*/{E, E, E, E, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, E, E, E, E},
/*11*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*12*/{E, E, E, E, W, W, D, E, E, E, W, E, E, E, E, E, E, W, E, E, E, D, W, W, E, E, E, E},
/*13*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*14*/{W, W, W, W, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, W, W, W, W},
/*15*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*16*/{E, E, E, E, W, W, D, W, W, E, E, E, E, W, W, E, E, E, E, W, W, D, W, W, E, E, E, E},
/*17*/{W, D, D, D, D,  R, D, D, D, D, D, D, B, W, W, D, D, D, D, D, D, D,  R, D, D, D, D, W},
/*18*/{W, D, W, W, D, W, W, D, W, D, D, W, W, D, W, D, W, W, D, D, W, D, W, W, D, W, D, W},
/*19*/{W, D, W, D, D, W, D, D, W, D, W, D, D, W, E, W, D, D, W, D, D, W, D, D, W, D, D, W},
/*20*/{W, P, D, D, W, W, D, D, D, D, D, D, D, E, E, D, D, D, D, D, D, D, W, W, D, D, P, W},
/*21*/{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/*22*/{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/*23*/{W, D, D, S, D, D, D, W, W, D, D, D, D, W, W, D, D, D, D, W, W, D, D, D, S, D, D, W},
/*24*/{W, D, W, D, W, W, W, W, W, W, W, D, D, W, W, D, D, W, W, W, W, W, W, D, W, W, D, W},
/*25*/{W, D, D, D, W, E, E, E, E, E, E, D, W, W, W, W, D, E, E, E, E, E, E, W, D, D, D, W},
/*26*/{W, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, W},
/*27*/{W, W, W, W, W, W, W, W, W, W, W, W, W, O, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
};

/* ================================================================== */
/* Nivel 3 — Fuego: anillo exterior, centro abierto, trampas          */
/* ================================================================== */
static const uint8_t MAP_3[MAP_ROWS][MAP_COLS] = {
/*col 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27*/
/*00*/{W, W, W, W, W, V, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, V, W, W, W, W, W},
/*01*/{W, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, W},
/*02*/{W, D, W, W, W, D, W, W, W, D, W, W, D, W, W, D, W, W, D, W, W, W, D, W, W, W, D, W},
/*03*/{W, P, W, E, E, D, W, E, E, D, E, D, E, E, W, E, E, D, E, E, W, E, D, E, E, W, P, W},
/*04*/{W, D, W, W, W, D, W, W, W, D, W, W, D, W, W, D, W, W, D, W, W, W, D, W, W, W, D, W},
/*05*/{W, T, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, T, W},
/*06*/{W, D, W, W, D, W, D, W, W, D, W, W, D, D, D, D, D, W, W, D, W, D, W, W, D, W, D, W},
/*07*/{W, D, W, E, D, W, D, E, W, D, W, E, W, E, E, E, W, E, D, W, D, E, W, E, D, W, D, W},
/*08*/{V, D, S, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, S, W},
/*09*/{W, W, W, W, W, W, D, W, W, W, W, W, E, W, W, E, W, W, W, W, W, W, D, W, W, W, W, W},
/*10*/{E, E, E, E, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, E, E, E, E},
/*11*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*12*/{E, E, E, E, W, W, D, E, E, E, W, E, E, E, E, E, E, W, E, E, E, D, W, W, E, E, E, E},
/*13*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*14*/{W, W, W, W, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, W, W, W, W},
/*15*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*16*/{E, E, E, E, W, W, D, W, W, E, E, E, E, W, W, E, E, E, E, W, W, D, W, W, E, E, E, E},
/*17*/{W, D, D, D, D, D, D, D, D, D, D, D, B, W, W, D, D, D, D, D, D, D, D, D, D, D, D, W},
/*18*/{W, D, W, W, D, W, D, W, W, D, W, W, D, D, D, D, D, W, W, D, W, D, W, W, D, W, D, W},
/*19*/{W, D, W, E, D, W, D, E, W, D, W, E, W, E, E, E, W, E, D, W, D, E, W, E, D, W, D, W},
/*20*/{W, P, D, D, W, W, D, D, D, D, D, D, D, E, E, D, D, D, D, D, D, D, W, W, D, D, P, V},
/*21*/{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/*22*/{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/*23*/{W, D, S, D, D, D, D, W, W, D, D, D, D, W, W, D, D, D, D, W, W, D, D, D, D, S, D, W},
/*24*/{W, D, W, W, W, W, W, W, W, W, W, W, D, W, W, D, W, W, W, W, W, W, W, W, W, W, D, W},
/*25*/{W, D, W, E, E, E, D, E, D, E, E, E, W, W, W, W, E, E, E, D, E, D, E, E, E, W, D, W},
/*26*/{W, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, W},
/*27*/{W, W, W, W, W, W, W, W, W, W, W, W, W, O, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
};

/* ================================================================== */
/* Nivel 4 — Rayo: pasillos en zigzag, teleportadores clave           */
/* ================================================================== */
static const uint8_t MAP_4[MAP_ROWS][MAP_COLS] = {
/*col 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27*/
/*00*/{W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
/*01*/{W, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, W},
/*02*/{W, D, W, D, W, D, W, D, W, D, W, D, W, D, W, D, W, D, W, D, W, D, W, D, W, D, D, W},
/*03*/{W, P, D, W, D, D, D, W, D, D, D, W, E, D, W, D, E, W, D, D, D, W, D, D, D, W, P, W},
/*04*/{W, D, D, W, W, D, W, W, D, W, W, D, W, D, W, D, W, D, W, W, D, W, W, D, W, W, D, W},
/*05*/{W, T, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, T, W},
/*06*/{W, D, W, W, W, W, D, W, W, W, W, D, W, D, W, D, W, D, W, W, W, W, D, W, W, W, D, W},
/*07*/{W, D, W, E, E, W, D, W, E, E, W, D, W, E, D, E, W, D, W, E, E, W, D, W, E, E, D, W},
/*08*/{W, D, D, S, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, S, D, W},
/*09*/{W, W, W, W, W, W, D, W, W, W, W, W, E, W, W, E, W, W, W, W, W, W, D, W, W, W, W, W},
/*10*/{E, E, E, E, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, E, E, E, E},
/*11*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*12*/{E, E, E, E, W, W, D, E, E, E, W, E, E, E, E, E, E, W, E, E, E, D, W, W, E, E, E, E},
/*13*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*14*/{W, W, W, W, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, W, W, W, W},
/*15*/{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/*16*/{E, E, E, E, W, W, D, W, W, E, E, E, E, W, W, E, E, E, E, W, W, D, W, W, E, E, E, E},
/*17*/{W, D, D, D, D, D, D, D, D, D, D, D, B, W, W, D, D, D, D, D, D, D, D, D, D, D, D, W},
/*18*/{W, D, W, W, W, W, D, W, W, W, W, D, W, D, W, D, W, D, W, W, W, W, D, W, W, W, D, W},
/*19*/{W, D, W, E, E, W, D, W, E, E, W, D, W, E, D, E, W, D, W, E, E, W, D, W, E, E, D, W},
/*20*/{W, P, D, D, W, W, D, D, D, D, D, D, D, E, E, D, D, D, D, D, D, D, W, W, D, D, P, W},
/*21*/{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/*22*/{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/*23*/{W, D, D, S, D, D, D, W, W, D, D, D, D, W, W, D, D, D, D, W, W, D, D, D, S, D, D, W},
/*24*/{W, D, W, W, W, W, W, W, W, W, W, W, D, W, W, D, W, W, W, W, W, W, W, W, W, W, D, W},
/*25*/{W, D, W, E, E, W, D, D, D, E, E, E, W, W, W, W, E, E, E, D, D, D, W, E, E, W, D, W},
/*26*/{W, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, W},
/*27*/{W, W, W, W, W, W, W, W, W, W, W, W, W, O, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
};

#undef W
#undef D
#undef P
#undef E
#undef T
#undef S
#undef B
#undef O
#undef V
#undef R
#undef WW
#undef BU

/* Tabla de punteros a los 5 mapas */
static const uint8_t (* const LEVELS[LEVEL_COUNT])[MAP_COLS] = {
    MAP_0, MAP_1, MAP_2, MAP_3, MAP_4
};

/* ------------------------------------------------------------------ */
/* Estado mutable en RAM                                               */
/* ------------------------------------------------------------------ */

unsigned char map_state[MAP_ROWS][MAP_COLS];
unsigned char dots_remaining;
unsigned char door_open;
unsigned char door_dirty;
unsigned char current_level;

/* ------------------------------------------------------------------ */
/* map_init                                                            */
/* ------------------------------------------------------------------ */

void map_init(void)
{
    const uint8_t (*src)[MAP_COLS] = LEVELS[current_level];
    unsigned char r, c;

    dots_remaining = 0;
    door_open  = 0;
    door_dirty = 0;

    for (r = 0; r < MAP_ROWS; r++) {
        for (c = 0; c < MAP_COLS; c++) {
            map_state[r][c] = src[r][c];
            if (src[r][c] == TILE_DOT || src[r][c] == TILE_PELLET)
                dots_remaining++;
        }
    }
    /* La puerta se bloquea como pared hasta que se abra */
    map_state[DOOR_TY][DOOR_TX] = TILE_WALL;
}

/* ------------------------------------------------------------------ */
/* map_render                                                          */
/* ------------------------------------------------------------------ */

void map_render(void)
{
    const uint8_t (*src)[MAP_COLS] = LEVELS[current_level];
    uint8_t row;
    unsigned int addr;
    for (row = 0; row < MAP_ROWS; row++) {
        addr = NTADR_A(MAP_X_OFFSET, MAP_Y_OFFSET + row);
        vram_adr(addr);
        vram_write((unsigned char *)src[row], MAP_COLS);
    }
}

/* ------------------------------------------------------------------ */
/* map_eat_dot                                                         */
/* ------------------------------------------------------------------ */

void map_open_door(void)
{
    door_open  = 1;
    door_dirty = 1;
    map_state[DOOR_TY][DOOR_TX] = TILE_DOOR_OPEN;
}

unsigned char map_eat_dot(unsigned char tx, unsigned char ty)
{
    unsigned char tile = map_state[ty][tx];
    if (tile == TILE_DOT || tile == TILE_PELLET) {
        map_state[ty][tx] = TILE_EMPTY;
        if (dots_remaining > 0) dots_remaining--;
    } else if (tile == TILE_BOMB || tile == TILE_TREE ||
               tile == TILE_WHIRLWIND || tile == TILE_BUBBLE) {
        map_state[ty][tx] = TILE_EMPTY;
    }
    return tile;
}
