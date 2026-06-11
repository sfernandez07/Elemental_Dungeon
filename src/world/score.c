#include "neslib.h"
#include "map.h"
#include "score.h"

unsigned int  score;
unsigned char score_dirty;

void score_init(void) {
    score       = 0;
    score_dirty = 0;   /* main() escribe los tiles directamente durante el init */
}

void score_add(unsigned int pts) {
    score += pts;
    score_dirty = 1;
}

/* Construye una actualización VRAM horizontal de SCORE_DIGITS tiles para el HUD.
   Formato neslib: MSB|NT_UPD_HORZ, LSB, LEN, [tiles].
   Devuelve bytes escritos; 0 si el score no ha cambiado. */
unsigned char score_build_update(unsigned char *buf) {
    unsigned int  addr;
    unsigned int  tmp;
    unsigned char digits[SCORE_DIGITS];
    unsigned char i, n;

    if (!score_dirty) return 0;

    /* Extraer SCORE_DIGITS dígitos decimales, el más significativo primero */
    tmp = score;
    for (i = SCORE_DIGITS; i > 0; i--) {
        digits[i - 1] = (unsigned char)(tmp % 10) + DIGIT_BASE;
        tmp /= 10;
    }

    addr    = NTADR_A(SCORE_COL, SCORE_ROW);
    n       = 0;
    buf[n++] = (unsigned char)((addr >> 8) | NT_UPD_HORZ);
    buf[n++] = (unsigned char)(addr & 0xFF);
    buf[n++] = SCORE_DIGITS;
    for (i = 0; i < SCORE_DIGITS; i++)
        buf[n++] = digits[i];

    score_dirty = 0;
    return n;
}

unsigned char score_lives_update(unsigned char *buf, unsigned char n,
                                 unsigned char lives_count,
                                 unsigned char *dirty)
{
    unsigned int addr;
    unsigned char i;
    if (!*dirty) return n;
    *dirty    = 0;
    addr      = NTADR_A(LIVES_HUD_COL, LIVES_HUD_ROW);
    buf[n++]  = (unsigned char)(addr >> 8) | NT_UPD_HORZ;
    buf[n++]  = (unsigned char)(addr & 0xFF);
    buf[n++]  = LIVES_HUD_SLOTS;
    for (i = 1; i <= LIVES_HUD_SLOTS; i++)
        buf[n++] = (lives_count >= i) ? TILE_PELLET : TILE_EMPTY;
    return n;
}
