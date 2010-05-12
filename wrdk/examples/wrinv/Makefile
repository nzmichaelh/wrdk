# Makefile
#

include $(shell wrdk-config --definitions)

# list of modules
SRC = $(wildcard *.c)
OBJ = $(addprefix $(BUILD2)/, $(SRC:.c=.o))

LIBS += $(LIB)

all: $(TARGETS)

include $(shell wrdk-config --rules)
