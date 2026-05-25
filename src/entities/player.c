#include "neslib.h"
#include "map.h"
#include "score.h"
#include "player.h"

/* Deltas de tile: RIGHT=0, LEFT=1, UP=2, DOWN=3 */
static const signed char DX[4] = {  1, -1,  0,  0 };
static const signed char DY[4] = {  0,  0, -1,  1 };

/* Tiles CHR de sprites por dirección */
static const unsigned char OPEN_TILE[4] = { 0x00, 0x02, 0x03, 0x04 };
#define CLOSED_TILE 0x01

/* Señales para main() y obstacles.c */
unsigned char dot_eaten  = 0;
unsigned int  dot_addr   = 0;
unsigned char bomb_eaten = 0;

/* ------------------------------------------------------------------ */

static unsigned char can_move(unsigned char tx, unsigned char ty,
                               unsigned char dir)
{
    signed char ntx = (signed char)tx + DX[dir];
    signed char nty = (signed char)ty + DY[dir];
    if (ntx < 0 || ntx >= MAP_COLS || nty < 0 || nty >= MAP_ROWS) return 0;
    return map_state[(unsigned char)nty][(unsigned char)ntx] != TILE_WALL;
}

/* ------------------------------------------------------------------ */

void player_init(Player *p)
{
    p->px          = TX_TO_PX(PLAYER_START_TX);
    p->py          = TY_TO_PY(PLAYER_START_TY);
    p->dir         = DIR_LEFT;
    p->next_dir    = DIR_LEFT;
    p->move_cnt    = 0;
    p->anim_frm    = 0;
    p->anim_cnt    = 0;
    p->power_timer = 0;
    p->slow_cnt    = 0;
    p->tele_lock   = 0;
}

/* ------------------------------------------------------------------ */

void player_update(Player *p)
{
    unsigned char buttons;
    unsigned char tx, ty;
    unsigned char eaten;

    /* --- Trampa de velocidad: omitir un frame de cada dos --- */
    tx = PX_TO_TX(p->px);
    ty = PY_TO_TY(p->py);
    if (map_state[ty][tx] == TILE_SLOW_TRAP) {
        p->slow_cnt ^= 1;
        if (p->slow_cnt) {
            /* Aun así leer el pad para no perder inputs */
            buttons = pad_poll(0);
            if      (buttons & PAD_RIGHT) p->next_dir = DIR_RIGHT;
            else if (buttons & PAD_LEFT)  p->next_dir = DIR_LEFT;
            else if (buttons & PAD_UP)    p->next_dir = DIR_UP;
            else if (buttons & PAD_DOWN)  p->next_dir = DIR_DOWN;
            return;
        }
    } else {
        p->slow_cnt = 0;   /* resetear al salir de la trampa */
    }

    /* --- Leer entrada --- */
    buttons = pad_poll(0);
    if      (buttons & PAD_RIGHT) p->next_dir = DIR_RIGHT;
    else if (buttons & PAD_LEFT)  p->next_dir = DIR_LEFT;
    else if (buttons & PAD_UP)    p->next_dir = DIR_UP;
    else if (buttons & PAD_DOWN)  p->next_dir = DIR_DOWN;

    /* --- Movimiento pixel a pixel --- */
    dot_eaten  = 0;
    bomb_eaten = 0;

    if (p->move_cnt > 0) {
        p->px = (unsigned char)(p->px + DX[p->dir]);
        p->py = (unsigned char)(p->py + DY[p->dir]);
        p->move_cnt--;
    } else {
        /* Alineado en tile */
        tx = PX_TO_TX(p->px);
        ty = PY_TO_TY(p->py);

        /* Teletransportador */
        if (p->tele_lock == 0 && map_state[ty][tx] == TILE_TELEPORT) {
            if (tx == TELE_A_TX && ty == TELE_A_TY) {
                p->px = TX_TO_PX(TELE_B_TX);
                p->py = TY_TO_PY(TELE_B_TY);
            } else {
                p->px = TX_TO_PX(TELE_A_TX);
                p->py = TY_TO_PY(TELE_A_TY);
            }
            p->tele_lock = 16;
            p->move_cnt  = 0;
            /* Recalcular tile tras el teletransporte */
            tx = PX_TO_TX(p->px);
            ty = PY_TO_TY(p->py);
        }

        /* Intentar cambio de dirección solicitado */
        if (p->next_dir != DIR_NONE && can_move(tx, ty, p->next_dir))
            p->dir = p->next_dir;

        /* Avanzar si la dirección está libre */
        if (p->dir != DIR_NONE && can_move(tx, ty, p->dir)) {
            p->px = (unsigned char)(p->px + DX[p->dir]);
            p->py = (unsigned char)(p->py + DY[p->dir]);
            p->move_cnt = 7;
        }

        /* Colisión con puntos, pellets y bomba */
        eaten = map_eat_dot(tx, ty);
        if (eaten == TILE_DOT) {
            score_add(DOT_PTS);
            dot_eaten = 1;
            dot_addr  = NTADR_A(MAP_X_OFFSET + tx, MAP_Y_OFFSET + ty);
        } else if (eaten == TILE_PELLET) {
            score_add(PELLET_PTS);
            p->power_timer = POWER_DURATION;
            dot_eaten = 1;
            dot_addr  = NTADR_A(MAP_X_OFFSET + tx, MAP_Y_OFFSET + ty);
        } else if (eaten == TILE_BOMB) {
            bomb_eaten = 1;
            dot_eaten  = 1;
            dot_addr   = NTADR_A(MAP_X_OFFSET + tx, MAP_Y_OFFSET + ty);
        }
    }

    /* Decrementar power timer y tele_lock */
    if (p->power_timer > 0) p->power_timer--;
    if (p->tele_lock   > 0) p->tele_lock--;

    /* Animación: alternar cada 8 frames */
    p->anim_cnt++;
    if (p->anim_cnt >= 8) {
        p->anim_cnt = 0;
        p->anim_frm ^= 1;
    }
}

/* ------------------------------------------------------------------ */

void player_draw(const Player *p)
{
    unsigned char tile;
    unsigned char dir_idx = (p->dir == DIR_NONE) ? DIR_RIGHT : p->dir;
    tile = (p->anim_frm == 0) ? OPEN_TILE[dir_idx] : CLOSED_TILE;
    oam_spr(p->px, p->py, tile, 0x00, 0);
}
