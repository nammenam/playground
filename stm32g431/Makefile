BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
RM = rm -rf

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
DB = $(PREFIX)gdb

TARGET = $(BIN_DIR)/program
LINKER_SCRIPT = STM32G431RBTX_FLASH.ld

CPU = cortex-m4
CPUFLAGS = -mcpu=$(CPU) -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
DEFINES = -DSTM32G431xx

# Source Files
SRC_DIR = src
STARTUP_DIR = startup
DRIVER_DIR = HALDrivers

C_SOURCES = $(shell find $(SRC_DIR) -type f -name "*.c")
C_SOURCES += $(shell find $(STARTUP_DIR) -type f -name "*.c")
C_SOURCES += $(shell find $(DRIVER_DIR) -type f -name "*.c")
ASM_SOURCES = $(shell find $(STARTUP_DIR) -type f -name "*.s")

# Object files — each source tree maps to its own subtree under OBJ_DIR
C_OBJECTS  = $(patsubst $(SRC_DIR)/%.c,    $(OBJ_DIR)/$(SRC_DIR)/%.o,    $(filter $(SRC_DIR)/%,    $(C_SOURCES)))
C_OBJECTS += $(patsubst $(STARTUP_DIR)/%.c, $(OBJ_DIR)/$(STARTUP_DIR)/%.o, $(filter $(STARTUP_DIR)/%, $(C_SOURCES)))
C_OBJECTS += $(patsubst $(DRIVER_DIR)/%.c,  $(OBJ_DIR)/$(DRIVER_DIR)/%.o,  $(filter $(DRIVER_DIR)/%,  $(C_SOURCES)))
ASM_OBJECTS = $(patsubst $(STARTUP_DIR)/%.s,$(OBJ_DIR)/$(STARTUP_DIR)/%.o,$(ASM_SOURCES))

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Flags
CFLAGS += $(CPUFLAGS) $(DEFINES)
CFLAGS += -W -Wall --std=gnu11 -Os
CFLAGS += -fno-diagnostics-show-caret
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -funsigned-char -funsigned-bitfields
CFLAGS += -flto
CFLAGS += -include stdint.h

ASFLAGS += $(CPUFLAGS) -x assembler-with-cpp

LDFLAGS += -mcpu=$(CPU) -T $(LINKER_SCRIPT) --specs=nosys.specs --specs=nano.specs -e Reset_Handler
LDFLAGS += -Wl,--gc-sections -mthumb
LDFLAGS += -flto

# Build Rules
$(OBJ_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/$(STARTUP_DIR)/%.o: $(STARTUP_DIR)/%.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

$(OBJ_DIR)/$(STARTUP_DIR)/%.o: $(STARTUP_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/$(DRIVER_DIR)/%.o: $(DRIVER_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $^ -o $@

$(TARGET).hex: $(TARGET).elf
	$(CP) -O ihex $< $@

.PHONY: all clean flash compile link clean-all
all: $(TARGET).elf
link: $(TARGET).elf
compile: $(OBJECTS)
flash: $(TARGET).elf
	openocd -f interface/stlink.cfg \
	        -f target/stm32g4x.cfg \
	        -c "program $< verify reset exit"
clean:
	$(RM) $(OBJ_DIR)/src
clean-all:
	$(RM) $(BUILD_DIR)
