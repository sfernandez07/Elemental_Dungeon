#ifndef NES_H
#define NES_H

#include <stdint.h>

/* =========================================================================
   Registros hardware de la NES
   (útiles para acceso directo desde C en casos excepcionales;
    en el juego normal, usar siempre las funciones de neslib.h)
   ========================================================================= */

/* PPU */
#define PPU_CTRL    (*(volatile uint8_t*)0x2000)
#define PPU_MASK    (*(volatile uint8_t*)0x2001)
#define PPU_STATUS  (*(volatile uint8_t*)0x2002)
#define OAM_ADDR    (*(volatile uint8_t*)0x2003)
#define OAM_DATA    (*(volatile uint8_t*)0x2004)
#define PPU_SCROLL  (*(volatile uint8_t*)0x2005)
#define PPU_ADDR    (*(volatile uint8_t*)0x2006)
#define PPU_DATA    (*(volatile uint8_t*)0x2007)
#define OAM_DMA     (*(volatile uint8_t*)0x4014)

/* Bits de PPU_CTRL (para crt0.s y acceso excepcional) */
#define PPUCTRL_NMI_ENABLE  0x80
#define PPUCTRL_SPR_SIZE    0x20
#define PPUCTRL_BG_TABLE    0x10
#define PPUCTRL_SPR_TABLE   0x08
#define PPUCTRL_VRAM_INC    0x04

/* Bits de PPU_STATUS */
#define PPUSTAT_VBLANK      0x80
#define PPUSTAT_SPR0_HIT    0x40

/* APU */
#define APU_PULSE1_VOL   (*(volatile uint8_t*)0x4000)
#define APU_PULSE1_SWEEP (*(volatile uint8_t*)0x4001)
#define APU_PULSE1_LO    (*(volatile uint8_t*)0x4002)
#define APU_PULSE1_HI    (*(volatile uint8_t*)0x4003)
#define APU_PULSE2_VOL   (*(volatile uint8_t*)0x4004)
#define APU_PULSE2_SWEEP (*(volatile uint8_t*)0x4005)
#define APU_PULSE2_LO    (*(volatile uint8_t*)0x4006)
#define APU_PULSE2_HI    (*(volatile uint8_t*)0x4007)
#define APU_TRI_LINEAR   (*(volatile uint8_t*)0x4008)
#define APU_TRI_LO       (*(volatile uint8_t*)0x400A)
#define APU_TRI_HI       (*(volatile uint8_t*)0x400B)
#define APU_NOISE_VOL    (*(volatile uint8_t*)0x400C)
#define APU_NOISE_LO     (*(volatile uint8_t*)0x400E)
#define APU_NOISE_HI     (*(volatile uint8_t*)0x400F)
#define APU_STATUS_REG   (*(volatile uint8_t*)0x4015)
#define APU_FRAME_CTR    (*(volatile uint8_t*)0x4017)

#endif /* NES_H */
