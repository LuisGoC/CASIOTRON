SRCS = main.c app_ints.c app_msps.c startup_stm32f070xb.c system_stm32f0xx.c \
       stm32f0xx_hal.c stm32f0xx_hal_cortex.c stm32f0xx_hal_rcc.c stm32f0xx_hal_flash.c \
	   stm32f0xx_hal_gpio.c stm32f0xx_hal_uart.c stm32f0xx_hal_dma.c stm32f0xx_hal_rtc.c \
	   stm32f0xx_hal_rcc_ex.c app_serial.c app_clock.c stm32f0xx_hal_wwdg.c stm32f0xx_hal_spi.c \
	   LCD.c queue.c stm32f0xx_hal_tim.c stm32f0xx_hal_tim_ex.c stm32f0xx_hal_i2c.c stm32f0xx_hal_i2c_ex.c \
	   temp.c 

#global symbols
SYMBOLS = -DSTM32F070xB -DUSE_HAL_DRIVER

#files .c and .s locations
SRC_DIR  = app/Src
SRC_DIR += cmsisf0/startups
SRC_DIR += half0/Src

#files .h locations
INC_DIR  = app/Inc
INC_DIR += cmsisf0/core
INC_DIR += cmsisf0/registers
INC_DIR += half0/Inc

OBJ_DIR = Build/obj

CC= arm-none-eabi-gcc
CS= arm-none-eabi-as
MACH= cortex-m0
CFLAGS= -c -mcpu=$(MACH) -mthumb -mfloat-abi=soft -std=gnu99 -Wall -O0 -g3 \
        -ffunction-sections -fdata-sections -MMD -MP

LDFLAGS= -mcpu=$(MACH) -mthumb -mfloat-abi=soft -Wl,--gc-sections --specs=rdimon.specs \
         --specs=nano.specs -Wl,-Map=Build/final.map -T linker.ld

OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:%.o=%.d)
VPATH = $(SRC_DIR)
INCLS = $(addprefix -I ,$(INC_DIR))


all: build Build/final.elf

Build/final.elf: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLS) $(SYMBOLS) $< -o $@

$(OBJ_DIR)/%.o: %.s
	$(CS) -c -mcpu=$(MACH) -mthumb -mfloat-abi=soft $< -o $@

.PHONY: build clean misra_test open debug flash
build:
	mkdir -p $(OBJ_DIR)

-include $(DEPS)

clean:
	rm -rf Build/

misra_test: 
	cppcheck --addon=misra.json --inline-suppr --quiet --std=c99 --template=gcc --force app/Src -I app/Inc

open:
	openocd -f board/st_nucleo_f0.cfg 

debug:
	arm-none-eabi-gdb final.elf -x=commands.gdb

flash:
	openocd -f interface/stlink-v2-1.cfg -f target/stm32f0x.cfg -c "program Build/$(TARGET).hex verify reset" -c shutdown

