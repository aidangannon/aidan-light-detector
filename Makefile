CC      	= /opt/microchip/xc8/v3.00/bin/xc8-cc
MCU     	= 16F877A
CFLAGS		= -mcpu=$(MCU) -c -D__XC8__
ASMFLAGS	= -mcpu=$(MCU) -S -D__XC8__
HEXFLAGS	= -mcpu=$(MCU) -D__XC8__
PROG    	= pk2cmd
PROGFLAGS	= -M -PPIC$(MCU) -F -Y

$(call check_required, PROG_NAME)
$(call check_required, PROG_DEFINES)

ENTRY_FILE = entry.c
BUILD_DIR = build
SRC_DIR = src
HDR := $(wildcard $(SRC_DIR)/*.h)

PROG_DIR = $(BUILD_DIR)/$(PROG_NAME)

PROG_OBJ = $(PROG_DIR)/$(ENTRY_FILE:.c=.p1)
PROG_HEX = $(PROG_DIR)/$(ENTRY_FILE:.c=.hex)
PROG_ASM = $(PROG_DIR)/$(ENTRY_FILE:.c=.s)

all: $(PROG_HEX) $(PROG_HEX) $(PROG_ASM) $(PROG_ASM)

$(PROG_DIR) $(PROG_DIR):
	mkdir -p $@

$(PROG_OBJ): $(SRC_DIR)/$(ENTRY_FILE) $(HDR) | $(PROG_DIR)
	$(CC) $(CFLAGS) $(PROG_DEFINES) $< -o $@

$(PROG_HEX): $(PROG_OBJ)
	$(CC) $(HEXFLAGS) $(PROG_DEFINES) $^ -o $@

$(PROG_ASM): $(SRC_DIR)/$(ENTRY_FILE) $(HDR) | $(PROG_DIR)
	$(CC) $(ASMFLAGS) $(PROG_DEFINES) $< -o $@

flash: $(PROG_HEX)
	$(PROG) $(PROGFLAGS)$(PROG_HEX)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all flash-master flash-slave clean