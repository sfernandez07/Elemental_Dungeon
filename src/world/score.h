#ifndef SCORE_H
#define SCORE_H

/* Tile BG base para los dígitos: tile $04 = '0', $05 = '1', ... $0D = '9'
   (lo usan text.c y la pantalla de introducción de nivel) */
#define DIGIT_BASE      0x04

/* Puntos por tipo de ítem */
#define DOT_PTS         10
#define PELLET_PTS      50

/* Puntuación acumulada de la partida (no se muestra en pantalla) */
extern unsigned int  score;

void          score_init(void);
void          score_add(unsigned int pts);

#endif
