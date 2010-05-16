include ../samo-lib/Mk/definitions.mk

SUBDIRS = loader examples/wrinv examples/baby

#.PHONY: $(SUBDIRS)

all: build

build:
	for i in $(SUBDIRS); do make -C $$i; done

clean:
	for i in $(SUBDIRS); do make -C $$i clean; done

BUILD = build/wrdk
C33 = $(BUILD)/c33-epson-elf

toolchain:
	mkdir -p $(C33)/include $(C33)/lib
	cp -a $(HOST_TOOLS)/toolchain-install/* $(BUILD)
	cp -a $(SAMO_LIB_INCLUDE)/* \
	$(FATFS_INCLUDE)/*.h \
	$(GRIFO_INCLUDE)/* \
	$(MINI_LIBC_INCLUDE)/* \
	$(C33)/include
	cp -a $(MINI_LIBC_LIB) \
	$(FATFS_LIB) \
	$(GRIFO_LIB) \
	$(C33)/lib
	cp -a $(IMAGE2HEADER) $(BUILD)/bin
	cp -a bin/* $(BUILD)/bin
	cp -a $(GRIFO_SCRIPTS)/* $(BUILD)/bin
	rsync -Ca examples $(BUILD)
	cp -a share $(BUILD)
	cp $(GRIFO)/lds/* $(BUILD)/share
	cp -a loader $(BUILD)
	for i in $(SUBDIRS); do $(MAKE) -C $(BUILD)/$$i clean; done


