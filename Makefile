# Toolchain settings
CC      = xc8-cc
MCU     = 16F877A
CFLAGS  = -mcpu=$(MCU) -S  # -S outputs assembler
HEXFLAGS = -mcpu=$(MCU)
PROG    = pk2cmd             # Change if using another programmer
PROGFLAGS = -M -PPIC$(MCU) -Fmain.hex -Y

# Files
SRC = master_up_down.c
ASM = $(SRC:.c=.asm)
HEX = $(SRC:.c=.hex)

all: $(HEX)

$(ASM): src/$(SRC)
	$(CC) $(CFLAGS) $<

$(HEX): src/$(SRC)
	$(CC) $(HEXFLAGS) $< -o $@

flash: $(HEX)
	$(PROG) $(PROGFLAGS)

clean:
	rm -f $(ASM) $(HEX) *.p1 *.lst *.obj

.PHONY: all flash clean
