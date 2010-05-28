$(BUILD2)/$(THIS).app: images $(BUILD2) $(BUILD2)/$(THIS).o $(LIB) $(GRIFO_APPLICATION_LDS)
	$(LD) -o $@ $(LDFLAGS) $(BUILD2)/$(THIS).o $(LIBS) -T $(GRIFO_APPLICATION_LDS)

$(BUILD2)/%.bin: $(BUILD2)/%.app
	 $(OBJCOPY) -j .text -j .data -j .rodata -O binary $< $@


