GAME    := pacman_nes
BUILD   := build

CC65    := cc65
CA65    := ca65
LD65    := ld65

# Directorios de includes para que cualquier .c/.h se referencie por nombre
# sin necesidad de rutas relativas (ej. #include "map.h", #include "neslib.h")
INC     := -I lib -I src/core -I src/world -I src/graphics -I src/entities

CFLAGS  := -t nes -O --add-source $(INC)
ASFLAGS := -t nes -I lib

# ---- Fuentes C --------------------------------------------------------
C_SRCS  := src/core/main.c \
            src/world/map.c \
            src/world/score.c \
            src/entities/player.c \
            src/entities/ghost.c \
            src/entities/obstacles.c

# ---- Fuentes ASM -------------------------------------------------------
S_SRCS  := src/core/crt0.s \
            src/graphics/tiles.s \
            lib/neslib.s \
            lib/famitone2.s

# ---- Objetos (todos en build/) ----------------------------------------
C_OBJS  := $(patsubst %.c,$(BUILD)/%.o,$(notdir $(C_SRCS)))
S_OBJS  := $(patsubst %.s,$(BUILD)/%.o,$(notdir $(S_SRCS)))
ALL_OBJ := $(S_OBJS) $(C_OBJS)

# Librería C de cc65 para NES (popa, popax, runtime…)
# Ajusta CC65_HOME a tu instalación (ej. C:/cc65 en Windows con instalador)
CC65_HOME ?= /usr/share/cc65
NES_LIB   := $(CC65_HOME)/lib/nes.lib

.PHONY: all clean run

all: $(BUILD)/$(GAME).nes

$(BUILD)/$(GAME).nes: $(ALL_OBJ) nes.cfg
	$(LD65) -C nes.cfg -o $@ $(ALL_OBJ) $(NES_LIB)

# Compilar .c → .s → .o  (src/core/ y src/world/)
$(BUILD)/%.o: src/core/%.c | $(BUILD)
	$(CC65) $(CFLAGS) -o $(BUILD)/$*.s $<
	$(CA65) $(ASFLAGS) -o $@ $(BUILD)/$*.s

$(BUILD)/%.o: src/world/%.c | $(BUILD)
	$(CC65) $(CFLAGS) -o $(BUILD)/$*.s $<
	$(CA65) $(ASFLAGS) -o $@ $(BUILD)/$*.s

$(BUILD)/%.o: src/entities/%.c | $(BUILD)
	$(CC65) $(CFLAGS) -o $(BUILD)/$*.s $<
	$(CA65) $(ASFLAGS) -o $@ $(BUILD)/$*.s

# Ensamblar .s → .o
$(BUILD)/%.o: src/core/%.s | $(BUILD)
	$(CA65) $(ASFLAGS) -o $@ $<

$(BUILD)/%.o: src/graphics/%.s | $(BUILD)
	$(CA65) $(ASFLAGS) -o $@ $<

$(BUILD)/%.o: lib/%.s | $(BUILD)
	$(CA65) $(ASFLAGS) -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -f $(BUILD)/*.o $(BUILD)/*.s $(BUILD)/*.nes

run: $(BUILD)/$(GAME).nes
	fceux $<
