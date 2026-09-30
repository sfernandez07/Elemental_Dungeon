#include "sound.h"

/* ------------------------------------------------------------------ */
/* Registros APU (I/O mapeado en memoria)                             */
/* ------------------------------------------------------------------ */

#define WREG(addr, val) (*((volatile unsigned char*)(addr)) = (val))

/* Pulse 1 */
#define P1_CTRL   0x4000
#define P1_SWEEP  0x4001
#define P1_LO     0x4002
#define P1_HI     0x4003
/* Triangle */
#define TRI_LINEAR 0x4008  /* C RRRRRRR: control/halt + linear counter reload */
#define TRI_LO     0x400A
#define TRI_HI     0x400B
/* Noise */
#define NOI_CTRL   0x400C  /* -- LC -- VVVV: loop, const-vol, volume */
#define NOI_PERIOD 0x400E  /* M --- PPPP: mode, period index */
#define NOI_LEN    0x400F  /* LLLLL ---: length counter index */
/* APU */
#define APU_STS   0x4015   /* enable channels: bit0=P1, bit1=P2, bit2=TRI, bit3=NOISE */
#define APU_FC    0x4017   /* frame counter: $40 = 4-step, IRQ off */

/* ------------------------------------------------------------------ */
/* Música de fondo — Triangle (melodía) + Noise (ritmo)               */
/* ------------------------------------------------------------------ */

/* Períodos del canal Triangle para notas en rango A3–A4 (NTSC, hi=0)
   Fórmula: period = 1789773 / (32 * freq_Hz) - 1               */
#define N_REST 0x00   /* silencio: apagar linear counter            */
#define N_A3   253    /* 220 Hz */
#define N_B3   225    /* 247 Hz */
#define N_C4   213    /* 262 Hz */
#define N_D4   189    /* 294 Hz */
#define N_E4   169    /* 330 Hz */
#define N_F4   159    /* 349 Hz */
#define N_G4   142    /* 392 Hz */
#define N_A4   126    /* 440 Hz */
#define N_END  0xFF   /* marcador de fin → reiniciar bucle          */

/* Melodía en La menor: pares [duración_frames, nota]
   Total: 192 frames (~3.2 s a 60 Hz, sincroniza con 6 ciclos de ritmo) */
static const unsigned char MELODY[] = {
    16, N_A3,   8, N_C4,   8, N_E4,  16, N_A4,
     8, N_G4,   8, N_E4,  16, N_C4,  16, N_E4,
     8, N_D4,   8, N_F4,  16, N_G4,   8, N_A4,
     8, N_G4,   8, N_E4,  16, N_C4,  24, N_A3,
    N_END
};

/* Ritmo de ruido: pares [duración_frames, periodo_noise]
   0x0F=sentinela silencio; total 32 frames por ciclo (~7.5 rep/loop) */
#define DR_KICK  0x00   /* noise period 0, short mode off → kick bajo   */
#define DR_SNARE 0x04   /* noise period 4                 → caja         */
#define DR_HIHAT 0x06   /* noise period 6                 → hi-hat       */
#define DR_SIL   0x0F   /* sentinel: silenciar noise                     */

static const unsigned char RHYTHM[] = {
    3, DR_KICK,  5, DR_SIL,
    3, DR_HIHAT, 5, DR_SIL,
    3, DR_SNARE, 5, DR_SIL,
    3, DR_HIHAT, 5, DR_SIL,
    N_END
};

/* Estado del driver de música */
static unsigned char            mus_active;
static const unsigned char     *mel_pos;
static unsigned char            mel_tmr;
static const unsigned char     *rhy_pos;
static unsigned char            rhy_tmr;
static unsigned char            mus_paused;

/* ------------------------------------------------------------------ */
/* Datos de efectos en ROM                                             */
/* ------------------------------------------------------------------ */
/*  Formato de cada paso: [duracion_frames, ctrl($4000), lo($4002), hi($4003)]
    Fin de efecto: byte 0x00                                           */

/* SFX_DOT — C5 (523 Hz, period $0D5), 3 frames */
static const unsigned char DAT_DOT[] = {
    3, 0xBF, 0xD5, 0x00,
    0
};

/* SFX_PELLET — E4 → G4 → C5 ascendente */
static const unsigned char DAT_PELLET[] = {
    6, 0xBF, 0x51, 0x01,   /* E4  period $151 */
    6, 0xBF, 0x1C, 0x01,   /* G4  period $11C */
    8, 0xBF, 0xD5, 0x00,   /* C5  period $0D5 */
    0
};

/* SFX_GHOST_EAT — G4 → E4 descendente rápido */
static const unsigned char DAT_GHOST[] = {
    5, 0xBF, 0x1C, 0x01,   /* G4 */
    5, 0xBF, 0x51, 0x01,   /* E4 */
    0
};

/* SFX_BOMB — pulso muy grave que decae */
static const unsigned char DAT_BOMB[] = {
    8, 0xBF, 0x54, 0x03,   /* C3  period $354, muy grave */
    8, 0x8F, 0xA9, 0x01,   /* C4  period $1A9, 25% duty, vol=15 */
    0
};

/* SFX_DEATH — C4 → A3 → G3 → E3 → C3 descendente lento */
static const unsigned char DAT_DEATH[] = {
    15, 0xBF, 0xA9, 0x01,  /* C4  period $1A9 */
    15, 0xBF, 0xFB, 0x01,  /* A3  period $1FB */
    15, 0xBF, 0x39, 0x02,  /* G3  period $239 */
    15, 0xBF, 0xA4, 0x02,  /* E3  period $2A4 */
    20, 0xBF, 0x54, 0x03,  /* C3  period $354 */
    0
};

/* ------------------------------------------------------------------ */
/* Estado del driver (RAM)                                             */
/* ------------------------------------------------------------------ */

static const unsigned char *sfx_pos;   /* posición actual en DAT_* */
static unsigned char sfx_timer;        /* frames restantes del paso actual */
static unsigned char sfx_prio;         /* prioridad del sonido en curso */

/* ------------------------------------------------------------------ */
/* API                                                                 */
/* ------------------------------------------------------------------ */

void sound_init(void)
{
    WREG(APU_FC,    0x40);  /* 4-step mode, IRQ deshabilitado */
    WREG(APU_STS,   0x0D);  /* P1 + Triangle + Noise activos */
    WREG(P1_CTRL,   0x10);  /* silencio P1 */
    WREG(P1_SWEEP,  0x08);  /* sweep off */
    WREG(TRI_LINEAR,0x80);  /* silencio Triangle (linear counter=0) */
    WREG(NOI_CTRL,  0x10);  /* silencio Noise */
    sfx_pos    = 0;
    sfx_timer  = 0;
    sfx_prio   = 0;
    mus_active = 0;
    mus_paused = 0;
}

void sound_play(unsigned char id)
{
    unsigned char prio;
    const unsigned char *dat;

    switch (id) {
        case SFX_DOT:       prio = 1; dat = DAT_DOT;    break;
        case SFX_PELLET:    prio = 2; dat = DAT_PELLET; break;
        case SFX_GHOST_EAT: prio = 3; dat = DAT_GHOST;  break;
        case SFX_BOMB:      prio = 4; dat = DAT_BOMB;   break;
        case SFX_DEATH:     prio = 5; dat = DAT_DEATH;  break;
        default:            return;
    }

    if (prio < sfx_prio) return;   /* no interrumpir un sonido más urgente */

    sfx_pos   = dat;
    sfx_timer = 0;
    sfx_prio  = prio;
}

void sound_update(void)
{
    /* --- SFX en Pulse 1 --- */
    if (sfx_pos != 0) {
        if (sfx_timer > 0) {
            sfx_timer--;
        } else if (*sfx_pos == 0) {
            WREG(P1_CTRL, 0x10);
            sfx_pos  = 0;
            sfx_prio = 0;
        } else {
            sfx_timer = sfx_pos[0] - 1;
            WREG(P1_CTRL,  sfx_pos[1]);
            WREG(P1_SWEEP, 0x08);
            WREG(P1_LO,    sfx_pos[2]);
            WREG(P1_HI,    sfx_pos[3]);
            sfx_pos += 4;
        }
    }

    /* --- Música en Triangle + Noise --- */
    if (!mus_active || mus_paused) return;

    /* Triangle: melodía */
    if (mel_tmr == 0) {
        if (*mel_pos == N_END) mel_pos = MELODY;  /* loop */
        mel_tmr = mel_pos[0];
        if (mel_pos[1] == N_REST) {
            WREG(TRI_LINEAR, 0x80);   /* C=1, R=0 → linear counter=0 → silencio */
        } else {
            WREG(TRI_LINEAR, 0xFF);   /* C=1, R=127 → siempre activo */
            WREG(TRI_LO,     mel_pos[1]);
            WREG(TRI_HI,     0x08);   /* length index=1 → muy largo */
        }
        mel_pos += 2;
    }
    mel_tmr--;

    /* Noise: ritmo */
    if (rhy_tmr == 0) {
        if (*rhy_pos == N_END) rhy_pos = RHYTHM;  /* loop */
        rhy_tmr = rhy_pos[0];
        if (rhy_pos[1] == DR_SIL) {
            WREG(NOI_CTRL, 0x10);    /* silencio noise */
        } else {
            WREG(NOI_CTRL,   0x14);  /* vol=4, constant vol */
            WREG(NOI_PERIOD, rhy_pos[1]);
            WREG(NOI_LEN,    0x08);  /* length index=1 */
        }
        rhy_pos += 2;
    }
    rhy_tmr--;
}

void bgm_start(void)
{
    if (mus_active) return;
    mus_active = 1;
    mus_paused = 0;
    mel_pos    = MELODY;
    mel_tmr    = 0;
    rhy_pos    = RHYTHM;
    rhy_tmr    = 0;
}

void bgm_stop(void)
{
    mus_active = 0;
    mus_paused = 0;
    WREG(TRI_LINEAR, 0x80);
    WREG(NOI_CTRL,   0x10);
}

void bgm_pause(unsigned char paused)
{
    mus_paused = paused;
    if (paused) {
        WREG(TRI_LINEAR, 0x80);
        WREG(NOI_CTRL,   0x10);
    }
}
