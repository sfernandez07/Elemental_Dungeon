#ifndef OBSTACLES_H
#define OBSTACLES_H

#include "player.h"

/* Temporización de la pared temporal */
#define TEMPWALL_PERIOD    120   /* 2 s a 60 Hz por estado (on u off) */
#define TEMPWALL_COUNT       2   /* número de paredes temporales en el mapa */

/* Bomba */
#define BOMB_RESPAWN_TIME  600   /* ~10 s: tiempo hasta que reaparece */
#define BOMB_EFFECT_TIME   180   /* ~3 s: tiempo que los fantasmas permanecen DEAD */

/* Volcanes (MAP_3) */
#define VOLCANO_COUNT        4
#define LAVA_SLOT_COUNT      2
#define LAVA_DURATION      240   /* 4 s */
#define VOLCANO_DORMANT_MIN 300
#define VOLCANO_DORMANT_RNG 300  /* dormant = MIN + rand8()%RNG */

/* Rayo (MAP_4) */
#define LIGHTNING_DURATION 120   /* 2 s */
#define LIGHTNING_SPAWN_MIN 180
#define LIGHTNING_SPAWN_RNG  76  /* spawn = MIN + rand8()%RNG → 3–4.25 s */

/* Inicializa el estado interno de los obstáculos.
   Llamar con la PPU deshabilitada; escribe los tiles directamente en VRAM. */
void obstacles_init(void);

/* Actualiza los obstáculos un frame:
     - Paredes temporales: toggle + actualiza map_state + añade VRAM update.
     - Bomba: gestiona respawn + añade VRAM update si reaparece.
   Añade las entradas necesarias a buf[] a partir de la posición *plen,
   y actualiza *plen al nuevo tamaño. */
void obstacles_update(Player *p,
                      unsigned char *buf, unsigned char *plen);

#endif
