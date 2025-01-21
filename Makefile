CC			= xc8-cc							# compiler type
MCU			= 16F877A							# chip type
CFLAGS		= -mcpu=$(MCU) -S
HEXFLAGS	= -mcpu=$(MCU)
PROG		= pk2cmd							# programmer type
PROGFLAGS	= -M -PPIC$(MCU) -Fmain.hex -Y

# Files
SRC = slave_left_right.c master_up_down.c
ASM_DIR = asm

# Generate ASM and HEX file lists from SRC
ASM = $(patsubst %.c,$(ASM_DIR)/%.asm,$(SRC))
HEX = $(patsubst %.c,$(ASM_DIR)/%.hex,$(SRC))

all: $(HEX)

$(ASM_DIR):
	mkdir -p $(ASM_DIR)

# Compile .c to .asm
$(ASM_DIR)/%.asm: src/%.c | $(ASM_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Compile .c to .hex
$(ASM_DIR)/%.hex: src/%.c | $(ASM_DIR)
	$(CC) $(HEXFLAGS) $< -o $@

flash: $(HEX)
	$(PROG) $(PROGFLAGS)

clean:
	rm -f $(ASM) $(HEX) *.p1 *.lst *.obj

.PHONY: all flash clean
