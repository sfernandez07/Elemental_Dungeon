#include "neslib.h"
#include "map.h"
#include "score.h"
#include "player.h"
#include "ghost.h"
#include "obstacles.h"
#include "game.h"
#include "sound.h"
#include "text.h"

/* ------------------------------------------------------------------ */
/* Variables globales                                                  */
/* ------------------------------------------------------------------ */

unsigned char game_state;
unsigned char lives;
unsigned char lives_dirty;
unsigned char game_death_timer;

static unsigned char level_intro_timer;
static unsigned char title_cheat_step;
static unsigned char eof_buf[1];

/* ← ← → → ↑ ↓ ↑ ↓  (PAD_LEFT=0x02, PAD_RIGHT=0x01, PAD_UP=0x08, PAD_DOWN=0x04) */
static const unsigned char TITLE_CHEAT[8] = {0x02, 0x02, 0x01, 0x01, 0x08, 0x04, 0x08, 0x04};

/* ------------------------------------------------------------------ */
/* Paletas por nivel (BG×4 subpaletas + sprites fijos)                */
/* ------------------------------------------------------------------ */

/* BG: [fondo, paredes, puntos, pellet/obstacle] × 4 subpaletas      */
/* Sprites: paleta 0 jugador, 1 enemigo rojo, 2 enemigo cian, 3 miedo*/
static const char LEVEL_PALETTES[LEVEL_COUNT][32] = {
    /* 0 — Agua: paredes azul, puntos cian, pellets blanco */
    { 0x0F,0x12,0x21,0x30, 0x0F,0x12,0x21,0x30,
      0x0F,0x12,0x21,0x30, 0x0F,0x12,0x21,0x30,
      0x0F,0x28,0x16,0x30, 0x0F,0x16,0x30,0x21,
      0x0F,0x21,0x30,0x16, 0x0F,0x01,0x30,0x21 },
    /* 1 — Viento: paredes gris, puntos blanco, pellets amarillo pálido */
    { 0x0F,0x10,0x30,0x38, 0x0F,0x10,0x30,0x38,
      0x0F,0x10,0x30,0x38, 0x0F,0x10,0x30,0x38,
      0x0F,0x28,0x16,0x30, 0x0F,0x16,0x30,0x21,
      0x0F,0x21,0x30,0x16, 0x0F,0x01,0x30,0x21 },
    /* 2 — Bosque: paredes verde oscuro, puntos verde, pellets amarillo */
    { 0x0F,0x0A,0x2A,0x28, 0x0F,0x0A,0x2A,0x28,
      0x0F,0x0A,0x2A,0x28, 0x0F,0x0A,0x2A,0x28,
      0x0F,0x28,0x16,0x30, 0x0F,0x16,0x30,0x21,
      0x0F,0x21,0x30,0x16, 0x0F,0x01,0x30,0x21 },
    /* 3 — Fuego: paredes rojo oscuro, puntos naranja, pellets amarillo */
    { 0x0F,0x06,0x26,0x28, 0x0F,0x06,0x26,0x28,
      0x0F,0x06,0x26,0x28, 0x0F,0x06,0x26,0x28,
      0x0F,0x28,0x16,0x30, 0x0F,0x16,0x30,0x21,
      0x0F,0x21,0x30,0x16, 0x0F,0x01,0x30,0x21 },
    /* 4 — Rayo: paredes púrpura oscuro, puntos amarillo, pellets blanco */
    { 0x0F,0x04,0x28,0x30, 0x0F,0x04,0x28,0x30,
      0x0F,0x04,0x28,0x30, 0x0F,0x04,0x28,0x30,
      0x0F,0x28,0x16,0x30, 0x0F,0x16,0x30,0x21,
      0x0F,0x21,0x30,0x16, 0x0F,0x01,0x30,0x21 },
};

/* ------------------------------------------------------------------ */
/* Helpers internos                                                    */
/* ------------------------------------------------------------------ */

static void show_title(void)
{
    unsigned char i;
    ppu_off();
    pal_all(LEVEL_PALETTES[0]);
    vram_adr(NAMETABLE_A);
    vram_fill(TILE_EMPTY, 0x400);

    /* Borde doble de paredes: filas 0-1 y 28-29 */
    vram_adr(NTADR_A(0, 0));  vram_fill(TILE_WALL, 64);
    vram_adr(NTADR_A(0, 28)); vram_fill(TILE_WALL, 64);

    /* Columnas laterales filas 2-27 (col 0 oculta por PPUMASK → borde izq en 1-2, der en 30-31) */
    for (i = 2; i < 28; i++) {
        vram_adr(NTADR_A(1,  i)); vram_put(TILE_WALL);
        vram_adr(NTADR_A(2,  i)); vram_put(TILE_WALL);
        vram_adr(NTADR_A(30, i)); vram_put(TILE_WALL);
        vram_adr(NTADR_A(31, i)); vram_put(TILE_WALL);
    }

    /* Línea de puntos con pellets en esquinas — fila 5 */
    vram_adr(NTADR_A(3, 5));
    vram_put(TILE_PELLET);
    for (i = 0; i < 25; i++) vram_put(TILE_DOT);
    vram_put(TILE_PELLET);

    /* Línea de puntos con pellets — fila 22 */
    vram_adr(NTADR_A(3, 22));
    vram_put(TILE_PELLET);
    for (i = 0; i < 25; i++) vram_put(TILE_DOT);
    vram_put(TILE_PELLET);

    /* Fila decorativa alternando pellet/punto — fila 8 */
    vram_adr(NTADR_A(3, 8));
    for (i = 0; i < 27; i++) vram_put((i & 1) ? TILE_DOT : TILE_PELLET);

    /* Texto del título centrado */
    draw_text(11, 11, "ELEMENTAL");
    draw_text(12, 13, "DUNGEON");
    draw_text(10, 18, "PRESS START");

    oam_clear();
    eof_buf[0] = NT_UPD_EOF;
    set_vram_update(eof_buf);
    ppu_on_all();
}

static void show_screen(unsigned char is_win)
{
    unsigned char i;
    bgm_stop();
    ppu_off();
    pal_all(LEVEL_PALETTES[0]);
    vram_adr(NAMETABLE_A);
    vram_fill(TILE_EMPTY, 0x400);

    /* Borde de paredes doble */
    vram_adr(NTADR_A(0, 0));  vram_fill(TILE_WALL, 64);
    vram_adr(NTADR_A(0, 28)); vram_fill(TILE_WALL, 64);
    for (i = 2; i < 28; i++) {
        vram_adr(NTADR_A(1,  i)); vram_put(TILE_WALL);
        vram_adr(NTADR_A(2,  i)); vram_put(TILE_WALL);
        vram_adr(NTADR_A(30, i)); vram_put(TILE_WALL);
        vram_adr(NTADR_A(31, i)); vram_put(TILE_WALL);
    }

    /* Líneas de puntos */
    vram_adr(NTADR_A(3, 5));
    vram_put(TILE_PELLET);
    for (i = 0; i < 25; i++) vram_put(TILE_DOT);
    vram_put(TILE_PELLET);

    vram_adr(NTADR_A(3, 22));
    vram_put(TILE_PELLET);
    for (i = 0; i < 25; i++) vram_put(TILE_DOT);
    vram_put(TILE_PELLET);

    if (is_win) {
        draw_text(12, 13, "YOU WIN!");
    } else {
        draw_text(11, 13, "GAME OVER");
    }
    draw_text(10, 16, "PRESS START");

    oam_clear();
    eof_buf[0] = NT_UPD_EOF;
    set_vram_update(eof_buf);
    ppu_on_all();
}

/* Muestra la pantalla de inicio de nivel durante STATE_LEVEL_INTRO. */
static void show_level_intro(void)
{
    unsigned char i, j;
    unsigned char sym;
    unsigned char name_col;
    const char *name;

    switch (current_level) {
        case 1: name_col = 10; name = "MUNDO VIENTO"; sym = TILE_WHIRLWIND; break;
        case 2: name_col = 10; name = "MUNDO BOSQUE"; sym = TILE_TREE;      break;
        case 3: name_col = 10; name = "MUNDO FUEGO";  sym = TILE_VOLCANO;   break;
        case 4: name_col = 11; name = "MUNDO RAYO";   sym = TILE_LIGHTNING; break;
        default: name_col = 11; name = "MUNDO AGUA";  sym = TILE_BUBBLE;    break;
    }

    ppu_off();
    pal_all(LEVEL_PALETTES[current_level]);

    /* Borde de pared completo + interior vacío (cols 1-2 y 30-31 quedan como borde) */
    vram_adr(NAMETABLE_A);
    vram_fill(TILE_WALL, 0x400);
    for (i = 2; i < 28; i++) {
        vram_adr(NTADR_A(3, i));
        vram_fill(TILE_EMPTY, 27);
    }

    /* Líneas de puntos con pellets en esquinas */
    vram_adr(NTADR_A(3, 3));
    vram_put(TILE_PELLET);
    for (i = 0; i < 25; i++) vram_put(TILE_DOT);
    vram_put(TILE_PELLET);

    vram_adr(NTADR_A(3, 26));
    vram_put(TILE_PELLET);
    for (i = 0; i < 25; i++) vram_put(TILE_DOT);
    vram_put(TILE_PELLET);

    /* Bloque de símbolo temático: 10 ancho × 4 filas, centrado */
    for (j = 6; j < 10; j++) {
        vram_adr(NTADR_A(11, j));
        for (i = 0; i < 10; i++) vram_put(sym);
    }

    /* Nombre del mundo y número de nivel */
    draw_text(name_col, 13, name);
    draw_text(12, 16, "NIVEL ");
    vram_put(DIGIT_BASE + current_level + 1);
    draw_text(10, 20, "PRESS START");

    oam_clear();
    eof_buf[0] = NT_UPD_EOF;
    set_vram_update(eof_buf);
    ppu_on_all();
}

/* Dibuja (show=1) o borra (show=0) el overlay de pausa. */
static void pause_overlay(unsigned char show)
{
    unsigned char j;
    bgm_pause(show);
    ppu_off();
    if (show) {
        draw_text(13, 14, "PAUSED");
    } else {
        /* Restaurar los 6 tiles del mapa que "PAUSED" ocupa:
           nametable (13,14) → map_state[ty=13][tx=11..16] */
        vram_adr(NTADR_A(13, 14));
        for (j = 0; j < 6; j++)
            vram_put(map_state[13][11 + j]);
    }
    eof_buf[0] = NT_UPD_EOF;
    set_vram_update(eof_buf);
    ppu_on_all();
}

/* Renderiza el nivel actual en RAM con PPU apagada y arranca el juego.
   reset_score=1 → reinicia marcador y lo dibuja; 0 → solo marca dirty. */
static void level_load(Player *p, unsigned char reset_score)
{
    unsigned char i;

    ppu_off();
    pal_all(LEVEL_PALETTES[current_level]);
    vram_adr(NAMETABLE_A);
    vram_fill(TILE_EMPTY, 0x400);

    map_init();
    map_render();

    score_dirty = 1;
    if (reset_score) {
        score_init();
        vram_adr(NTADR_A(SCORE_COL, SCORE_ROW));
        for (i = 0; i < SCORE_DIGITS; i++) vram_put(DIGIT_BASE);
    }

    vram_adr(NTADR_A(2, 0));
    for (i = 0; i < lives; i++)             vram_put(TILE_PELLET);
    for (i = lives; i < LIVES_INITIAL; i++) vram_put(TILE_EMPTY);

    scroll(0, 0);
    player_init(p);
    ghost_init_all();
    obstacles_init();

    game_state       = STATE_PLAYING;
    game_death_timer = 0;
    lives_dirty      = 0;

    eof_buf[0] = NT_UPD_EOF;
    set_vram_update(eof_buf);
    ppu_on_all();

    /* Descarta un Start pulsado durante la carga: si no, el primer frame de
       juego lo vería como pulsación nueva y entraría directamente en pausa. */
    pad_trigger(0);
}


static void do_full_restart(Player *p)
{
    current_level = 0;
    lives         = LIVES_INITIAL;
    sound_init();
    bgm_start();
    show_level_intro();
    level_intro_timer = LEVEL_INTRO_DURATION;
    game_state        = STATE_LEVEL_INTRO;
    (void)p;
}

/* ------------------------------------------------------------------ */
/* API pública                                                         */
/* ------------------------------------------------------------------ */

void game_init(Player *p)
{
    game_state        = STATE_TITLE;
    lives             = LIVES_INITIAL;
    lives_dirty       = 0;
    game_death_timer  = 0;
    level_intro_timer = 0;
    current_level     = 0;
    eof_buf[0]        = NT_UPD_EOF;
    (void)p;
}

void game_update(Player *p, unsigned char pad_trig)
{
    switch (game_state) {

    case STATE_PLAYING:
        /* pad_trig ya fue leído una sola vez en main.c (estilo Chase) */
        if (pad_trig & PAD_START) {
            pause_overlay(1);
            game_state = STATE_PAUSED;
            break;
        }
        if (player_died) {
            player_died      = 0;
            game_state       = STATE_DEAD;
            game_death_timer = DEATH_DELAY;
            bgm_stop();
            sound_play(SFX_DEATH);
            break;
        }
        if (dots_remaining == 0 && !door_open) {
            map_open_door();
            ghost_rage_all();
        }
        if (door_open) {
            unsigned char ptx = PX_TO_TX(p->px);
            unsigned char pty = PY_TO_TY(p->py);
            if (ptx == DOOR_TX && pty == DOOR_TY) {
                if (current_level < LEVEL_COUNT - 1) {
                    current_level++;
                    show_level_intro();
                    level_intro_timer = LEVEL_INTRO_DURATION;
                    game_state = STATE_LEVEL_INTRO;
                } else {
                    show_screen(1);
                    game_state = STATE_WIN;
                }
            }
        }
        break;

    case STATE_PAUSED:
        if (pad_trig & PAD_START) {
            pause_overlay(0);
            game_state = STATE_PLAYING;
        }
        break;

    case STATE_LEVEL_INTRO:
        if ((pad_trig & PAD_START) || level_intro_timer == 0) {
            level_load(p, current_level == 0 ? 1 : 0);
        } else {
            level_intro_timer--;
        }
        break;

    case STATE_DEAD:
        if (game_death_timer > 0) {
            game_death_timer--;
            break;
        }
        if (lives > 0) lives--;
        lives_dirty = 1;

        if (lives == 0) {
            show_screen(0);
            game_state = STATE_GAMEOVER;
        } else {
            player_init(p);
            ghost_init_all();
            if (door_open) ghost_rage_all();
            bgm_start();
            game_state = STATE_PLAYING;
        }
        break;

    case STATE_GAMEOVER:
    case STATE_WIN:
        if (pad_trig & PAD_START) {
            show_title();
            game_state = STATE_TITLE;
        }
        break;

    case STATE_TITLE: {
        if (pad_trig) {
            if (pad_trig == TITLE_CHEAT[title_cheat_step]) {
                title_cheat_step++;
                if (title_cheat_step == 8) {
                    title_cheat_step = 0;
                    show_screen(1);
                    game_state = STATE_WIN;
                    break;
                }
            } else {
                title_cheat_step = (pad_trig == TITLE_CHEAT[0]) ? 1 : 0;
            }
        }
        if (pad_trig & PAD_START)
            do_full_restart(p);
        break;
    }
    }
}

void game_title_screen(void) { show_title(); }
