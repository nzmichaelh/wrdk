# Makefile
#

include $(shell wrdk-config --definitions)

TARGETS = stub.app stub.ico

# list of modules
SRC = $(wildcard *.c)
OBJ = $(addprefix $(BUILD)/, $(SRC:.c=.o))

LIBS += $(LIB)

all: $(TARGETS)

stub.app: build build/stub.o $(LIBS) $(GRIFO_APPLICATION_LDS)
	$(LD) -o $@ $(LDFLAGS) build/stub.o $(LIBS) -T $(GRIFO_APPLICATION_LDS)

loader.elf: build build/main.o $(LIBS) loader.lds
	$(LD) -o $@ $(LDFLAGS) build/main.o $(LIBS) -T loader.lds

$(BUILD)/%.inc: %.bin
	python bin2array $< $@

include $(shell wrdk-config --rules)
