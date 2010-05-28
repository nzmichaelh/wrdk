# Common definitions across all wrdk targets
#

LAYOUT := $(shell wrdk-config --layout)

# Target to build for: samov1 (WikiReader), host
TARGET ?= samov1

# Build directory
BUILD = build
# Target specific build directory
BUILD2 = $(BUILD)/$(TARGET)

HERE := $(notdir $(shell pwd))
# Default name of this project
THIS := $(word $(words $(HERE)), $(HERE))

# Targets to build (application and icon)
TARGETS = $(BUILD2)/$(THIS).app $(BUILD)/$(THIS).ico
# Library to build into
LIB = $(BUILD2)/lib$(THIS).a

# Pull in the target and layout specific files
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

IMAGETOOL ?= wrdk-imagetool
