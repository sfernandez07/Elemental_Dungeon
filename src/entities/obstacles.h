#ifndef OBSTACLES_H
#define OBSTACLES_H

#include "player.h"

/* Temporización de la pared temporal:
   Los dos bloqueadores alternan cada TEMPWALL_PERIOD frames
   (aparecen → desaparecen → aparecen, …) */
#define TEMPWALL_PERIOD    120   /* 2 s a 60 Hz por estado (on u off) */

/* Bomba */
#define BOMB_RESPAWN_TIME  600   /* ~10 s: tiempo hasta que reaparece */
#define BOMB_EFFECT_TIME   180   /* ~3 s: tiempo que los fantasmas permanecen DEAD */

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
