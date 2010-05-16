# Makefile
#

WRDK_SHARE := $(shell wrdk-config --share)
include $(WRDK_SHARE)/definitions.mk

TARGETS = stub.app $(BUILD)/stub.ico

# list of modules
SRC = $(wildcard *.c)
OBJ = $(addprefix $(BUILD2)/, $(SRC:.c=.o))

LIBS += $(LIB)

all: $(TARGETS)

stub.app: $(BUILD2) $(BUILD2)/stub.o $(LIB) $(GRIFO_APPLICATION_LDS)
	$(LD) -o $@ $(LDFLAGS) $(BUILD2)/stub.o $(LIBS) -T $(GRIFO_APPLICATION_LDS)

loader.elf: $(BUILD2) $(BUILD2)/main.o $(LIB) loader.lds
	$(LD) -o $@ $(LDFLAGS) $(BUILD2)/main.o $(LIBS) -T loader.lds

$(BUILD)/%.inc: %.bin
	python bin2array $< $@

include $(WRDK_SHARE)/rules.mk
