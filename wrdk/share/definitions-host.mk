CFLAGS += -I$(WIKIREADER_ROOT)/wrdk/wrem -DWRDK_HOST

LIBS += $(WIKIREADER_ROOT)/wrdk/wrem/libwrem.a -lSDL -lrt -lm
