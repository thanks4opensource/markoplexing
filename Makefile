all: charlieplex.elf markoplex.elf

SWITCHABLE_KEYPAD ?= D
USE_DISPLAY	  ?= D

ifeq ($(SWITCHABLE_KEYPAD), D) 
$(info SWITCHABLE_KEYPAD is enabled)
endif

ifeq ($(USE_DISPLAY), D) 
$(info USE_DISPLAY is enabled)
DISPLAY_OBJ := display.o
endif

CHARL_OBJS = main.o charlieplex.o $(DISPLAY_OBJ)
MARKO_OBJS = main.o markoplex.o   $(DISPLAY_OBJ)


DEBUG_FLAG    ?= -g3
OPTIMIZE_FLAG ?= -O0



.PHONY: clean
clean:
	rm -f *.o *.elf *.elf.map

ifndef GCC_ARM_ROOT
$(error set GCC_ARM_ROOT environment variable)
endif

export PATH := $(GCC_ARM_ROOT)/bin:$(PATH)

CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld


CONFIGURATION_FLAGS = -$(SWITCHABLE_KEYPAD)SWITCHABLE_KEYPAD \
		      -$(USE_DISPLAY)USE_DISPLAY

WARNINGS_FLAG = -Wextra -Wswitch --warn-no-return-type -Werror=return-type
CC_ARM_FLAGS  = -mthumb \
		-mcpu=cortex-m0plus \
		-D__VTOR_PRESENT \
		-mfloat-abi=softfp -D__SOFTFP__
INIT_DEFINES  = -DWEAK_FUNCTIONS

GNU_ARM_LIBDIRS  = -L$(GCC_ARM_ROOT)/arm-none-eabi/lib/thumb/v7e-m \
		   -L$(GCC_ARM_ROOT)/lib/gcc/arm-none-eabi/6.3.1/thumb/v7e-m/fpv4-sp/softfp



EXTRA_CC_FLAGS = -DLPC82X

INIT_FILE = init/lpc8xx_ram_init
LD_SCRIPT = ld/lpc824_ram.ld

CCOPTS     = -I./source			\
	     -I./include		\
	     -I./include/arm		\
	     -I./include/nxp		\
	     $(WARNINGS_FLAG)		\
	     $(DEBUG_FLAG)		\
	     $(CC_ARM_FLAGS)		\
	     $(INIT_DEFINES)		\
	     $(OPTIMIZE_FLAG)		\
	     $(EXTRA_CC_FLAGS)		\
	     $(CONFIGURATION_FLAGS)

LD_ARM_FLAGS = -marmelf
LINK_FLAGS   = -static -M --print-memory-usage
LDOPTS       = $(LD_ARM_FLAGS) $(LINK_FLAGS)


charlieplex.elf: $(CHARL_OBJS) $(INIT_FILE).o
	$(LD) -o $@ 			\
	       $(LDOPTS)		\
	       $(CHARL_OBJS)		\
	       $(GNU_ARM_LIBDIRS)	\
	       $(INIT_FILE).o		\
	       -T $(LD_SCRIPT)		\
	 >     $@.map
	tail -n 3 $@.map

markoplex.elf: $(MARKO_OBJS) $(INIT_FILE).o
	$(LD) -o $@ 			\
	       $(LDOPTS)		\
	       $(MARKO_OBJS)		\
	       $(GNU_ARM_LIBDIRS)	\
	       $(INIT_FILE).o		\
	       -T $(LD_SCRIPT)		\
	 >     $@.map
	tail -n 3 $@.map


vpath %.c   source


%.o: %.c
	$(CC) -c $(CCOPTS) $< -o $@

charlieplex.o: source/charlieplex.c source/lpc824_bits.h
display.o:     source/display.c     source/lpc824_bits.h
main.o:        source/main.c	    source/lpc824_bits.h
markoplex.o:   source/markoplex.c   source/lpc824_bits.h
