##
## Makefile for Miosix np embedded OS
## TFT:Terraneo Federico Technlogies
## 
## This file contains the options required by the build system to build
## Miosix on various target architectures. All options start with OPT_
## to be easily recognizable.
## All architecture specific build code is grouped at the end of this file.
##

##
## Target board, choose one. This also implicitly select the target
## architecture
##
#OPT_BOARD := lpc2138_miosix_board
OPT_BOARD := stm32f103ze_stm3210e-eval

##
## Optimization flags, choose one.
## -O0 produces large and slow code, but is useful for in circuit debugging.
## -O2 is recomended otherwise, as it provides a good balance between code
## size and speed
##
#OPT_OPTIMIZATION := -O0
#OPT_OPTIMIZATION := -O2
#OPT_OPTIMIZATION := -O3
OPT_OPTIMIZATION := -Os

##
## C++ Exception/rtti support disable flags.
## To save code size if not using C++ exceptions (nor some STL code which
## implicitly uses it) uncomment this option.
## the -D__NO_EXCEPTIONS is used by Miosix to know if exceptions are used.
##
OPT_EXCEPT := -fno-exceptions -fno-rtti -D__NO_EXCEPTIONS

#############################################################################
## Board specific options
#############################################################################

##---------------------------------------------------------------------------
## lpc2138_miosix_board
##

# No options

##---------------------------------------------------------------------------
## stm32f103ze_stm3210e-eval
##
ifeq ($(OPT_BOARD),stm32f103ze_stm3210e-eval)

    ## Linker script type, there are three options
    ## 1) Code in FLASH, stack + heap in internal RAM (file *_rom.ld)
    ##    the most common choice, available for all microcontrollers
    ## 2) Code in FLASH stack in internal RAM heap in external RAM (file
    ##    *_xram.ld) useful for hardware like STM3210E-EVAL when big heap is
    ##    needed. The microcontroller must have an external memory interface.
    ## 3) Code + stack + heap in external RAM, (file *_all_in_xram.ld)
    ##    useful for debugging code in hardware like STM3210E-EVAL. Code runs
    ##    *very* slow compared to FLASH. Works only with a booloader that
    ##    forwards interrrupts @ 0x68000000 (see miosix/bootloaders for one).
    ##    The microcontroller must have an external memory interface.
    ## Warning! enable external ram if you use a linker script that requires it
    ## (see the XRAM flag below)
    LINKER_SCRIPT_PATH := arch/cortexM3_stm32/stm32f103ze_stm3210e-eval/
    LINKER_SCRIPT := $(LINKER_SCRIPT_PATH)stm32_512k+64k_rom.ld
    #LINKER_SCRIPT := $(LINKER_SCRIPT_PATH)stm32_512k+64k_xram.ld
    #LINKER_SCRIPT := $(LINKER_SCRIPT_PATH)stm32_512k+64k_all_in_xram.ld

    ## Enable/disable initialization of external RAM at boot. Three options:
    ## __ENABLE_XRAM : If you want the heap in xram (with an appropriate linker
    ## script selected above)
    ## __ENABLE_XRAM and __CODE_IN_XRAM : Debug mode with code + stack + heap
    ## in xram (with an appropriate linker script selected above)
    ## none selected : don't use xram (with an appropriate linker script
    ## selected above)
    #XRAM := -D__ENABLE_XRAM
    #XRAM := -D__ENABLE_XRAM -D__CODE_IN_XRAM

    ## Select clock frequency
    ## Not defining any of these results in the internal 8MHz clock to be used
    #CLOCK_FREQ := -DSYSCLK_FREQ_24MHz=24000000
    #CLOCK_FREQ := -DSYSCLK_FREQ_36MHz=36000000
    #CLOCK_FREQ := -DSYSCLK_FREQ_48MHz=48000000
    #CLOCK_FREQ := -DSYSCLK_FREQ_56MHz=56000000
    CLOCK_FREQ := -DSYSCLK_FREQ_72MHz=72000000

endif

############################################################################
## From the options selected above, now fill all the variables needed to  ##
## build Miosix. You should modify something here only if you are adding  ##
## a new board or porting Miosix to a new architecture                    ##
############################################################################

##
## First, auto guess architecture name from board name
##
ifeq ($(OPT_BOARD),lpc2138_miosix_board)
    ARCH := arm7_lpc2000
else ifeq ($(OPT_BOARD),stm32f103ze_stm3210e-eval)
    ARCH := cortexM3_stm32
else
    $(error Error: no board specified in miosix/config/Makefile.inc)
endif


##
## Then, initialize C/C++ flags with -D_MIOSIX so that application code can
## know if the OS is MIOSIX
##
CFLAGS_BASE   := -D_MIOSIX -D_POSIX_THREADS
CXXFLAGS_BASE := -D_MIOSIX -D_POSIX_THREADS

##
## Now two big switch-like constructs nested. The first lists all possible
## architectures, setting common things for all boards in the architecture.
## Then for each architecture there is a switch for evry board, where all
## board specific options are set.
##

##-----------------------------------------------------------------------------
## ARCHITECTURE: arm7_lpc2000
##
ifeq ($(ARCH),arm7_lpc2000)

    ##-------------------------------------------------------------------------
    ## BOARD: lpc2138_miosix_board
    ##
    ifeq ($(OPT_BOARD),lpc2138_miosix_board)

        ## Base directory with header files for this board
        BOARD_INC := arch/arm7_lpc2000/lpc2138_miosix_board

        ## Select linker script and boot file
        ## Their path must be relative to the miosix directory.
        BOOT_FILE := $(BOARD_INC)/core/stage_1_boot.o
        LINKER_SCRIPT := arch/arm7_lpc2000/lpc2138_miosix_board/miosix.ld

        ## Select architecture specific files
        ## These are the files in arch/<arch name>/<board name>
        ARCH_SRC :=                                  \
        $(BOARD_INC)/core/interrupts.cpp             \
        $(BOARD_INC)/drivers/serial.cpp              \
        $(BOARD_INC)/interfaces-impl/portability.cpp \
        $(BOARD_INC)/interfaces-impl/console.cpp     \
        $(BOARD_INC)/interfaces-impl/disk.cpp        \
        $(BOARD_INC)/interfaces-impl/delays.cpp      \
        $(BOARD_INC)/interfaces-impl/bsp.cpp

        ## Add a #define to allow querying board name
        CFLAGS_BASE   += -D_BOARD_MIOSIX_BOARD
        CXXFLAGS_BASE += -D_BOARD_MIOSIX_BOARD
        
    ##-------------------------------------------------------------------------
    ## End of board list
    ##
    endif

    ## Select compiler
    PREFIX=arm-miosix-eabi-

    ## From compiler prefix form the name of the compiler and other tools
    CC  := $(PREFIX)gcc
    CXX := $(PREFIX)g++
    LD  := $(PREFIX)ld
    AR  := $(PREFIX)ar
    AS  := $(PREFIX)as
    CP  := $(PREFIX)objcopy
    OD  := $(PREFIX)objdump
    SZ  := $(PREFIX)size

    ## Select appropriate compiler flags for both ASM/C/C++/linker
    AFLAGS_BASE   := -mcpu=arm7tdmi -mapcs-32 -mfloat-abi=soft
    CFLAGS_BASE   += -D_ARCH_ARM7_LPC2000 -mcpu=arm7tdmi $(OPT_OPTIMIZATION) \
                     -ffunction-sections -Wall -g -c
    CXXFLAGS_BASE += -D_ARCH_ARM7_LPC2000 -mcpu=arm7tdmi $(OPT_OPTIMIZATION) \
                     -ffunction-sections $(OPT_EXCEPT) -Wall -g -c
    LFLAGS_BASE   := -mcpu=arm7tdmi -Wl,--gc-sections,-Map,main.map          \
                     -Wl,-T./miosix/$(LINKER_SCRIPT) $(OPT_EXCEPT) -nostdlib

    ## Select architecture specific files
    ## These are the files in arch/<arch name>/common
    #ARCH_SRC += Nothing to add

    ## Base directory with header files for this board
    ARCH_INC := arch/arm7_lpc2000/common

    ## Select programmer command line
    ## This is the program that is invoked when the user types 'make program'
    ## The command must provide a way to program the board, or print an error
    ## message saying that 'make program' is not supported for that board.
    PROGRAM_CMDLINE := lpc21isp -verify main.hex /dev/ttyUSB0 115200 14746

##-----------------------------------------------------------------------------
## ARCHITECTURE: cortexM3_stm32
##
else ifeq ($(ARCH),cortexM3_stm32)

    ##-------------------------------------------------------------------------
    ## BOARD: stm32f103ze_stm3210e-eval
    ##
    ifeq ($(OPT_BOARD),stm32f103ze_stm3210e-eval)

        ## Base directory with header files for this board
        BOARD_INC := arch/cortexM3_stm32/stm32f103ze_stm3210e-eval

        ## Select linker script and boot file
        ## Their path must be relative to the miosix directory.
        BOOT_FILE := $(BOARD_INC)/core/stage_1_boot.o
        #LINKER_SCRIPT := already selected in board options

        ## Select architecture specific files
        ## These are the files in arch/<arch name>/<board name>
        ARCH_SRC :=                                  \
        $(BOARD_INC)/core/interrupts.cpp             \
        $(BOARD_INC)/drivers/serial.cpp              \
        $(BOARD_INC)/interfaces-impl/portability.cpp \
        $(BOARD_INC)/interfaces-impl/console.cpp     \
        $(BOARD_INC)/interfaces-impl/disk.cpp        \
        $(BOARD_INC)/interfaces-impl/delays.cpp      \
        $(BOARD_INC)/interfaces-impl/bsp.cpp         \
        $(BOARD_INC)/CMSIS/core_cm3.c                \
        $(BOARD_INC)/CMSIS/system_stm32f10x.c

        ## Add a #define to allow querying board name
        CFLAGS_BASE   += -D_BOARD_STM3210E_EVAL
        CXXFLAGS_BASE += -D_BOARD_STM3210E_EVAL

    ##-------------------------------------------------------------------------
    ## End of board list
    ##
    endif

    ## Select compiler
    PREFIX=arm-miosix-eabi-

    ## From compiler prefix form the name of the compiler and other tools
    CC  := $(PREFIX)gcc
    CXX := $(PREFIX)g++
    LD  := $(PREFIX)ld
    AR  := $(PREFIX)ar
    AS  := $(PREFIX)as
    CP  := $(PREFIX)objcopy
    OD  := $(PREFIX)objdump
    SZ  := $(PREFIX)size

    ## Select appropriate compiler flags for both ASM/C/C++/linker
    AFLAGS_BASE   := -mcpu=cortex-m3 -mthumb -mfix-cortex-m3-ldrd
    CFLAGS_BASE   += -D_ARCH_CORTEXM3_STM32 -DSTM32F10X_HD $(CLOCK_FREQ)     \
                     $(XRAM) -mcpu=cortex-m3 -mthumb -mfix-cortex-m3-ldrd    \
                     $(OPT_OPTIMIZATION) -ffunction-sections -Wall -g -c 
    CXXFLAGS_BASE += -D_ARCH_CORTEXM3_STM32 -DSTM32F10X_HD $(CLOCK_FREQ)     \
                     $(XRAM) $(OPT_EXCEPT) -mcpu=cortex-m3 -mthumb           \
                     -mfix-cortex-m3-ldrd $(OPT_OPTIMIZATION)                \
                     -ffunction-sections -Wall -g -c
    LFLAGS_BASE   := -mcpu=cortex-m3 -mthumb -mfix-cortex-m3-ldrd            \
                     -Wl,--gc-sections,-Map,main.map                         \
                     -Wl,-T./miosix/$(LINKER_SCRIPT) $(OPT_EXCEPT) -nostdlib

    ## Select architecture specific files
    ## These are the files in arch/<arch name>/common
    #ARCH_SRC += Nothing to add

    ## Base directory with header files for this board
    ARCH_INC := arch/cortexM3_stm32/common

    ## Select programmer command line
    ## This is the program that is invoked when the user types 'make program'
    ## The command must provide a way to program the board, or print an error
    ## message saying that 'make program' is not supported for that board.
    PROGRAM_CMDLINE := miosix/bootloaders/stm3210e-eval/pc_loader/pc_loader \
        /dev/ttyUSB0 main.bin

##-----------------------------------------------------------------------------
## end of architecture list
##
endif