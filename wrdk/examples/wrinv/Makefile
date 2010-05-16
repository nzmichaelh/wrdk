# Makefile
#

WRDK_SHARE := $(shell wrdk-config --share)
include $(WRDK_SHARE)/definitions.mk

# list of modules
SRC = $(wildcard *.c)
OBJ = $(addprefix $(BUILD2)/, $(SRC:.c=.o))

LIBS += $(LIB)

all: $(TARGETS)

include $(WRDK_SHARE)/rules.mk
