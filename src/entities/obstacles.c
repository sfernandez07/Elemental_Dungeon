#include "neslib.h"
#include "map.h"
#include "score.h"
#include "player.h"
#include "ghost.h"
#include "obstacles.h"

/* ------------------------------------------------------------------ */
/* Estado interno                                                      */
/* ------------------------------------------------------------------ */

static unsigned char tempwall_timer;    /* cuenta frames hasta el próximo toggle */
static unsigned char tempwall_active;   /* 0=abiertos, 1=bloqueados */

static unsigned char bomb_respawn;      /* frames hasta que reaparece la bomba (0=ya visible) */

/* Posiciones de las paredes temporales (en tiles) */
static const unsigned char TW_TX[TEMPWALL_COUNT] = { TEMPWALL_TX0, TEMPWALL_TX1 };
static const unsigned char TW_TY[TEMPWALL_COUNT] = { TEMPWALL_TY0, TEMPWALL_TY1 };

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

/* Escribe una entrada de actualización VRAM de byte único en buf. */
static void vram_buf_put(unsigned char *buf, unsigned char *plen,
                          unsigned int addr, unsigned char tile)
{
    unsigned char n = *plen;
    buf[n++] = (unsigned char)(addr >> 8);
    buf[n++] = (unsigned char)(addr & 0xFF);
    buf[n++] = tile;
    *plen = n;
}

/* ------------------------------------------------------------------ */
/* Inicialización                                                      */
/* ------------------------------------------------------------------ */

void obstacles_init(void)
{
    tempwall_timer  = 0;
    tempwall_active = 0;   /* paredes empiezan abiertas */
    bomb_respawn    = 0;   /* bomba visible desde el inicio */
}

/* ------------------------------------------------------------------ */
/* Actualización por frame                                             */
/* ------------------------------------------------------------------ */

void obstacles_update(Player *p, unsigned char *buf, unsigned char *plen)
{
    unsigned char i;
    unsigned int  addr;

    /* ============================================================ */
    /* 1. Paredes temporales                                        */
    /* ============================================================ */
    tempwall_timer++;
    if (tempwall_timer >= TEMPWALL_PERIOD) {
        tempwall_timer  = 0;
        tempwall_active ^= 1;

        for (i = 0; i < TEMPWALL_COUNT; i++) {
            unsigned char tx = TW_TX[i];
            unsigned char ty = TW_TY[i];

            if (tempwall_active) {
                /* Activar: bloquear paso y mostrar tile de pared temporal */
                map_state[ty][tx] = TILE_WALL;
                addr = NTADR_A(MAP_X_OFFSET + tx, MAP_Y_OFFSET + ty);
                vram_buf_put(buf, plen, addr, TILE_TEMP_WALL);
            } else {
                /* Desactivar: abrir paso y vaciar tile */
                map_state[ty][tx] = TILE_EMPTY;
                addr = NTADR_A(MAP_X_OFFSET + tx, MAP_Y_OFFSET + ty);
                vram_buf_put(buf, plen, addr, TILE_EMPTY);
            }
        }
    }

    /* ============================================================ */
    /* 2. Bomba                                                     */
    /* ============================================================ */
    if (bomb_eaten) {
        /* Pac-Man acaba de recoger la bomba (señal de player_update) */
        bomb_eaten   = 0;
        bomb_respawn = BOMB_RESPAWN_TIME;

        /* Destruir todos los fantasmas visibles */
        for (i = 0; i < GHOST_COUNT; i++) {
            if (ghosts[i].state != GHOST_DEAD) {
                ghosts[i].state      = GHOST_DEAD;
                ghosts[i].dead_timer = BOMB_EFFECT_TIME;
            }
        }
        /* Sumar puntos por usar la bomba */
        score_add(100);
    }

    if (bomb_respawn > 0) {
        bomb_respawn--;
        if (bomb_respawn == 0) {
            /* Reaparece la bomba */
            map_state[BOMB_TY][BOMB_TX] = TILE_BOMB;
            addr = NTADR_A(MAP_X_OFFSET + BOMB_TX, MAP_Y_OFFSET + BOMB_TY);
            vram_buf_put(buf, plen, addr, TILE_BOMB);
        }
    }

    /* Suprimir advertencia de parámetro no usado */
    (void)p;
}
