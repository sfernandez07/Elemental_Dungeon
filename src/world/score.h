#ifndef SCORE_H
#define SCORE_H

/* Número de dígitos mostrados en pantalla (max display: 99999) */
#define SCORE_DIGITS    5

/* Posición HUD en el nametable (fila 0, columna 12 — centro) */
#define SCORE_COL      12
#define SCORE_ROW       0

/* Tile BG base para los dígitos: tile $04 = '0', $05 = '1', ... $0D = '9' */
#define DIGIT_BASE      0x04

/* Puntos por tipo de ítem */
#define DOT_PTS         10
#define PELLET_PTS      50

/* Puntuación acumulada */
extern unsigned int  score;

/* Flag: 1 si el score cambió y hay que actualizar el nametable */
extern unsigned char score_dirty;

/* Slots del HUD de vidas (columna 2, fila 0, 3 tiles) */
#define LIVES_HUD_COL   2
#define LIVES_HUD_ROW   0
#define LIVES_HUD_SLOTS 3

void          score_init(void);
void          score_add(unsigned int pts);

/* Escribe en buf la secuencia de actualización VRAM para el marcador.
   Devuelve el número de bytes escritos (0 si score_dirty == 0). */
unsigned char score_build_update(unsigned char *buf);

/* Añade la actualización VRAM del HUD de vidas a buf a partir de la posición n.
   Devuelve la nueva posición n. Limpia *dirty si era 1. */
unsigned char score_lives_update(unsigned char *buf, unsigned char n,
                                 unsigned char lives_count,
                                 unsigned char *dirty);

#endif
