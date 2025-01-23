CC      = /opt/microchip/xc8/v3.00/bin/xc8-cc
MCU     = 16F877A
CFLAGS  = -mcpu=$(MCU) -S
HEXFLAGS= -mcpu=$(MCU)
PROG    = pk2cmd
PROGFLAGS = -M -PPIC$(MCU) -Fmain.hex -Y

SRC     := $(wildcard src/*.c)
HDR     := $(wildcard src/*.h)

ASM_DIR = asm

ASM = $(patsubst src/%.c,$(ASM_DIR)/%.asm,$(SRC))
HEX = $(patsubst src/%.c,$(ASM_DIR)/%.hex,$(SRC))

all: $(HEX)

$(ASM_DIR):
	mkdir -p $(ASM_DIR)

$(ASM_DIR)/%.asm: src/%.c | $(ASM_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(ASM_DIR)/%.hex: src/%.c | $(ASM_DIR)
	$(CC) $(HEXFLAGS) $< -o $@

flash: $(HEX)
	$(PROG) $(PROGFLAGS)

clean:
	rm -f $(ASM) $(HEX) *.p1 *.lst *.obj

.PHONY: all flash clean
