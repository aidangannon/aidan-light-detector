CC      	= /opt/microchip/xc8/v3.00/bin/xc8-cc
MCU     	= 16F877A
CFLAGS		= -mcpu=$(MCU) -c -D__XC8__
ASMFLAGS	= -mcpu=$(MCU) -S -D__XC8__
HEXFLAGS	= -mcpu=$(MCU) -D__XC8__
PROG    	= pk2cmd
PROGFLAGS	= -M -PPIC$(MCU) -F -Y

$(call check_required, PROG_NAME)
$(call check_required, PROG_DEFINES)

SRC_DIR = src

HAL_DIR = $(SRC_DIR)/hal
HAL_SOURCES := $(wildcard $(HAL_DIR)/*.c)
HAL_HEADERS := $(wildcard $(HAL_DIR)/*.h)

ifeq ($(MCU),16F877A)
	HAL_FILTERED = $(filter %_pic16f877a.c,$(HAL_SOURCES))
else
	HAL_FILTERED = $(filter %_sim.c,$(HAL_SOURCES))
endif

ENTRY_FILE = entry.c
BUILD_DIR = build
HDR := $(wildcard $(SRC_DIR)/*.h) $(HAL_HEADERS)

PROG_DIR = $(BUILD_DIR)/$(PROG_NAME)

HAL_OBJS = $(patsubst $(HAL_DIR)/%.c,$(PROG_DIR)/hal_%.p1,$(HAL_FILTERED))

PROG_OBJ = $(PROG_DIR)/$(ENTRY_FILE:.c=.p1)
PROG_HEX = $(PROG_DIR)/$(ENTRY_FILE:.c=.hex)
PROG_ASM = $(PROG_DIR)/$(ENTRY_FILE:.c=.s)

all: $(PROG_HEX) $(PROG_ASM)

$(PROG_DIR):
	mkdir -p $@

# compile but dont link
$(PROG_OBJ): $(SRC_DIR)/$(ENTRY_FILE) $(HDR) | $(PROG_DIR)
	$(CC) $(CFLAGS) $(PROG_DEFINES) -I$(HAL_DIR) $< -o $@

#compile the hal
$(PROG_DIR)/hal_%.p1: $(HAL_DIR)/%.c $(HAL_HEADERS) | $(PROG_DIR)
	$(CC) $(CFLAGS) $(PROG_DEFINES) -I$(HAL_DIR) $< -o $@

# link objects
$(PROG_HEX): $(PROG_OBJ) $(HAL_OBJS)
	$(CC) $(HEXFLAGS) $(PROG_DEFINES) $^ -o $@

# compile to human readable assembly for debugging analysis
$(PROG_ASM): $(PROG_HEX)
	$(CC) $(ASMFLAGS) $(PROG_DEFINES) $(PROG_OBJ) $(HAL_OBJS) -o $@

# flash the hex files to pic ROM
flash: $(PROG_HEX)
	$(PROG) $(PROGFLAGS)$(PROG_HEX)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all flash-master flash-slave clean