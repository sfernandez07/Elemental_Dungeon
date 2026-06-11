#ifndef TEXT_H
#define TEXT_H

/* Base de los tiles de letras en CHR tabla 0.
   'A' = TEXT_LETTER_BASE + 0, 'B' = +1, ..., 'Z' = +25
   Digits: DIGIT_BASE (0x04) + ch - '0'   (definido en score.h)
   '!'  = TEXT_BANG_TILE
   '-'  = TEXT_DASH_TILE
   ' '  = 0x00 (TILE_EMPTY) */
#define TEXT_LETTER_BASE  0x2C
#define TEXT_BANG_TILE    0x46
#define TEXT_DASH_TILE    0x47

/* Escribe str en el nametable a partir de (x, y).
   Solo mayúsculas A-Z, dígitos 0-9, espacio, '!' y '-'.
   Llamar con PPU deshabilitada o dentro de un buffer VRAM. */
void draw_text(unsigned char x, unsigned char y, const char *str);

#endif
