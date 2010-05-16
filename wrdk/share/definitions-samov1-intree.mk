INCLUDES = \
	-I$(WIKIREADER_ROOT)/samo-lib/mini-libc/include \
	-I$(WIKIREADER_ROOT)/samo-lib/grifo/include \
	-I$(WIKIREADER_ROOT)/samo-lib/include

CFLAGS += \
	-DSAMO_RESTRICTIONS=1

LIBS = \
	$(WIKIREADER_ROOT)/samo-lib/mini-libc/lib/libc.a \
	$(WIKIREADER_ROOT)/samo-lib/grifo/lib/libgrifo.a \
	$(WIKIREADER_ROOT)/host-tools/toolchain-install/lib/gcc-lib/c33-epson-elf/3.3.2/libgcc.a

LDFLAGS = 

GRIFO_APPLICATION_LDS = $(WIKIREADER_ROOT)/samo-lib/grifo/lds/application.lds

