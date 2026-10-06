#ifndef PLAYER_H
#define PLAYER_H

#define DIR_RIGHT  0
#define DIR_LEFT   1
#define DIR_UP     2
#define DIR_DOWN   3
#define DIR_NONE   4

/* Velocidad (píxeles/frame) y posición inicial en tiles */
#define PLAYER_SPEED     1
#define PLAYER_START_TX  13
#define PLAYER_START_TY  26

/* Duración del modo power-up en frames (5 s a 60 Hz) */
#define POWER_DURATION   300

typedef struct {
    unsigned char px;           /* posición X en pantalla (píxeles) */
    unsigned char py;           /* posición Y en pantalla (píxeles) */
    unsigned char dir;          /* dirección actual (DIR_*) */
    unsigned char next_dir;     /* dirección solicitada por el jugador */
    unsigned char move_cnt;     /* píxeles restantes hasta el siguiente tile */
    unsigned char anim_frm;     /* frame de animación: 0=boca abierta, 1=cerrada */
    unsigned char anim_cnt;     /* contador de frames para animación */
    unsigned int  power_timer;  /* frames restantes en modo power-up (0=inactivo); 16 bits: 300 no cabe en 8 */
    unsigned char slow_cnt;     /* paridad para trampa de velocidad (alterna 0/1) */
    unsigned char tele_lock;    /* frames de bloqueo tras teleportarse */
    unsigned char freeze_timer; /* frames de inmovilización (árbol/burbuja) */
    unsigned int  ctrl_rev_timer; /* frames con controles invertidos (remolino) */
} Player;

/* Señales que player_update deja para que main() construya el buffer VRAM */
extern unsigned char dot_eaten;   /* 1 = hay que borrar dot_addr del nametable */
extern unsigned int  dot_addr;    /* dirección PPU del tile a borrar */
extern unsigned char bomb_eaten;  /* 1 = el héroe acaba de recoger la bomba */

void player_init(Player *p);
void player_update(Player *p);
void player_draw(const Player *p);

#endif
