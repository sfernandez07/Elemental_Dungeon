#include "neslib.h"
#include "map.h"
#include "score.h"
#include "player.h"
#include "ghost.h"
#include "obstacles.h"
#include "sound.h"

/* ------------------------------------------------------------------ */
/* Estado interno                                                      */
/* ------------------------------------------------------------------ */

static unsigned char tempwall_timer;
static unsigned char tempwall_active;

static unsigned char bomb_respawn;

/* ---- Volcanes (MAP_3) ---- */

/* Posiciones ROM */
static const unsigned char VOL_TX[VOLCANO_COUNT]  = {  5, 22,  0, 27 };
static const unsigned char VOL_TY[VOLCANO_COUNT]  = {  0,  0,  8, 20 };
/* DIR_DOWN=3, DIR_RIGHT=0 corrected: RIGHT=0,LEFT=1,UP=2,DOWN=3 */
static const unsigned char VOL_DIR[VOLCANO_COUNT] = {  3,  3,  0,  1 };

static unsigned int  vol_dormant[VOLCANO_COUNT];   /* 16 bits: 300–599 frames no caben en 8 */
static unsigned char lava_slot_vol[LAVA_SLOT_COUNT]; /* 0xFF = vacío */
static unsigned char lava_timer[LAVA_SLOT_COUNT];
/* Backup de tiles: máx 28 tiles por fila/columna */
static unsigned char lava_backup[LAVA_SLOT_COUNT][28];
static unsigned char lava_active_count;

/* ---- Rayo (MAP_4) ---- */
static unsigned int  lightning_spawn_timer;
static unsigned char lightning_active;
static unsigned char lightning_tx;
static unsigned char lightning_ty;
static unsigned char lightning_duration;
static unsigned char lightning_anim_cnt;
static unsigned char lightning_anim_frm;
static unsigned char lightning_under;   /* tile que tapa el rayo (DOT o EMPTY), se restaura al apagarse */

/* Posiciones de las paredes temporales */
static const unsigned char TW_TX[TEMPWALL_COUNT] = { TEMPWALL_TX0, TEMPWALL_TX1 };
static const unsigned char TW_TY[TEMPWALL_COUNT] = { TEMPWALL_TY0, TEMPWALL_TY1 };

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

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
    unsigned char i;

    tempwall_timer  = 0;
    tempwall_active = 0;
    bomb_respawn    = 0;

    /* Volcanes */
    lava_active_count = 0;
    for (i = 0; i < VOLCANO_COUNT; i++)
        vol_dormant[i] = VOLCANO_DORMANT_MIN + (rand16() % VOLCANO_DORMANT_RNG);
    for (i = 0; i < LAVA_SLOT_COUNT; i++) {
        lava_slot_vol[i] = 0xFF;
        lava_timer[i]    = 0;
    }

    /* Rayo */
    lightning_active       = 0;
    lightning_spawn_timer  = (unsigned int)(LIGHTNING_SPAWN_MIN + (rand8() % LIGHTNING_SPAWN_RNG));
    lightning_duration     = 0;
    lightning_anim_cnt     = 0;
    lightning_anim_frm     = 0;
}

/* ------------------------------------------------------------------ */
/* Lógica volcanes                                                     */
/* ------------------------------------------------------------------ */

static void volcano_update(unsigned char *buf, unsigned char *plen)
{
    unsigned char i, s, j;
    unsigned char tx, ty, t;
    unsigned int addr;

    /* Volcanes inactivos: decrementar dormancy */
    for (i = 0; i < VOLCANO_COUNT; i++) {
        /* Comprobar si ya está erucionando */
        unsigned char erupting = 0;
        for (s = 0; s < LAVA_SLOT_COUNT; s++) {
            if (lava_slot_vol[s] == i) { erupting = 1; break; }
        }
        if (erupting) continue;

        if (vol_dormant[i] > 0) {
            vol_dormant[i]--;
            continue;
        }
        /* Erupcionar si hay slot libre */
        if (lava_active_count >= LAVA_SLOT_COUNT) {
            /* Reiniciar temporizador y esperar */
            vol_dormant[i] = VOLCANO_DORMANT_MIN + (rand16() % VOLCANO_DORMANT_RNG);
            continue;
        }
        /* Buscar slot vacío */
        for (s = 0; s < LAVA_SLOT_COUNT; s++) {
            if (lava_slot_vol[s] == 0xFF) break;
        }
        lava_slot_vol[s] = i;
        lava_timer[s]    = LAVA_DURATION;
        lava_active_count++;

        /* Rellenar fila o columna con lava */
        if (VOL_DIR[i] == 3 || VOL_DIR[i] == 2) {
            /* DIR_DOWN / DIR_UP → erupción por columna */
            tx = VOL_TX[i];
            for (j = 0; j < MAP_ROWS; j++) {
                t = map_state[j][tx];
                lava_backup[s][j] = t;
                if (t != TILE_WALL && t != TILE_VOLCANO) {
                    map_state[j][tx] = TILE_LAVA;
                    addr = NTADR_A(MAP_X_OFFSET + tx, MAP_Y_OFFSET + j);
                    vram_buf_put(buf, plen, addr, TILE_LAVA);
                }
            }
        } else {
            /* DIR_RIGHT / DIR_LEFT → erupción por fila */
            ty = VOL_TY[i];
            for (j = 0; j < MAP_COLS; j++) {
                t = map_state[ty][j];
                lava_backup[s][j] = t;
                if (t != TILE_WALL && t != TILE_VOLCANO) {
                    map_state[ty][j] = TILE_LAVA;
                    addr = NTADR_A(MAP_X_OFFSET + j, MAP_Y_OFFSET + ty);
                    vram_buf_put(buf, plen, addr, TILE_LAVA);
                }
            }
        }
        vol_dormant[i] = VOLCANO_DORMANT_MIN + (rand16() % VOLCANO_DORMANT_RNG);
    }

    /* Slots activos: decrementar timer y restaurar cuando expiren */
    for (s = 0; s < LAVA_SLOT_COUNT; s++) {
        if (lava_slot_vol[s] == 0xFF) continue;
        if (lava_timer[s] > 0) {
            lava_timer[s]--;
            continue;
        }
        /* Restaurar */
        i = lava_slot_vol[s];
        if (VOL_DIR[i] == 3 || VOL_DIR[i] == 2) {
            tx = VOL_TX[i];
            for (j = 0; j < MAP_ROWS; j++) {
                if (map_state[j][tx] == TILE_LAVA) {
                    map_state[j][tx] = lava_backup[s][j];
                    addr = NTADR_A(MAP_X_OFFSET + tx, MAP_Y_OFFSET + j);
                    vram_buf_put(buf, plen, addr, lava_backup[s][j]);
                }
            }
        } else {
            ty = VOL_TY[i];
            for (j = 0; j < MAP_COLS; j++) {
                if (map_state[ty][j] == TILE_LAVA) {
                    map_state[ty][j] = lava_backup[s][j];
                    addr = NTADR_A(MAP_X_OFFSET + j, MAP_Y_OFFSET + ty);
                    vram_buf_put(buf, plen, addr, lava_backup[s][j]);
                }
            }
        }
        lava_slot_vol[s] = 0xFF;
        if (lava_active_count > 0) lava_active_count--;
    }
}

/* ------------------------------------------------------------------ */
/* Lógica rayo                                                         */
/* ------------------------------------------------------------------ */

static void lightning_update(unsigned char *buf, unsigned char *plen)
{
    unsigned char ltx, lty;
    unsigned int  addr;

    if (lightning_active) {
        /* Parpadeo: alterna entre frame A (brillante) y B (tenue) cada 8 frames */
        lightning_anim_cnt++;
        if (lightning_anim_cnt >= 8) {
            lightning_anim_cnt = 0;
            lightning_anim_frm ^= 1;
            addr = NTADR_A(MAP_X_OFFSET + lightning_tx, MAP_Y_OFFSET + lightning_ty);
            vram_buf_put(buf, plen, addr,
                         lightning_anim_frm ? TILE_LIGHTNING_B : TILE_LIGHTNING);
        }
        if (lightning_duration > 0) {
            lightning_duration--;
        } else {
            /* Borrar rayo restaurando lo que tapaba: si era un punto y se
               borrase, dots_remaining nunca llegaría a 0 y la puerta no se abriría. */
            map_state[lightning_ty][lightning_tx] = lightning_under;
            addr = NTADR_A(MAP_X_OFFSET + lightning_tx, MAP_Y_OFFSET + lightning_ty);
            vram_buf_put(buf, plen, addr, lightning_under);
            lightning_active = 0;
            lightning_spawn_timer = (unsigned int)(LIGHTNING_SPAWN_MIN +
                                    (rand8() % LIGHTNING_SPAWN_RNG));
        }
    } else {
        if (lightning_spawn_timer > 0) {
            lightning_spawn_timer--;
        } else {
            /* Buscar tile válido (DOT o EMPTY) en zona de juego */
            ltx = (unsigned char)((rand8() % 26) + 1);
            lty = (unsigned char)((rand8() % 26) + 1);
            if (map_state[lty][ltx] == TILE_DOT ||
                map_state[lty][ltx] == TILE_EMPTY) {
                lightning_under     = map_state[lty][ltx];
                map_state[lty][ltx] = TILE_LIGHTNING;
                addr = NTADR_A(MAP_X_OFFSET + ltx, MAP_Y_OFFSET + lty);
                vram_buf_put(buf, plen, addr, TILE_LIGHTNING);
                lightning_tx       = ltx;
                lightning_ty       = lty;
                lightning_active   = 1;
                lightning_duration = LIGHTNING_DURATION;
            } else {
                /* Tile ocupado, reintentar el siguiente frame */
                lightning_spawn_timer = 1;
            }
        }
    }
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
                map_state[ty][tx] = TILE_WALL;
                addr = NTADR_A(MAP_X_OFFSET + tx, MAP_Y_OFFSET + ty);
                vram_buf_put(buf, plen, addr, TILE_TEMP_WALL);
            } else {
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
        bomb_eaten   = 0;
        sound_play(SFX_BOMB);
        bomb_respawn = BOMB_RESPAWN_TIME;

        for (i = 0; i < GHOST_COUNT; i++) {
            if (ghosts[i].state != GHOST_DEAD) {
                ghosts[i].state      = GHOST_DEAD;
                ghosts[i].dead_timer = BOMB_EFFECT_TIME;
            }
        }
        score_add(100);
    }

    if (bomb_respawn > 0) {
        bomb_respawn--;
        if (bomb_respawn == 0) {
            map_state[BOMB_TY][BOMB_TX] = TILE_BOMB;
            addr = NTADR_A(MAP_X_OFFSET + BOMB_TX, MAP_Y_OFFSET + BOMB_TY);
            vram_buf_put(buf, plen, addr, TILE_BOMB);
        }
    }

    /* ============================================================ */
    /* 3. Volcanes — solo en MAP_3 (nivel 3)                       */
    /* ============================================================ */
    if (current_level == 3)
        volcano_update(buf, plen);

    /* ============================================================ */
    /* 4. Rayo — solo en MAP_4 (nivel 4)                           */
    /* ============================================================ */
    if (current_level == 4)
        lightning_update(buf, plen);

    (void)p;
}
