#include "neslib.h"
#include "score.h"
#include "text.h"

static unsigned char char_to_tile(char c)
{
    if (c >= 'A' && c <= 'Z') return (unsigned char)(TEXT_LETTER_BASE + (c - 'A'));
    if (c >= '0' && c <= '9') return (unsigned char)(DIGIT_BASE        + (c - '0'));
    if (c == '!')              return TEXT_BANG_TILE;
    if (c == '-')              return TEXT_DASH_TILE;
    return 0x00;   /* espacio u otro carácter → tile vacío */
}

void draw_text(unsigned char x, unsigned char y, const char *str)
{
    vram_adr(NTADR_A(x, y));
    while (*str) {
        vram_put(char_to_tile(*str));
        str++;
    }
}
