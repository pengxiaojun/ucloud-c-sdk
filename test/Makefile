BINTARGET = ucloudtest
CFLAGS += -I../include
CFLAGS += -I$(JSON_ROOT)/include
CXXFLAGS += -I../include

LDFLAGS += -L$(JSON_ROOT)/lib64 -ljson-c
LDFLAGS += -lucloud-c -lcurl

CSRC += $(wildcard *.c)
CXXSRC += $(wildcard *.cpp)

include ../include/Makefile.inc
