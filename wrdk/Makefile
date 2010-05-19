# Makefile that packages up a copy of wrdk
#

# make USE_COLLECT2= HOST_CC=gcc GCC_FOR_TARGET=c33-epson-elf-gcc
# ../gcc-3.3.2/configure --target=c33-epson-elf --host=i586-mingw32msvc --build=i686-pc-linux-gnuaout --prefix=$HOME/mingw  --enable-languages=c
# ../binutils-2.10.1/configure --target=c33-epson-elf --host=i586-mingw32msvc --build=i686-pc-linux-gnuaout --prefix=$HOME/mingw  --program-prefix=c33-epson-elf --program-suffix=.exe

include ../samo-lib/Mk/definitions.mk

# Release version
VERSION = 0.1

SUBDIRS = loader examples/wrinv examples/baby examples/splash

all: build

# Build all tools
build:
	for i in $(SUBDIRS); do make -C $$i; done

clean:
	for i in $(SUBDIRS); do make -C $$i clean; done

# Short name of the host to build for
HOST = linux
# Where to build into
BUILD = $(shell pwd)/build/$(HOST)
# Where the final build is staged into
STAGING = $(BUILD)/wrdk
# Where the intermediate build stamps go
STAMPS = $(BUILD)/stamps
C33 = $(STAGING)/c33-epson-elf

# Where the Openmoko toolchain patches live
PATCHES = $(PWD)/../host-tools/toolchain-patches

BINUTILS_VERSION = 2.10.1
BINUTILS_PACKAGE = binutils-${BINUTILS_VERSION}.tar.gz
BINUTILS_FILE = $(BINUTILS_PACKAGE)

GCC_VERSION = 3.3.2
GCC_PACKAGE = gcc-${GCC_VERSION}.tar.gz
GCC_FILE = $(GCC_PACKAGE)

toolchain: $(STAMPS)/binutils $(STAMPS)/gcc

# Shortcuts
binutils-patch: $(STAMPS)/binutils-patch
gcc-patch: $(STAMPS)/gcc-patch

# Fetch binutils
$(STAMPS)/binutils-fetch:
	mkdir -p $(@D)
	cp ../host-tools/toolchain-download/binutils-* .
	touch $@

# Extract and add the patches
$(STAMPS)/binutils-patch: $(STAMPS)/binutils-fetch
	mkdir -p $(@D)
	rm -fr $(BUILD)/binutils-$(BINUTILS_VERSION)
	tar -xzf "${BINUTILS_FILE}" -C $(BUILD)
	cd $(BUILD)/binutils-${BINUTILS_VERSION} && \
	for p in $(PATCHES)/*-binutils-*.patch ; \
	do \
	   patch -p1 < "$${p}" ; \
	done
	touch $@

# Build and stage binutils
$(STAMPS)/binutils: $(STAMPS)/binutils-patch
	mkdir -p $(BUILD)/build-binutils
	cd $(BUILD)/build-binutils && \
	$(HOST_FLAGS) CPPFLAGS="-D_FORTIFY_SOURCE=0" ../binutils-$(BINUTILS_VERSION)/configure --prefix $(STAGING) --target=c33-epson-elf && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ${MAKE}
	${MAKE} -C $(BUILD)/build-binutils install
	touch $@

# Fetch gcc
$(STAMPS)/gcc-fetch:
	mkdir -p $(@D)
	cp ../host-tools/toolchain-download/gcc-* .
	touch $@

# Extract and patch gcc
$(STAMPS)/gcc-patch: $(STAMPS)/gcc-fetch
	tar -xzf "${GCC_FILE}" -C $(BUILD)
	cd $(BUILD)/gcc-$(GCC_VERSION) && \
	for p in $(PATCHES)/*-gcc-*.patch ; \
	do \
	   patch -p1 < "$${p}" ; \
	done
	touch $@

# Build and stage gcc
$(STAMPS)/gcc: $(STAMPS)/binutils $(STAMPS)/gcc-patch
	mkdir -p $(BUILD)/build-gcc
	export PATH="$(STAGING)/bin:${PATH}" && \
	cd $(BUILD)/build-gcc && \
	$(HOST_FLAGS) CPPFLAGS="-D_FORTIFY_SOURCE=0" ../gcc-$(GCC_VERSION)/configure --prefix $(STAGING) --target=c33-epson-elf --enable-languages=c && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ${MAKE} && \
	${MAKE} install
	${TOUCH} $@

# Pulls together the includes, libraries, examples, and other
# components together into the staging area
package:
	mkdir -p $(C33)/include $(C33)/lib
	cp -a $(SAMO_LIB_INCLUDE)/* \
	$(FATFS_INCLUDE)/*.h \
	$(GRIFO_INCLUDE)/* \
	$(MINI_LIBC_INCLUDE)/* \
	$(C33)/include
	cp -a $(MINI_LIBC_LIB) \
	$(FATFS_LIB) \
	$(GRIFO_LIB) \
	$(C33)/lib
	cp -a $(IMAGE2HEADER) $(STAGING)/bin
	cp -a bin/* $(STAGING)/bin
	cp -a $(GRIFO_SCRIPTS)/* $(STAGING)/bin
	rsync -Ca examples $(STAGING)
	cp -a share $(STAGING)
	cp $(GRIFO)/lds/* $(STAGING)/share
	cp -a loader $(STAGING)
	mkdir -p $(STAGING)/share/sdimage
	cp $(GRIFO)/grifo.elf $(STAGING)/share/sdimage/kernel.elf
	cp init.ini $(GRIFO)/examples/init/init.app $(STAGING)/share/sdimage
	for i in $(SUBDIRS); do $(MAKE) -C $$i install INSTALL_DIR=$(STAGING)/share/sdimage; done
	cd $(STAGING)/share/sdimage && sha256sum * > sha256.txt
	cp README $(STAGING)
	for i in $(SUBDIRS); do $(MAKE) -C $(STAGING)/$$i clean; done

foo:
	$(MAKE) -C loader install INSTALL_DIR=$(STAGING)/share/sdimage
#	for i in $(SUBDIRS); do $(MAKE) -C $$i install INSTALL_DIR=$(STAGING)/share/sdimage; done

# Archive up the final build
archive:
	tar czf wrdk-linux-$(VERSION).tar.gz -C build/linux wrdk
