#ifndef GHOST_H
#define GHOST_H

#include "player.h"

/* Estados del fantasma */
#define GHOST_NORMAL    0   /* persigue / patrulla, mata a Pac-Man */
#define GHOST_SCARED    1   /* asustado, Pac-Man puede comerlo */
#define GHOST_DEAD      2   /* muerto, respawn tras dead_timer frames */

/* Tipos de comportamiento */
#define GHOST_CHASER    0   /* greedy: reduce distancia Manhattan a Pac-Man */
#define GHOST_PATROLLER 1   /* right-hand rule: sigue la pared derecha */

#define GHOST_COUNT          4
#define GHOST_DEAD_DURATION  180   /* ~3 s a 60 Hz */

/* Puntos al comer un fantasma asustado */
#define GHOST_EAT_PTS  200

typedef struct {
    unsigned char px, py;       /* posición en pantalla (píxeles) */
    unsigned char dir;          /* dirección actual (DIR_*) */
    unsigned char state;        /* GHOST_NORMAL / SCARED / DEAD */
    unsigned char type;         /* GHOST_CHASER / GHOST_PATROLLER */
    unsigned char move_cnt;     /* píxeles restantes hasta el tile siguiente */
    unsigned char dead_timer;   /* frames hasta el respawn */
    unsigned char start_tx;     /* tile de reaparición X */
    unsigned char start_ty;     /* tile de reaparición Y */
    unsigned char start_dir;    /* dirección inicial / de reaparición */
    unsigned char palette;      /* atributo OAM: bits 0-1 = índice de paleta sprite */
} Ghost;

/* Array global de todos los fantasmas */
extern Ghost ghosts[GHOST_COUNT];

/* Flag: 1 si un fantasma normal tocó a Pac-Man este frame */
extern unsigned char pacman_died;

void          ghost_init_all(void);
void          ghost_update_all(const Player *p);
unsigned char ghost_draw_all(const Player *p, unsigned char sprid);

/* Cambia todos los fantasmas NORMAL a SCARED y les invierte la dirección.
   Llamar desde main.c cuando Pac-Man come un power pellet. */
void ghost_scare_all(void);

#endif
