#include "neslib.h"
#include "map.h"

#define W TILE_WALL
#define D TILE_DOT
#define P TILE_PELLET
#define E TILE_EMPTY
#define T TILE_TELEPORT   /* teletransportador */
#define S TILE_SLOW_TRAP  /* trampa de velocidad */
#define B TILE_BOMB       /* bomba */

const uint8_t MAP_DATA[MAP_ROWS][MAP_COLS] = {
/* col: 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 */
/* 00 */{W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
/* 01 */{W, D, D, D, D, D, D, D, D, D, D, D, D, W, W, D, D, D, D, D, D, D, D, D, D, D, D, W},
/* 02 */{W, D, W, W, W, W, D, W, W, W, W, W, D, W, W, D, W, W, W, W, W, D, W, W, W, W, D, W},
/* 03 */{W, P, W, E, E, W, D, W, E, E, E, W, D, W, W, D, W, E, E, E, W, D, W, E, E, W, P, W},
/* 04 */{W, D, W, W, W, W, D, W, W, W, W, W, D, W, W, D, W, W, W, W, W, D, W, W, W, W, D, W},
/* 05 */{W, T, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, T, W},
/* 06 */{W, D, W, W, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, W, W, D, W},
/* 07 */{W, D, W, W, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, W, W, D, W},
/* 08 */{W, D, D, S, D, D, D, W, W, D, D, D, D, W, W, D, D, D, D, W, W, D, D, D, S, D, D, W},
/* 09 */{W, W, W, W, W, W, D, W, W, W, W, W, E, W, W, E, W, W, W, W, W, D, W, W, W, W, W, W},
/* 10 */{E, E, E, E, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, E, E, E, E},
/* 11 */{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/* 12 */{E, E, E, E, W, W, D, E, E, E, W, E, E, E, E, E, E, W, E, E, E, D, W, W, E, E, E, E},
/* 13 */{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/* 14 */{W, W, W, W, W, W, D, W, W, E, E, E, E, E, E, E, E, E, E, W, W, D, W, W, W, W, W, W},
/* 15 */{E, E, E, E, W, W, D, W, W, E, W, W, W, W, W, W, W, W, E, W, W, D, W, W, E, E, E, E},
/* 16 */{E, E, E, E, W, W, D, W, W, E, E, E, E, W, W, E, E, E, E, W, W, D, W, W, E, E, E, E},
/* 17 */{W, D, D, D, D, D, D, D, D, D, D, D, B, W, W, D, D, D, D, D, D, D, D, D, D, D, D, W},
/* 18 */{W, D, W, W, W, W, D, W, W, W, W, W, D, W, W, D, W, W, W, W, W, D, W, W, W, W, D, W},
/* 19 */{W, D, W, W, W, W, D, W, W, W, W, W, D, W, W, D, W, W, W, W, W, D, W, W, W, W, D, W},
/* 20 */{W, P, D, D, W, W, D, D, D, D, D, D, D, E, E, D, D, D, D, D, D, D, W, W, D, D, P, W},
/* 21 */{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/* 22 */{W, W, W, D, W, W, D, W, W, D, W, W, W, W, W, W, W, W, D, W, W, D, W, W, D, W, W, W},
/* 23 */{W, D, D, S, D, D, D, W, W, D, D, D, D, W, W, D, D, D, D, W, W, D, D, D, S, D, D, W},
/* 24 */{W, D, W, W, W, W, W, W, W, W, W, W, D, W, W, D, W, W, W, W, W, W, W, W, W, W, D, W},
/* 25 */{W, D, W, W, W, W, W, W, W, W, W, W, D, W, W, D, W, W, W, W, W, W, W, W, W, W, D, W},
/* 26 */{W, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, W},
/* 27 */{W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
};

#undef W
#undef D
#undef P
#undef E
#undef T
#undef S
#undef B

/* Estado mutable: copia de MAP_DATA en RAM; TILE_EMPTY cuando un punto es comido */
unsigned char map_state[MAP_ROWS][MAP_COLS];
unsigned char dots_remaining;

void map_init(void) {
    unsigned char r, c;
    dots_remaining = 0;
    for (r = 0; r < MAP_ROWS; r++) {
        for (c = 0; c < MAP_COLS; c++) {
            map_state[r][c] = MAP_DATA[r][c];
            if (MAP_DATA[r][c] == TILE_DOT || MAP_DATA[r][c] == TILE_PELLET)
                dots_remaining++;
        }
    }
}

unsigned char map_eat_dot(unsigned char tx, unsigned char ty) {
    unsigned char tile = map_state[ty][tx];
    if (tile == TILE_DOT || tile == TILE_PELLET) {
        map_state[ty][tx] = TILE_EMPTY;
        if (dots_remaining > 0) dots_remaining--;
    } else if (tile == TILE_BOMB) {
        /* La bomba se recoge: desaparece del mapa (obstacles.c gestiona el respawn) */
        map_state[ty][tx] = TILE_EMPTY;
    }
    return tile;
}

/* Vuelca el laberinto al nametable usando las funciones de neslib.
   Llamar con renderizado deshabilitado (ppu_off ya llamado). */
void map_render(void) {
    uint8_t row;
    unsigned int addr;
    for (row = 0; row < MAP_ROWS; row++) {
        addr = NTADR_A(MAP_X_OFFSET, MAP_Y_OFFSET + row);
        vram_adr(addr);
        vram_write((unsigned char *)MAP_DATA[row], MAP_COLS);
    }
}
