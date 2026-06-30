TARGET     = sensor_pipeline
BUILD_DIR  = build

# Set this to your FreeRTOS-Kernel checkout, or pass on command line:
#   make FREERTOS_DIR=/path/to/FreeRTOS-Kernel
FREERTOS_DIR ?= $(error Set FREERTOS_DIR to the FreeRTOS-Kernel source path)

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CPU     = -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
CFLAGS  = $(CPU) -Wall -Wextra -O2 -std=c11 -ffreestanding \
          -ffunction-sections -fdata-sections \
          -Iinc \
          -I$(FREERTOS_DIR)/include \
          -I$(FREERTOS_DIR)/portable/GCC/ARM_CM3

LDFLAGS = $(CPU) -Tlinker.ld -nostartfiles -Wl,--gc-sections \
          --specs=nano.specs --specs=nosys.specs \
          -Wl,-Map=$(BUILD_DIR)/$(TARGET).map

FREERTOS_SRCS = \
    $(FREERTOS_DIR)/tasks.c \
    $(FREERTOS_DIR)/queue.c \
    $(FREERTOS_DIR)/list.c \
    $(FREERTOS_DIR)/timers.c \
    $(FREERTOS_DIR)/portable/GCC/ARM_CM3/port.c \
    $(FREERTOS_DIR)/portable/MemMang/heap_4.c

APP_SRCS = \
    src/startup.c \
    src/syscalls.c \
    src/main.c \
    src/uart.c \
    src/sensor_task.c \
    src/processing_task.c \
    src/reporter_task.c

ALL_SRCS = $(APP_SRCS) $(FREERTOS_SRCS)

APP_OBJS      = $(patsubst src/%.c,       $(BUILD_DIR)/app/%.o,  $(APP_SRCS))
FREERTOS_OBJS = $(patsubst $(FREERTOS_DIR)/%.c, $(BUILD_DIR)/rtos/%.o, $(FREERTOS_SRCS))
ALL_OBJS      = $(APP_OBJS) $(FREERTOS_OBJS)

.PHONY: all clean flash

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin
	$(SIZE) $(BUILD_DIR)/$(TARGET).elf

$(BUILD_DIR)/app/%.o: src/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/rtos/%.o: $(FREERTOS_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(ALL_OBJS)
	$(CC) $(LDFLAGS) $(ALL_OBJS) -o $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x08000000

clean:
	rm -rf $(BUILD_DIR)
