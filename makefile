SHELL := /bin/bash

TARGET := rf24_gateway

include xcompile.mk

SRCDIRS := ./
INCDIRS := ../

EXCLUDE_srcs := ./rf24_message_handler.cpp

#$(TARGET)_srcs := $(shell find $(SRCDIRS) -iname *.cpp -o -iname *.c | sort -u)
srcs := $(shell find $(SRCDIRS) -iname "*.cpp" -o -iname "*.c" | sort -u)

$(TARGET)_srcs := $(filter-out $(EXCLUDE_srcs), $(srcs))

OBJ_DIR = ./obj

# Set vpaths to find source files.
# This may become problematic if we ever repeat a source file name anywhere...
vpath %.c   $(SRCDIRS) $(BUILD_SOURCE_DIRS)
vpath %.cpp $(SRCDIRS) $(BUILD_SOURCE_DIRS)


INCDIRS += $(SRCDIRS)


#FLAGS   += -g -O0 -Wall                                         
FLAGS   += $(addprefix -I, $(INCDIRS))
CXXFLAGS += $(FLAGS) -lrf24-bcm -lrf24network -lrf24mesh -lmosquitto -lmosquittopp
CFLAGS   += $(FLAGS) -lrf24-bcm -lrf24network -lrf24mesh -lmosquitto -lmosquittopp

$(TARGET)_file:=$(OBJ_DIR)/$(TARGET)

$(TARGET)_objdir := $(OBJ_DIR)
$(TARGET)_srcs := $(sort $(notdir $(filter %.c,$($(TARGET)_srcs)) $(filter %.cpp,$($(TARGET)_srcs))))
#$(TARGET)_objs := $(shell find $(SRCDIRS) -iname *.cpp -o -iname *.c | sort -u)
$(TARGET)_objs   := $(addprefix $($(TARGET)_objdir)/,$(subst .c,.o,$($(TARGET)_srcs:.cpp=.o)))

$(TARGET)_deps := $(subst .o,.d,$($(TARGET)_objs))

$($(TARGET)_deps) $($(TARGET)_objs) $($(TARGET)_file): | $($(TARGET)_objdir)


$($(TARGET)_objdir)/%.o: %.cpp
	#@mkdir -p $(@D)
	$(CXX) $($(TARGET)_cxxflags) $(CXXFLAGS) -c -o $@ $<

$($(TARGET)_objdir)/%.o: %.c
	#@mkdir -p $(@D)
	$(CC) $($(TARGET)_cflags) $(CFLAGS) -c -o $@ $<

$($(TARGET)_objdir)/%.d: %.c
	#@mkdir -p $(@D)
	echo Generating $@...
	$(CC) -M $< -MT "$(@:.d=.o) $@" $($(TARGET)_cflags) $(CFLAGS) -MF $@

$($(TARGET)_objdir)/%.d: %.cpp
	#@mkdir -p $(@D)
	echo Generating $@...
	$(CXX)  -M $< -MT "$(@:.d=.o) $@" $($(TARGET)_cxxflags) $(CXXFLAGS) -MF $@

.PHONY: $(TARGET)
$(TARGET): $($(TARGET)_file)
$($(TARGET)_file): $($(TARGET)_deps) $($(TARGET)_objs)
	$(CXX) -o $@ $($(TARGET)_objs) $($(TARGET)_cxxflags) $(CXXFLAGS) $($(TARGET)_ldflags) $(LDFLAGS)

.DEFAULT_GOAL:=
.PHONY: all clean-all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf  $($(TARGET)_file) $($(TARGET)_objs) $($(TARGET)_objdir) $(BINDIR)
       

all_objdirs:=$(sort $(OBJDIR_BASE) $(OBJDIR) $(all_objdirs))# sort them to get rid of any dupes
all_objdirs += $($(TARGET)_objdir) $(BINDIR)

$(all_objdirs):
	mkdir -p $@

#clean-all: cleanall

include $($(TARGET)_deps) 



