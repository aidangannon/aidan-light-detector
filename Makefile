CC      	= /opt/microchip/xc8/v3.00/bin/xc8-cc
MCU     	= 16F877A
CFLAGS		= -mcpu=$(MCU) -S  -D__XC8__
HEXFLAGS	= -mcpu=$(MCU) -D__XC8__
PROG    	= pk2cmd
PROGFLAGS	= -M -PPIC$(MCU) -Fmain.hex -Y

SRC     := $(wildcard src/*.c)
HDR     := $(wildcard src/*.h)

ASM_DIR = asm

ASM = $(patsubst src/%.c,$(ASM_DIR)/%.s,$(SRC))
HEX = $(patsubst src/%.c,$(ASM_DIR)/%.hex,$(SRC))

all: $(HEX) $(ASM)

$(ASM_DIR):
	mkdir -p $(ASM_DIR)

$(ASM_DIR)/%.s: src/%.c | $(ASM_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(ASM_DIR)/%.hex: src/%.c | $(ASM_DIR)
	$(CC) $(HEXFLAGS) $< -o $@

flash: $(HEX)
	$(PROG) $(PROGFLAGS)

clean:
	rm -rf asm -r

.PHONY: all flash clean
