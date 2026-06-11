#include "neslib.h"
#include "map.h"
#include "score.h"
#include "player.h"
#include "ghost.h"
#include "sound.h"

/* ------------------------------------------------------------------ */
/* Tablas de dirección                                                 */
/* ------------------------------------------------------------------ */

/* Deltas de tile: RIGHT=0, LEFT=1, UP=2, DOWN=3 */
static const signed char GDX[4] = {  1, -1,  0,  0 };
static const signed char GDY[4] = {  0,  0, -1,  1 };

/* Dirección opuesta */
static const unsigned char DIR_OPP[5] = {
    DIR_LEFT, DIR_RIGHT, DIR_DOWN, DIR_UP, DIR_NONE
};

/* "Giro a la derecha" para la regla de la pared derecha (right-hand rule):
   RIGHT→DOWN, LEFT→UP, UP→RIGHT, DOWN→LEFT */
static const unsigned char TURN_R[4] = { DIR_DOWN, DIR_UP, DIR_RIGHT, DIR_LEFT };

/* "Giro a la izquierda" (opuesto del giro derecho):
   RIGHT→UP, LEFT→DOWN, UP→LEFT, DOWN→RIGHT */
static const unsigned char TURN_L[4] = { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

/* ------------------------------------------------------------------ */
/* Configuración inicial (en ROM)                                      */
/* ------------------------------------------------------------------ */

typedef struct { unsigned char tx, ty, dir, type, palette; } GhostCfg;

static const GhostCfg CFG[GHOST_COUNT] = {
    {  9, 10, DIR_RIGHT, GHOST_CHASER,    0x01 },   /* rojo,  perseguidor */
    { 18, 10, DIR_LEFT,  GHOST_PATROLLER, 0x02 },   /* cian,  patrullero  */
    {  9, 16, DIR_UP,    GHOST_PATROLLER, 0x01 },   /* rojo,  patrullero  */
    { 18, 16, DIR_UP,    GHOST_PATROLLER, 0x02 },   /* cian,  patrullero  */
};

/* ------------------------------------------------------------------ */
/* Variables globales                                                  */
/* ------------------------------------------------------------------ */

Ghost        ghosts[GHOST_COUNT];
unsigned char player_died;
unsigned char ghost_rage;

static unsigned char ghost_anim_cnt;
static unsigned char ghost_anim_frm;

/* ------------------------------------------------------------------ */
/* Helpers internos                                                    */
/* ------------------------------------------------------------------ */

/* ¿Puede el fantasma entrar en el tile vecino en la dirección d?
   Usa map_state (no MAP_DATA) para que las paredes temporales también bloqueen. */
static unsigned char ghost_can_move(unsigned char tx, unsigned char ty,
                                     unsigned char d)
{
    unsigned char t;
    signed char ntx = (signed char)tx + GDX[d];
    signed char nty = (signed char)ty + GDY[d];
    if (ntx < 0 || ntx >= MAP_COLS || nty < 0 || nty >= MAP_ROWS) return 0;
    t = map_state[(unsigned char)nty][(unsigned char)ntx];
    return (t != TILE_WALL && t != TILE_DOOR_OPEN &&
            t != TILE_LAVA && t != TILE_VOLCANO);
}

/* Distancia Manhattan entre dos tiles */
static unsigned char manhatt(unsigned char ax, unsigned char ay,
                               unsigned char bx, unsigned char by)
{
    unsigned char dx = (ax > bx) ? ax - bx : bx - ax;
    unsigned char dy = (ay > by) ? ay - by : by - ay;
    return dx + dy;
}

/* IA perseguidora: elige la dirección que minimiza (flee=0) o maximiza
   (flee=1) la distancia Manhattan a (ptx, pty). No puede invertir marcha. */
static unsigned char chaser_dir(unsigned char tx, unsigned char ty,
                                 unsigned char cur_dir,
                                 unsigned char ptx, unsigned char pty,
                                 unsigned char flee)
{
    unsigned char d, ntx, nty, dist;
    unsigned char rev      = DIR_OPP[cur_dir];
    unsigned char best_dir = DIR_NONE;
    unsigned char best     = flee ? 0 : 255;

    for (d = 0; d < 4; d++) {
        if (d == rev) continue;
        if (!ghost_can_move(tx, ty, d)) continue;

        ntx  = (unsigned char)((signed char)tx + GDX[d]);
        nty  = (unsigned char)((signed char)ty + GDY[d]);
        dist = manhatt(ntx, nty, ptx, pty);

        if (flee ? (dist > best) : (dist < best)) {
            best     = dist;
            best_dir = d;
        }
    }

    /* Fallback: si todas las salidas están bloqueadas, invertir */
    if (best_dir == DIR_NONE) best_dir = rev;
    return best_dir;
}

/* IA patrullera: right-hand rule (prioridad: gira derecha, recto, gira
   izquierda, invierte). Produce circuitos consistentes alrededor de paredes. */
static unsigned char patroller_dir(unsigned char tx, unsigned char ty,
                                    unsigned char cur_dir)
{
    unsigned char d;

    d = TURN_R[cur_dir]; if (ghost_can_move(tx, ty, d)) return d;
    d = cur_dir;          if (ghost_can_move(tx, ty, d)) return d;
    d = TURN_L[cur_dir]; if (ghost_can_move(tx, ty, d)) return d;
    return DIR_OPP[cur_dir];   /* último recurso: invertir */
}

/* TX_TO_PX, TY_TO_PY, PX_TO_TX, PY_TO_TY definidos en map.h */

/* ------------------------------------------------------------------ */
/* Inicialización                                                      */
/* ------------------------------------------------------------------ */

static void ghost_reset(Ghost *g, unsigned char idx)
{
    g->px        = TX_TO_PX(CFG[idx].tx);
    g->py        = TY_TO_PY(CFG[idx].ty);
    g->dir       = CFG[idx].dir;
    g->state     = GHOST_NORMAL;
    g->type      = CFG[idx].type;
    g->move_cnt  = 0;
    g->dead_timer= 0;
    g->start_tx  = CFG[idx].tx;
    g->start_ty  = CFG[idx].ty;
    g->start_dir = CFG[idx].dir;
    g->palette   = CFG[idx].palette;
}

void ghost_init_all(void)
{
    unsigned char i;
    player_died    = 0;
    ghost_rage     = 0;
    ghost_anim_cnt = 0;
    ghost_anim_frm = 0;
    for (i = 0; i < GHOST_COUNT; i++) ghost_reset(&ghosts[i], i);
}

/* ------------------------------------------------------------------ */
/* Actualización por frame                                             */
/* ------------------------------------------------------------------ */

static void ghost_update_one(Ghost *g, unsigned char idx,
                              const Player *p)
{
    unsigned char tx, ty, ptx, pty;
    unsigned char dx, dy;

    /* --- Estado DEAD: contar y hacer respawn --- */
    if (g->state == GHOST_DEAD) {
        if (g->dead_timer > 0) { g->dead_timer--; return; }
        ghost_reset(g, idx);
        return;
    }

    /* --- Fin de modo asustado --- */
    if (g->state == GHOST_SCARED && p->power_timer == 0)
        g->state = GHOST_NORMAL;

    /* --- Movimiento pixel a pixel --- */
    if (g->move_cnt > 0) {
        g->px = (unsigned char)(g->px + GDX[g->dir]);
        g->py = (unsigned char)(g->py + GDY[g->dir]);
        g->move_cnt--;
    } else {
        /* Alineado en tile: elegir dirección */
        tx  = PX_TO_TX(g->px);
        ty  = PY_TO_TY(g->py);
        ptx = PX_TO_TX(p->px);
        pty = PY_TO_TY(p->py);

        if (g->type == GHOST_CHASER)
            g->dir = chaser_dir(tx, ty, g->dir, ptx, pty,
                                 g->state == GHOST_SCARED);
        else
            g->dir = patroller_dir(tx, ty, g->dir);

        g->px = (unsigned char)(g->px + GDX[g->dir]);
        g->py = (unsigned char)(g->py + GDY[g->dir]);
        g->move_cnt = ghost_rage ? 5 : 10;
    }

    /* --- Colisión con el héroe (distancia < 6 px en ambos ejes) --- */
    dx = (g->px > p->px) ? g->px - p->px : p->px - g->px;
    dy = (g->py > p->py) ? g->py - p->py : p->py - g->py;
    if (dx < 6 && dy < 6) {
        if (g->state == GHOST_SCARED) {
            score_add(GHOST_EAT_PTS);
            sound_play(SFX_GHOST_EAT);
            g->state      = GHOST_DEAD;
            g->dead_timer = GHOST_DEAD_DURATION;
        } else {
            player_died = 1;
        }
    }
}

void ghost_update_all(const Player *p)
{
    unsigned char i;
    player_died = 0;
    for (i = 0; i < GHOST_COUNT; i++)
        ghost_update_one(&ghosts[i], i, p);
}

/* ------------------------------------------------------------------ */
/* Modo rabia — activa velocidad doble en todos los enemigos           */
/* ------------------------------------------------------------------ */

void ghost_rage_all(void)
{
    ghost_rage = 1;
}

/* ------------------------------------------------------------------ */
/* Dibujo en OAM                                                       */
/* ------------------------------------------------------------------ */

/* Sprite tiles (tabla 1):
   $06/$07 = esqueleto normal frame A/B  (color 1, paleta del fantasma)
   $08/$09 = esqueleto asustado frame A/B (color 1, paleta 3)
   Frame B de asustado se usa como parpadeo de advertencia (<60 frames restantes) */
unsigned char ghost_draw_all(const Player *p, unsigned char sprid)
{
    unsigned char i, tile, attr;
    unsigned char blink;

    /* Avanzar animación de caminar (8 frames por frame) */
    ghost_anim_cnt++;
    if (ghost_anim_cnt >= 8) {
        ghost_anim_cnt = 0;
        ghost_anim_frm ^= 1;
    }

    /* Parpadeo de advertencia cuando queda < 1 segundo de power */
    blink = (p->power_timer > 0 && p->power_timer < 60 &&
             (p->power_timer & 0x08));

    for (i = 0; i < GHOST_COUNT; i++) {
        if (ghosts[i].state == GHOST_DEAD) continue;

        if (ghosts[i].state == GHOST_SCARED) {
            /* frame B = ojos asomándose (advertencia); frame A = encogido */
            tile = blink ? 0x09 : 0x08;
            attr = 0x03;
        } else {
            tile = ghost_anim_frm ? 0x07 : 0x06;
            attr = ghosts[i].palette;
        }

        sprid = oam_spr(ghosts[i].px, ghosts[i].py, tile, attr, sprid);
    }
    return sprid;
}

/* ------------------------------------------------------------------ */
/* Activar modo asustado                                               */
/* ------------------------------------------------------------------ */

void ghost_scare_all(void)
{
    unsigned char i;
    for (i = 0; i < GHOST_COUNT; i++) {
        if (ghosts[i].state == GHOST_NORMAL) {
            ghosts[i].state = GHOST_SCARED;
            /* Invertir dirección de marcha */
            ghosts[i].dir   = DIR_OPP[ghosts[i].dir];
        }
    }
}
