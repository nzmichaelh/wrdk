# Common rules across all wrdk targets
#

LIBS += $(LIB)

all: $(TARGETS)

host:
	$(MAKE) TARGET=host

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

$(BUILD)/%.img: resources/%.xpm
	${IMAGE2HEADER} --auto-mask --inverted --header-file=$@ --variable-name=$(notdir ${@:.img=_image}) $<

# convert XPM to binary ICO format
$(BUILD)/%.ico: %.xpm
	$(XPM2ICON) --icon=$@ $<

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
