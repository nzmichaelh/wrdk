# Common rules across all wrdk targets
#

$(LIB): $(OBJ)
	$(RM) -f $@
	$(AR) r $@ $^

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.d: %.c
	mkdir -p $(BUILD)
	$(CC) -MM $(CFLAGS) -MT $(BUILD)/$*.o -MG -MF $@ $<

$(THIS).app: build build/$(THIS).o $(LIBS) $(GRIFO_APPLICATION_LDS)
	$(LD) -o $@ $(LDFLAGS) build/$(THIS).o $(LIBS) -T $(GRIFO_APPLICATION_LDS)

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -j .rodata -O binary $< $@

$(BUILD)/%.img: resources/%.xpm
	${IMAGE2HEADER} --inverted --header-file=$@ --variable-name=$(notdir ${@:.img=_image}) $<

# convert XPM to binary ICO format
%.ico: %.xpm
	${GRIFO_SCRIPTS}/xpm2icon --icon=$@ $<

build:
	$(MKDIR) $@

clean:
	$(RM) -r $(TARGETS) build
	$(RM) -r *.o *.app *.elf *.map *.asm33 *.dump *.ico *.inc

$(BUILD)/deps.mk: $(addprefix $(BUILD)/, $(SRC:.c=.d))
	mkdir -p $(BUILD)
	cat $^ > $@

include $(BUILD)/deps.mk
