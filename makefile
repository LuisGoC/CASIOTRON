#Nombre del proyecto
TARGET = temp
#Archivos a compilar
SRCS  = main.c app_ints.c app_msps.c startup_stm32f070xb.c system_stm32f0xx.c 
SRCS += stm32f0xx_hal.c stm32f0xx_hal_cortex.c stm32f0xx_hal_rcc.c stm32f0xx_hal_flash.c
SRCS += stm32f0xx_hal_gpio.c stm32f0xx_hal_uart.c stm32f0xx_hal_dma.c stm32f0xx_hal_rtc.c
SRCS += stm32f0xx_hal_rcc_ex.c app_serial.c app_clock.c stm32f0xx_hal_wwdg.c stm32f0xx_hal_spi.c
SRCS += LCD.c queue.c stm32f0xx_hal_tim.c stm32f0xx_hal_tim_ex.c stm32f0xx_hal_i2c.c stm32f0xx_hal_i2c_ex.c
SRCS += temp.c
#archivo linker a usar
LINKER = linker.ld
#Simbolos gloobales del programa (#defines globales)
SYMBOLS = -DSTM32F070xB -DUSE_HAL_DRIVER
#directorios con archivos a compilar (.c y .s)
SRC_PATHS  = app/Src
SRC_PATHS += cmsisf0/startups
SRC_PATHS += half0/Src
#direcotrios con archivos .h
INC_PATHS  = app/Inc
INC_PATHS += cmsisf0/core
INC_PATHS += cmsisf0/registers
INC_PATHS += half0/Inc

#compilador y opciones de compilacion
TOOLCHAIN = arm-none-eabi
CPU = -mcpu=cortex-m0 -mthumb -mfloat-abi=soft
CFLAGS  = $(CPU) -Wall -g3 -O0 -std=c99
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -MMD -MP
AFLAGS = $(CPU)
LFLAGS = $(CPU) -Wl,--gc-sections --specs=rdimon.specs --specs=nano.specs -Wl,-Map=Build/$(TARGET).map

#substituccion de prefijos y postfijos 
OBJS = $(SRCS:%.c=Build/obj/%.o)
DEPS = $(OBJS:%.o=%.d)
VPATH = $(SRC_PATHS)
INCLS = $(addprefix -I ,$(INC_PATHS))

#Instrucciones de compilacion
all : build $(TARGET)

$(TARGET) : $(addprefix Build/, $(TARGET).elf)
	$(TOOLCHAIN)-objcopy -Oihex $< Build/$(TARGET).hex
	$(TOOLCHAIN)-objdump -S $< > Build/$(TARGET).lst
	$(TOOLCHAIN)-size --format=berkeley $<

Build/$(TARGET).elf : $(OBJS)
	$(TOOLCHAIN)-gcc $(LFLAGS) -T $(LINKER) -o $@ $^

Build/obj/%.o : %.c
	$(TOOLCHAIN)-gcc $(CFLAGS) $(INCLS) $(SYMBOLS) -o $@ -c $<

Build/obj/%.o : %.s
	$(TOOLCHAIN)-as $(AFLAGS) -o $@ -c $<

build :
	mkdir -p Build/obj

-include $(DEPS)

#borrar archivos generados
clean :
	rm -rf Build

#Programar al tarjeta
flash :
	openocd -f interface/stlink-v2-1.cfg -f target/stm32f0x.cfg -c "program Build/$(TARGET).hex verify reset" -c shutdown
#Conectar OpenOCD con al Tarjeta
open :
	openocd -f interface/stlink-v2-1.cfg -f target/stm32f0x.cfg -c "reset_config srst_only srst_nogate"
#Lanzar sesion de debug (es necesario primero Conectar la tarjeta con OpenOCD)
debug :
	$(TOOLCHAIN)-gdb Build/$(TARGET).elf -iex "set auto-load safe-path /"

misra_test: 
	cppcheck --addon=misra.json --inline-suppr --quiet --std=c99 --template=gcc --force app/Src -I app/Inc

