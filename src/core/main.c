#include "neslib.h"
#include "map.h"
#include "score.h"
#include "player.h"
#include "ghost.h"
#include "obstacles.h"

/* Paleta completa (32 bytes): 16 BG + 16 sprites */
static const unsigned char PALETTE[32] = {
    /* BG paleta 0: negro | pared azul | punto blanco | pellet/obstacle amarillo */
    0x0F, 0x12, 0x30, 0x28,
    0x0F, 0x12, 0x30, 0x28,
    0x0F, 0x12, 0x30, 0x28,
    0x0F, 0x12, 0x30, 0x28,
    /* Sprite paleta 0: Pac-Man (amarillo) */
    0x0F, 0x28, 0x16, 0x30,
    /* Sprite paleta 1: fantasma rojo */
    0x0F, 0x16, 0x30, 0x21,
    /* Sprite paleta 2: fantasma cian */
    0x0F, 0x21, 0x30, 0x16,
    /* Sprite paleta 3: fantasma asustado (azul oscuro) */
    0x0F, 0x01, 0x30, 0x21,
};

/* Buffer VRAM: score(8) + dot(3) + 2×tempwall(6) + bomb(3) + EOF = 21 B max */
static unsigned char vram_buf[32];

static void reset_round(Player *p)
{
    player_init(p);
    ghost_init_all();
    /* Las paredes temporales y la bomba mantienen su estado entre rondas */
}

void main(void) {
    static Player player;
    unsigned char n, sprid;
    unsigned char was_powered;

    /* ------------------------------------------------------------------ */
    /* Inicialización — renderizado deshabilitado                         */
    /* ------------------------------------------------------------------ */
    ppu_off();

    pal_all(PALETTE);

    vram_adr(NAMETABLE_A);
    vram_fill(TILE_EMPTY, 0x400);

    map_init();
    map_render();

    score_init();
    {
        unsigned char i;
        vram_adr(NTADR_A(SCORE_COL, SCORE_ROW));
        for (i = 0; i < SCORE_DIGITS; i++) vram_put(DIGIT_BASE);
    }

    scroll(0, 0);

    oam_clear();
    player_init(&player);
    ghost_init_all();
    obstacles_init();

    vram_buf[0] = NT_UPD_EOF;
    set_vram_update(vram_buf);

    ppu_on_all();

    /* ------------------------------------------------------------------ */
    /* Bucle principal — ~60 Hz NTSC                                      */
    /* ------------------------------------------------------------------ */
    while (1) {
        was_powered = (player.power_timer > 0);

        /* --- Lógica --- */
        oam_clear();
        player_update(&player);
        ghost_update_all(&player);

        if (!was_powered && player.power_timer > 0)
            ghost_scare_all();

        if (pacman_died)
            reset_round(&player);

        /* --- Dibujo OAM --- */
        player_draw(&player);
        sprid = ghost_draw_all(&player, 4);
        oam_hide_rest(sprid);

        /* --- Buffer VRAM: score + dot + obstáculos --- */
        n = score_build_update(vram_buf);

        if (dot_eaten) {
            vram_buf[n++] = (unsigned char)(dot_addr >> 8);
            vram_buf[n++] = (unsigned char)(dot_addr & 0xFF);
            vram_buf[n++] = TILE_EMPTY;
            dot_eaten = 0;
        }

        /* obstacles_update añade entradas para paredes temporales y bomba */
        obstacles_update(&player, vram_buf, &n);

        vram_buf[n] = NT_UPD_EOF;
        set_vram_update(vram_buf);

        ppu_wait_frame();
    }
}
