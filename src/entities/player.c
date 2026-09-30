#include "neslib.h"
#include "map.h"
#include "score.h"
#include "player.h"
#include "ghost.h"

/* Deltas de tile: RIGHT=0, LEFT=1, UP=2, DOWN=3 */
static const signed char DX[4] = {  1, -1,  0,  0 };
static const signed char DY[4] = {  0,  0, -1,  1 };

/* Mapa DIR_* → bit de pad (para el truco de prioridad de Chase) */
static const unsigned char DIR_TO_PAD[4] = {
    PAD_RIGHT, PAD_LEFT, PAD_UP, PAD_DOWN
};

/* Sprites por dirección (RIGHT=0, LEFT=1, UP=2, DOWN=3).
   LEFT usa el mismo tile que RIGHT con flip horizontal (attr 0x40). */
static const unsigned char FRAME_A[4] = { 0x00, 0x00, 0x02, 0x04 };
static const unsigned char FRAME_B[4] = { 0x01, 0x01, 0x03, 0x05 };
static const unsigned char HFLIP[4]   = { 0x00, 0x40, 0x00, 0x00 };

/* Señales para main() y obstacles.c */
unsigned char dot_eaten  = 0;
unsigned int  dot_addr   = 0;
unsigned char bomb_eaten = 0;

/* ------------------------------------------------------------------ */

static unsigned char can_move(unsigned char tx, unsigned char ty,
                               unsigned char dir)
{
    unsigned char t;
    signed char ntx = (signed char)tx + DX[dir];
    signed char nty = (signed char)ty + DY[dir];
    if (ntx < 0 || ntx >= MAP_COLS || nty < 0 || nty >= MAP_ROWS) return 0;
    t = map_state[(unsigned char)nty][(unsigned char)ntx];
    return (t != TILE_WALL && t != TILE_LAVA && t != TILE_VOLCANO);
}

/* ------------------------------------------------------------------ */

void player_init(Player *p)
{
    p->px            = TX_TO_PX(PLAYER_START_TX);
    p->py            = TY_TO_PY(PLAYER_START_TY);
    p->dir           = DIR_LEFT;
    p->next_dir      = DIR_LEFT;
    p->move_cnt      = 0;
    p->anim_frm      = 0;
    p->anim_cnt      = 0;
    p->power_timer   = 0;
    p->slow_cnt      = 0;
    p->tele_lock     = 0;
    p->freeze_timer  = 0;
    p->ctrl_rev_timer = 0;
}

/* ------------------------------------------------------------------ */

/* Lee el pad (ya encuestado por pad_trigger en main.c) y actualiza
   next_dir con el truco de prioridad de dirección de Chase:
   la dirección actual se comprueba primero y se elimina del resto,
   de modo que cualquier otra dirección pulsada simultáneamente la anula. */
static void read_input(Player *p)
{
    unsigned char buttons = pad_state(0);   /* reutiliza el estado leído por pad_trigger */
    unsigned char cur_bit;

    /* Controles invertidos (trampa remolino) */
    if (p->ctrl_rev_timer > 0) {
        unsigned char swapped = 0;
        p->ctrl_rev_timer--;
        if (buttons & PAD_RIGHT) swapped |= PAD_LEFT;
        if (buttons & PAD_LEFT)  swapped |= PAD_RIGHT;
        if (buttons & PAD_UP)    swapped |= PAD_DOWN;
        if (buttons & PAD_DOWN)  swapped |= PAD_UP;
        buttons = swapped | (buttons & ~(PAD_RIGHT|PAD_LEFT|PAD_UP|PAD_DOWN));
    }

    /* Sin input direccional: detener movimiento en próximo tile */
    if (!(buttons & (PAD_RIGHT|PAD_LEFT|PAD_UP|PAD_DOWN))) {
        p->next_dir = DIR_NONE;
        return;
    }

    /* Truco Chase: dirección actual tiene menor prioridad que las nuevas.
       Se comprueba primero y se elimina para que las demás puedan ganar. */
    cur_bit = (p->dir < 4) ? DIR_TO_PAD[p->dir] : 0;
    if (cur_bit && (buttons & cur_bit)) {
        buttons  &= ~cur_bit;       /* eliminar del resto de comprobaciones */
        p->next_dir = p->dir;       /* mantener dirección actual por defecto */
    }

    /* Otras direcciones: cualquiera anula la actual */
    if      (buttons & PAD_RIGHT) p->next_dir = DIR_RIGHT;
    else if (buttons & PAD_LEFT)  p->next_dir = DIR_LEFT;
    else if (buttons & PAD_UP)    p->next_dir = DIR_UP;
    else if (buttons & PAD_DOWN)  p->next_dir = DIR_DOWN;
}

void player_update(Player *p)
{
    unsigned char tx, ty;
    unsigned char eaten;

    /* --- Congelación (árbol / burbuja) --- */
    if (p->freeze_timer > 0) {
        p->freeze_timer--;
        return;   /* hardware ya leído por pad_trigger en main.c */
    }

    /* --- Trampa de velocidad: omitir un frame de cada dos --- */
    tx = PX_TO_TX(p->px);
    ty = PY_TO_TY(p->py);
    if (map_state[ty][tx] == TILE_SLOW_TRAP) {
        p->slow_cnt ^= 1;
        if (p->slow_cnt) {
            read_input(p);   /* bufferizar dirección pero no mover */
            return;
        }
    } else {
        p->slow_cnt = 0;
    }

    /* --- Leer entrada (pad ya encuestado, sin releer hardware) --- */
    read_input(p);

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

        /* Muerte por rayo */
        if (map_state[ty][tx] == TILE_LIGHTNING) {
            player_died = 1;
        }

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
            tx = PX_TO_TX(p->px);
            ty = PY_TO_TY(p->py);
        }

        /* Intentar cambio de dirección solicitado */
        if (p->next_dir != DIR_NONE && can_move(tx, ty, p->next_dir))
            p->dir = p->next_dir;

        /* Avanzar solo si hay input activo */
        if (p->next_dir != DIR_NONE && can_move(tx, ty, p->dir)) {
            p->px = (unsigned char)(p->px + DX[p->dir]);
            p->py = (unsigned char)(p->py + DY[p->dir]);
            p->move_cnt = 7;
        }

        /* Colisión con puntos, pellets, bomba y obstáculos de mapa */
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
        } else if (eaten == TILE_TREE || eaten == TILE_BUBBLE) {
            p->freeze_timer = 60;
            dot_eaten = 1;
            dot_addr  = NTADR_A(MAP_X_OFFSET + tx, MAP_Y_OFFSET + ty);
        } else if (eaten == TILE_WHIRLWIND) {
            p->ctrl_rev_timer = 300;
            dot_eaten = 1;
            dot_addr  = NTADR_A(MAP_X_OFFSET + tx, MAP_Y_OFFSET + ty);
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
    unsigned char dir_idx = (p->dir == DIR_NONE) ? DIR_RIGHT : p->dir;
    unsigned char tile    = (p->anim_frm == 0) ? FRAME_A[dir_idx] : FRAME_B[dir_idx];
    oam_spr(p->px, p->py, tile, HFLIP[dir_idx], 0);
}
