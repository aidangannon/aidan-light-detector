CC			= xc8-cc							# compiler type
MCU			= 16F877A							# chip type
CFLAGS		= -mcpu=$(MCU) -S
HEXFLAGS	= -mcpu=$(MCU)
PROG		= pk2cmd							# programmer type
PROGFLAGS	= -M -PPIC$(MCU) -Fmain.hex -Y

# Files
SRC = master_up_down.c
ASM_DIR = asm
ASM = $(ASM_DIR)/$(SRC:.c=.asm)
HEX = $(ASM_DIR)/$(SRC:.c=.hex)

all: $(HEX)

$(ASM_DIR):
	mkdir -p $(ASM_DIR)

$(ASM): src/$(SRC) | $(ASM_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(HEX): src/$(SRC) | $(ASM_DIR)
	$(CC) $(HEXFLAGS) $< -o $@

flash: $(HEX)
	$(PROG) $(PROGFLAGS)

clean:
	rm -f $(ASM) $(HEX) *.p1 *.lst *.obj

.PHONY: all flash clean
