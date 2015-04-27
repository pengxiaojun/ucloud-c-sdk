SOTARGET = libucloud-c.so
CFLAGS += -I../include
CFLAGS += -I$(JSON_ROOT)/include
CXXFLAGS += -I../include

#debug switch
#CFLAGS += -D__DEBUG__

LDFLAGS += -L$(JSON_ROOT)/lib64 -ljson-c

CSRC += $(wildcard *.c)
CXXSRC += $(wildcard *.cpp)

include ../include/Makefile.inc
