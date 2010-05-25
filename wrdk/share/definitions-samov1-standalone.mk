INCLUDES =

CFLAGS += \
	-DSAMO_RESTRICTIONS=1

LIBS = -lgrifo -lc $(WRDK_ROOT)/lib/gcc-lib/c33-epson-elf/3.3.2/libgcc.a

LDFLAGS = -L$(WRDK_ROOT)/c33-epson-elf/lib

GRIFO_APPLICATION_LDS = $(WRDK_SHARE)/application.lds
