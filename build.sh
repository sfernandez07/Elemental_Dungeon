#!/usr/bin/env bash
# Build script for Linux CI (GitHub Actions).
# Equivalent to build.bat — requires cc65 installed via apt.
set -e

GAME=elemental_dungeon
BUILD=build
INC="-I lib -I src/core -I src/world -I src/graphics -I src/entities"
CFLAGS="-t nes -O --add-source $INC"
ASFLAGS="-t nes -I lib"

# Locate nes.lib (path differs between distros/cc65 versions)
NES_LIB=$(find /usr/share/cc65 /usr/lib/cc65 2>/dev/null -name "nes.lib" | head -1)
[ -z "$NES_LIB" ] && NES_LIB="nes.lib"

mkdir -p "$BUILD"

compile_c() {
    cc65 $CFLAGS -o "$BUILD/$2.s" "$1"
    ca65 $ASFLAGS -o "$BUILD/$2.o" "$BUILD/$2.s"
}

assemble_s() {
    ca65 $ASFLAGS -o "$BUILD/$2.o" "$1"
}

echo "Compilando fuentes C..."
compile_c src/core/main.c          main
compile_c src/core/game.c          game
compile_c src/core/text.c          text
compile_c src/core/sound.c         sound
compile_c src/world/map.c          map
compile_c src/world/score.c        score
compile_c src/entities/player.c    player
compile_c src/entities/ghost.c     ghost
compile_c src/entities/obstacles.c obstacles

echo "Ensamblando fuentes ASM..."
assemble_s src/core/crt0.s      crt0
assemble_s src/graphics/tiles.s tiles
assemble_s lib/neslib.s         neslib

echo "Enlazando..."
ld65 -C nes.cfg -o "$BUILD/$GAME.nes" \
    "$BUILD/crt0.o"      \
    "$BUILD/tiles.o"     \
    "$BUILD/neslib.o"    \
    "$BUILD/main.o"      \
    "$BUILD/game.o"      \
    "$BUILD/text.o"      \
    "$BUILD/sound.o"     \
    "$BUILD/map.o"       \
    "$BUILD/score.o"     \
    "$BUILD/player.o"    \
    "$BUILD/ghost.o"     \
    "$BUILD/obstacles.o" \
    "$NES_LIB"

rm -f "$BUILD"/*.s "$BUILD"/*.o
echo ""
echo "OK: $BUILD/$GAME.nes"
