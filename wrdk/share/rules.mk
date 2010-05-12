# Common rules across all wrdk targets
#

LIBS += $(LIB)

all: $(TARGETS)

host:
	$(MAKE) TARGET=host

OBJ ?= $(addprefix $(BUILD2)/, $(SRC:.c=.o))

$(LIB): $(OBJ)
	$(RM) -f $@
	$(AR) r $@ $^

$(BUILD2)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD2)/%.d: %.c
	mkdir -p $(BUILD2)
	$(CC) -MM $(CFLAGS) -MT $(BUILD2)/$*.o -MG -MF $@ $<

ifeq ($(TARGET),host)
$(BUILD2)/$(THIS).app: $(BUILD2) $(BUILD2)/$(THIS).o $(LIB)
	$(CC) -o $@ $(LDFLAGS) $(BUILD2)/$(THIS).o $(LIBS)
else
$(BUILD2)/$(THIS).app: $(BUILD2) $(BUILD2)/$(THIS).o $(LIB) $(GRIFO_APPLICATION_LDS)
	$(LD) -o $@ $(LDFLAGS) $(BUILD2)/$(THIS).o $(LIBS) -T $(GRIFO_APPLICATION_LDS)
endif

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -j .rodata -O binary $< $@

$(BUILD)/%.img: resources/%.xpm
	${IMAGE2HEADER} --auto-mask --inverted --header-file=$@ --variable-name=$(notdir ${@:.img=_image}) $<

# convert XPM to binary ICO format
$(BUILD)/%.ico: %.xpm
	${GRIFO_SCRIPTS}/xpm2icon --icon=$@ $<

$(BUILD2):
	$(MKDIR) $@

clean:
	$(RM) -r $(TARGETS) $(BUILD)
	$(RM) -r *.o *.app *.elf *.map *.asm33 *.dump *.ico *.inc

$(BUILD2)/deps.mk: $(addprefix $(BUILD2)/, $(SRC:.c=.d))
	mkdir -p $(BUILD2)
	cat $^ > $@

include $(BUILD2)/deps.mk
