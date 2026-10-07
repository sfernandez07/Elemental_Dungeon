#ifndef GAME_H
#define GAME_H

#include "player.h"

/* Estados del juego */
#define STATE_PLAYING     0
#define STATE_DEAD        1
#define STATE_GAMEOVER    2
#define STATE_WIN         3
#define STATE_TITLE       4
#define STATE_PAUSED      5
#define STATE_LEVEL_INTRO 6

#define LIVES_INITIAL         3
#define DEATH_DELAY          90   /* frames animación muerte (~1.5 s) */
#define LEVEL_INTRO_DURATION 120  /* frames intro de nivel (2 s) */

extern unsigned char game_state;
extern unsigned char lives;
extern unsigned char game_death_timer;

/* Inicializa el estado global del juego (llamar con PPU habilitada). */
void game_init(Player *p);

/* Actualiza la máquina de estados un frame. Llamar después de
   player_update / ghost_update_all en STATE_PLAYING.
   pad_trig: botones recién pulsados (resultado de pad_trigger) cuando el
   estado es STATE_PLAYING; pasar 0 en cualquier otro estado (game_update
   llama a pad_trigger internamente para los demás estados). */
void game_update(Player *p, unsigned char pad_trig);

/* Muestra la pantalla de título y activa STATE_TITLE.
   Puede llamarse con PPU encendida o apagada. */
void game_title_screen(void);

#endif
