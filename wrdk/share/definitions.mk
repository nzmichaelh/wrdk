# Common definitions across all wrdk targets
#

THIS := $(notdir $(shell pwd))

TARGETS = $(THIS).app $(THIS).ico
LIB = $(BUILD)/lib$(THIS).a

INCLUDES += $(shell wrdk-config --cflags)
GRIFO_APPLICATION_LDS := $(shell wrdk-config --lds)

HOST_TOOLS := $(shell wrdk-config --host-tools)
SAMO_LIB := $(shell wrdk-config --samo-lib)

GRIFO = ${SAMO_LIB}/grifo
GRIFO_SCRIPTS = ${GRIFO}/scripts

IMAGE2HEADER= $(HOST_TOOLS)/imagetool/image2header

CROSS_COMPILE ?= c33-epson-elf-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy

BUILD = build

LIBS := $(shell wrdk-config --ldflags)

OPT = -O2
CFLAGS += -Wall -gstabs -mlong-calls -fno-builtin $(OPT) --std=gnu99 -mc33pe $(INCLUDES)
