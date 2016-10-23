SHELL := /bin/bash
TARGET := message_test
CXX=g++
CPPFLAGS=--std=c++11 

SRCDIRS := ./
$(TARGET)_objdir = ./
$(TARGET)_srcs := main.cpp  message_handler.cpp
$(TARGET)_file:=$(TARGET).out

$(TARGET)_objs   := $(addprefix $($(TARGET)_objdir)/,$(subst .c,.o,$($(TARGET)_srcs:.cpp=.o)))

.PHONY: all clean
all: $(TARGET)_file
	echo Done.

$($(TARGET)_objdir)/%.o: %.cpp
	$(CXX)  $(CPPFLAGS) -c $<

$(TARGET)_file: $($(TARGET)_objs)
	$(CXX) -o $@ $^ 

clean: 
	rm $(TARGET)_file $(TARGET)_objs 
