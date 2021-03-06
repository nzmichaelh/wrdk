# Common rules across all wrdk targets
#

LIBS += $(LIB)

IMAGE_MASK ?= --with-mask

IMAGES ?= $(wildcard resources/*.xpm) $(wildcard resources/*.png)
IMAGE_HEADERS = $(IMAGES:resources/%=build/%.h)

all: $(TARGETS)

host:
	$(MAKE) TARGET=host

images: $(IMAGE_HEADERS)

install: all
	mkdir -p $(INSTALL_DIR)
	cp -f $(TARGETS) $(INSTALL_DIR)

OBJ ?= $(addprefix $(BUILD2)/, $(SRC:.c=.o))

$(LIB): $(OBJ)
	$(RM) -f $@
	$(AR) r $@ $^

$(BUILD2)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD2)/%.d: %.c
	mkdir -p $(BUILD2)
	$(CC) -MM $(CFLAGS) -MT $(BUILD2)/$*.o -MG -MF $@ $<

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -j .rodata -O binary $< $@

# Convert a XPM image into a header file
$(BUILD)/%.xpm.h: resources/%.xpm
	$(IMAGETOOL) -f header $(IMAGE_MASK) -o $@ --variable-name=$(notdir $(@:.xpm.h=_image)) $<

# Convert a PNG image into a header file
$(BUILD)/%.png.h: resources/%.png
	$(IMAGETOOL) -f header $(IMAGE_MASK) -o $@ --variable-name=$(notdir $(@:.png.h=_image)) $<

# convert XPM to binary ICO format
$(BUILD)/%.ico: %.xpm
	$(IMAGETOOL) -f ico -o $@ $<

$(BUILD2):
	$(MKDIR) $@

clean:
	rm -rf $(TARGETS) $(BUILD) $(INSTALL_DIR)
	rm -rf *.o *.app *.elf *.map *.asm33 *.dump *.ico *.inc

$(BUILD2)/deps.mk: $(addprefix $(BUILD2)/, $(SRC:.c=.d))
	mkdir -p $(BUILD2)
	cat $^ > $@

-include $(WRDK_SHARE)/rules-$(LAYOUT).mk
-include $(WRDK_SHARE)/rules-$(TARGET).mk
-include $(WRDK_SHARE)/rules-$(TARGET)-$(LAYOUT).mk

include $(BUILD2)/deps.mk
