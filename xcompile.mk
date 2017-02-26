SHELL := /bin/bash

# These tools are the host machine's arm cortex a8-target compilers
# They run on HOST and produce binaries that run on arm cortex a8
# The user should take care of putting the tools in the path...

ARCH=armv6zk
ifeq "$(shell uname -m)" "armv7l"
ARCH=armv7-a
endif

# Compiler/linker flags to sepcify CPU
CPU_FLAGS   := -mfpu=vfp -mfloat-abi=hard -march=$(ARCH) -mtune=arm1176jzf-s 


#SYSINCLUDE  := $(TOOLS_PATH)/include

CXXFLAGS    := -std=c++11 -Wall -Wno-main -pedantic -Wreturn-type -Wunused -fdata-sections -ffunction-sections $(CPU_FLAGS)
#CXXFLAGS    := -std=c++11 -Ofast -Wall -Wno-main -pedantic -Wreturn-type -Wunused -fdata-sections -ffunction-sections $(CPU_FLAGS)
CFLAGS      := $(CPU_FLAGS) 


CXX         := g++ 
CC          := gcc 
CPP         := cpp 
ASM         := as
AR          := ar
LD          := ld
STRIP       := strip
SIZE       := size


# Add this architecture to the master list
ALL_ARCHES += $(ARCH)


