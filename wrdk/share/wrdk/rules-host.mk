$(BUILD2)/$(THIS).app: $(BUILD2) $(BUILD2)/$(THIS).o $(LIB)
	$(CC) -o $@ $(LDFLAGS) $(BUILD2)/$(THIS).o $(LIBS)
