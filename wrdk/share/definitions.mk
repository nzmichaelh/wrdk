# Common definitions across all wrdk targets
#

LAYOUT := $(shell wrdk-config --layout)

TARGET ?= samov1

BUILD = build
BUILD2 = $(BUILD)/$(TARGET)
INSTALL_DIR = dist

HERE := $(notdir $(shell pwd))
THIS := $(word $(words $(HERE)), $(HERE))

TARGETS = $(BUILD2)/$(THIS).app $(BUILD)/$(THIS).ico
LIB = $(BUILD2)/lib$(THIS).a

-include $(WRDK_SHARE)/definitions-$(LAYOUT).mk
-include $(WRDK_SHARE)/definitions-$(TARGET).mk
-include $(WRDK_SHARE)/definitions-$(TARGET)-$(LAYOUT).mk

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS += -Wall -ggdb --std=gnu99
CFLAGS += $(INCLUDES)
CFLAGS += $(OPT)
