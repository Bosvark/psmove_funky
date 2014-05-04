# Makefile - build script */
APPNAME=psmove

INCDIR=./include
SRCDIR=./src
LIBDIR=./libs
OUTDIR=./out
OBJDIR=./obj
LDDIR=./ldscripts
 
# build environment
PREFIX ?= 
ARMGNU ?= $(PREFIX)arm-none-eabi
 
# source files
SOURCES_C   += $(LIBDIR)/CMSIS/src/startup_cm.c
SOURCES_C   += $(LIBDIR)/CMSIS/src/startup_stm32f10x.c
SOURCES_C   += $(LIBDIR)/CMSIS/src/system_stm32f10x.c
SOURCES_C   += $(LIBDIR)/CMSIS/src/vectors_stm32f10x.c
SOURCES_C   += $(LIBDIR)/CMSIS/src/core_cm3.c
SOURCES_C   += $(LIBDIR)/StdPeriph/src/stm32f10x_gpio.c
SOURCES_C   += $(LIBDIR)/StdPeriph/src/stm32f10x_tim.c
SOURCES_C   += $(LIBDIR)/StdPeriph/src/stm32f10x_rcc.c
SOURCES_C   += $(SRCDIR)/main.c
SOURCES_C   += $(SRCDIR)/timer.c

SOURCES_LD  := $(wildcard $(LDDIR)/*.ld)
 
# object files
LDS	:= $(patsubst %.ld,-T%.ld,$(SOURCES_LD))
OBJS	:= $(patsubst %.c,%.o,$(SOURCES_C))
    	     
# Build flags
DEPENDFLAGS := -MD -MP

INCLUDES    += -I$(INCDIR)
INCLUDES    += -I$(LIBDIR)/CMSIS/include
INCLUDES    += -I$(LIBDIR)/misc/include
INCLUDES    += -I$(LIBDIR)/StdPeriph/include

BASEFLAGS = -mcpu=cortex-m3 -mthumb -mthumb-interwork -mfloat-abi=soft -O0 -Wall  -g3 

COMPFLAGS += -DUSE_STDPERIPH_DRIVER
COMPFLAGS += -DSTM32F10X_MD
COMPFLAGS += -DHSE_VALUE=8000000
COMPFLAGS += -DDEBUG

LDFLAGS     +=  -nostartfiles -nostdlib $(LDS) -Xlinker --gc-sections -L"$(LDDIR)"

ifeq ($(FLOAT_TYPE), hard)
BASEFLAGS += -fsingle-precision-constant -Wdouble-promotion
BASEFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
else
BASEFLAGS += -mfloat-abi=soft
endif

WARNFLAGS   := -Wall -O0 -Wall -g3

ASFLAGS     := $(INCLUDES) $(DEPENDFLAGS) -D__ASSEMBLY__
CFLAGS      := $(COMPFLAGS) $(INCLUDES) $(DEPENDFLAGS) $(BASEFLAGS) $(WARNFLAGS)
CFLAGS      += -std=c11

# build rules
all: $(APPNAME).bin post-build
 
include $(wildcard *.d)
 
$(APPNAME).elf: $(OBJS) $(SOURCES_LD)
	$(ARMGNU)-gcc $(BASEFLAGS) $(LDFLAGS) $(OBJS) -o $@

$(APPNAME).bin: $(APPNAME).elf
	$(ARMGNU)-objcopy -O binary $(APPNAME).elf $(APPNAME).bin

$(APPNAME).hex: $(APPNAME).elf
	$(ARMGNU)-objcopy -O ihex $(APPNAME).elf $(APPNAME).hex

post-build: $(APPNAME).bin $(APPNAME).hex
	$(ARMGNU)-size --format=berkeley $(APPNAME).elf

clean:
	$(RM) -f $(OBJS) $(APPNAME).elf $(APPNAME).bin  $(APPNAME).hex
	$(RM) -f $(patsubst %.c,%.d,$(SOURCES_C))

# C.
%.o: %.c Makefile
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@
 
# AS.
%.o: %.s Makefile
	$(ARMGNU)-gcc $(ASFLAGS) -c $< -o $@
	
%.o: $(ICONS_DIR)/%.bin
	$(ARMGNU)-objcopy -I binary -O elf32-littlearm -B arm $< $@

