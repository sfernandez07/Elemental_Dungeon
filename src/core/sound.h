#ifndef SOUND_H
#define SOUND_H

/* IDs de efectos de sonido */
#define SFX_NONE        0
#define SFX_DOT         1   /* comer un punto            prio 1 */
#define SFX_PELLET      2   /* comer power pellet        prio 2 */
#define SFX_GHOST_EAT   3   /* comer fantasma asustado   prio 3 */
#define SFX_BOMB        4   /* explosión de bomba        prio 4 */
#define SFX_DEATH       5   /* el héroe muere            prio 5 */

/* Inicializa el APU y silencia todos los canales.
   Llamar con el renderizado deshabilitado o al inicio de cada partida. */
void sound_init(void);

/* Solicita reproducir el efecto sfx_id.
   Solo reemplaza el sonido actual si la prioridad es mayor o igual. */
void sound_play(unsigned char sfx_id);

/* Avanza el driver un frame. Llamar una vez por frame en el bucle principal. */
void sound_update(void);

/* Inicia la música de fondo (Triangle + Noise). */
void bgm_start(void);

/* Detiene la música de fondo y silencia los canales. */
void bgm_stop(void);

/* Pausa (paused=1) o reanuda (paused=0) la música. */
void bgm_pause(unsigned char paused);

#endif
