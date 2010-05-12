# Common definitions across all wrdk targets
#

TARGET ?= samov1

BUILD = build
BUILD2 = $(BUILD)/$(TARGET)

THIS := $(notdir $(shell pwd))

TARGETS = $(BUILD2)/$(THIS).app $(BUILD)/$(THIS).ico
LIB = $(BUILD2)/lib$(THIS).a

INCLUDES += $(shell wrdk-config --target=$(TARGET) --cflags)
GRIFO_APPLICATION_LDS := $(shell wrdk-config --target=$(TARGET) --lds)

HOST_TOOLS := $(shell wrdk-config --host-tools)
SAMO_LIB := $(shell wrdk-config --samo-lib)

GRIFO = ${SAMO_LIB}/grifo
GRIFO_SCRIPTS = ${GRIFO}/scripts

IMAGE2HEADER= $(HOST_TOOLS)/imagetool/image2header

ifeq ($(TARGET),host)
CROSS_COMPILE =
else
CROSS_COMPILE ?= c33-epson-elf-
OPT = -O2
CFLAGS += -mlong-calls -fno-builtin -mc33pe
endif

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS += -Wall -ggdb --std=gnu99
CFLAGS += $(INCLUDES)
CFLAGS += $(OPT)

LIBS := $(shell wrdk-config --target=$(TARGET) --ldflags)
