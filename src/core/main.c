#include "neslib.h"
#include "map.h"
#include "player.h"
#include "ghost.h"
#include "obstacles.h"
#include "game.h"
#include "sound.h"

/* Paleta completa (32 bytes): 16 BG + 16 sprites */
static const char PALETTE[32] = {
    /* BG paleta 0: negro | pared azul | punto blanco | pellet/obstacle amarillo */
    0x0F, 0x12, 0x30, 0x28,
    0x0F, 0x12, 0x30, 0x28,
    0x0F, 0x12, 0x30, 0x28,
    0x0F, 0x12, 0x30, 0x28,
    /* Sprite paleta 0: Héroe (plata) */
    0x0F, 0x28, 0x16, 0x30,
    /* Sprite paleta 1: fantasma rojo */
    0x0F, 0x16, 0x30, 0x21,
    /* Sprite paleta 2: fantasma cian */
    0x0F, 0x21, 0x30, 0x16,
    /* Sprite paleta 3: fantasma asustado (azul oscuro) */
    0x0F, 0x01, 0x30, 0x21,
};

/* Buffer VRAM (160 B). Peor caso por frame: punto(3) + puerta(3) + parpadeo de pellets(12)
   + obstáculos(87: una fila de lava 78, paredes temporales 6, bomba 3) + EOF(1) = 106 B */
static unsigned char vram_buf[160];

/* Parpadeo de power pellets: posiciones fijas en todos los mapas */
static const unsigned char PELLET_TX[4] = { 1, 26,  1, 26 };
static const unsigned char PELLET_TY[4] = { 3,  3, 20, 20 };
static unsigned char pellet_blink_tmr   = 0;
static unsigned char pellet_blink_vis   = 1;

void main(void) {
    static Player player;
    unsigned char n, sprid;
    unsigned char was_powered;

    /* ------------------------------------------------------------------ */
    /* Inicialización — PPU deshabilitada                                  */
    /* ------------------------------------------------------------------ */
    ppu_off();
    pal_all(PALETTE);
    scroll(0, 0);
    sound_init();
    game_init(&player);      /* establece STATE_TITLE                     */
    game_title_screen();     /* dibuja el título y llama a ppu_on_all()   */

    /* El resto de la inicialización (mapa, jugador, etc.) ocurre dentro
       de do_full_restart() cuando el jugador pulsa Start en el título. */

    /* ------------------------------------------------------------------ */
    /* Bucle principal — lógica a 50 Hz: en NTSC, ppu_wait_frame() salta  */
    /* 1 de cada 6 frames para igualar la velocidad de PAL                */
    /* ------------------------------------------------------------------ */
    while (1) {
        unsigned char pad_trig = 0;
        was_powered = (player.power_timer > 0);

        oam_clear();

        /* --- Lógica según estado --- */
        pad_trig = pad_trigger(0);   /* una sola lectura por frame en todos los estados */

        /* Si este frame se pausa, no se mueve nada: el buffer VRAM no se
           construye en pausa y un punto o la bomba recogidos se perderían. */
        if (game_state == STATE_PLAYING && !(pad_trig & PAD_START)) {
            player_update(&player);
            ghost_update_all(&player);

            if (!was_powered && player.power_timer > 0) {
                ghost_scare_all();
                sound_play(SFX_PELLET);
            }
        }

        game_update(&player, pad_trig);   /* gestiona transiciones de estado */

        /* --- Dibujo OAM --- */
        if (game_state == STATE_PLAYING || game_state == STATE_PAUSED) {
            player_draw(&player);
            sprid = ghost_draw_all(&player, 4);
            oam_hide_rest(sprid);
        } else if (game_state == STATE_DEAD) {
            /* Animación de muerte en 3 fases */
            if (game_death_timer > 60) {
                /* Fase 1 (90–61): parpadeo rápido cada 4 frames */
                if (game_death_timer & 0x04)
                    player_draw(&player);
            } else if (game_death_timer > 30) {
                /* Fase 2 (60–31): explosión grande */
                oam_spr(player.px, player.py, 0x0A, 0x00, 0);
            } else if (game_death_timer > 0) {
                /* Fase 3 (30–1): explosión pequeña */
                oam_spr(player.px, player.py, 0x0B, 0x00, 0);
            }
            oam_hide_rest(4);
        } else {
            /* TITLE / GAMEOVER / WIN / LEVEL_INTRO: sin sprites */
            oam_hide_rest(0);
        }

        /* --- Buffer VRAM (solo mientras el juego está activo) --- */
        if (game_state == STATE_PLAYING || game_state == STATE_DEAD) {
            n = 0;

            if (dot_eaten) {
                sound_play(SFX_DOT);
                vram_buf[n++] = (unsigned char)(dot_addr >> 8);
                vram_buf[n++] = (unsigned char)(dot_addr & 0xFF);
                vram_buf[n++] = TILE_EMPTY;
                dot_eaten = 0;
            }

            if (door_dirty) {
                unsigned int daddr = NTADR_A(MAP_X_OFFSET + DOOR_TX,
                                             MAP_Y_OFFSET + DOOR_TY);
                door_dirty = 0;
                vram_buf[n++] = (unsigned char)(daddr >> 8);
                vram_buf[n++] = (unsigned char)(daddr & 0xFF);
                vram_buf[n++] = TILE_DOOR_OPEN;
            }

            /* Parpadeo pellets: alternar visible/oculto cada 16 frames */
            pellet_blink_tmr++;
            if (pellet_blink_tmr >= 16) {
                unsigned char pi;
                pellet_blink_tmr = 0;
                pellet_blink_vis ^= 1;
                for (pi = 0; pi < 4; pi++) {
                    if (map_state[PELLET_TY[pi]][PELLET_TX[pi]] == TILE_PELLET) {
                        unsigned int paddr = NTADR_A(MAP_X_OFFSET + PELLET_TX[pi],
                                                     MAP_Y_OFFSET + PELLET_TY[pi]);
                        vram_buf[n++] = (unsigned char)(paddr >> 8);
                        vram_buf[n++] = (unsigned char)(paddr & 0xFF);
                        vram_buf[n++] = pellet_blink_vis ? TILE_PELLET : TILE_EMPTY;
                    }
                }
            }

            if (game_state == STATE_PLAYING)
                obstacles_update(&player, vram_buf, &n);

            vram_buf[n] = NT_UPD_EOF;
            set_vram_update(vram_buf);
        }
        /* En TITLE/GAMEOVER/WIN, set_vram_update apunta a eof_buf (game.c) */

        sound_update();
        ppu_wait_frame();
    }
}
