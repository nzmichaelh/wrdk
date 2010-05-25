# Makefile that packages up a copy of wrdk
#

include ../samo-lib/Mk/definitions.mk

# Release version
VERSION = 0.1

SUBDIRS = loader examples/wrinv examples/baby examples/splash examples/pongex

all: build

# Build all tools
build:
	for i in $(SUBDIRS); do make -C $$i; done

clean:
	for i in $(SUBDIRS); do make -C $$i clean; done

# Short name of the host to build for
HOST ?= linux
# Where to build into
BUILD = $(shell pwd)/build/$(HOST)
# Where the final build is staged into
STAGING = $(BUILD)/wrdk
# Where the intermediate build stamps go
STAMPS = $(BUILD)/stamps
# Where chunks of the toolchain end up
C33 = $(STAGING)/c33-epson-elf
# Original toolchain source
TOOLCHAIN = ../toolchain

BINUTILS_VERSION = 2.10.1
BINUTILS_PACKAGE = binutils-${BINUTILS_VERSION}.tar.gz
BINUTILS_FILE = $(BINUTILS_PACKAGE)

GCC_VERSION = 3.3.2
GCC_PACKAGE = gcc-${GCC_VERSION}.tar.gz
GCC_FILE = $(GCC_PACKAGE)

PARALLEL = -j3

toolchain: $(STAMPS)/binutils $(STAMPS)/gcc

-include $(HOST).mk

# Shortcuts
binutils: $(STAMPS)/binutils
gcc: $(STAMPS)/gcc

# Configure binutils
# ldgram.y and bison 2.4.1 are incompatible.  Make sure we use the pre-generated one
$(BUILD)/binutils/Makefile:
	mkdir -p $(@D)
	cd $(@D) && CPPFLAGS="-D_FORTIFY_SOURCE=0" ../../../$(TOOLCHAIN)/binutils-$(BINUTILS_VERSION)/configure \
	--target=c33-epson-elf --prefix=$(BUILD)/wrdk $(BINUTILS_FLAGS)
	cp $(TOOLCHAIN)/binutils-$(BINUTILS_VERSION)/ld/ldgram.{c,h} $(@D)/ld
	touch $(@D)/ld/ldgram*

$(STAMPS)/binutils-build: $(BUILD)/binutils/Makefile
	$(MAKE) -C $(<D) $(PARALLEL)
	$(MAKE) -C $(<D) $(PARALLEL) install
	mkdir -p $(@D)
	touch $@

# Build and stage binutils
$(STAMPS)/binutils: $(STAMPS)/binutils-build $(STAMPS)/binutils-post
	touch $@

# Configure gcc
$(BUILD)/gcc/Makefile:
	mkdir -p $(@D)
	cd $(@D) && CPPFLAGS="-D_FORTIFY_SOURCE=0" ../../../$(TOOLCHAIN)/gcc-$(GCC_VERSION)/configure \
	--target=c33-epson-elf --prefix=$(BUILD)/wrdk  --enable-languages=c --disable-multilib $(GCC_FLAGS)

$(STAMPS)/gcc-build: $(BUILD)/gcc/Makefile
	$(MAKE) -C $(<D) -k $(PARALLEL)
	mkdir -p $(@D)
	touch $@

$(STAMPS)/gcc-install: $(STAMPS)/gcc-build
	$(MAKE) -C $(BUILD)/gcc -k $(PARALLEL) install
	touch $@

# Build and stage gcc
$(STAMPS)/gcc: $(STAMPS)/gcc-install
	touch $@

# Pulls together the includes, libraries, examples, and other
# components together into the staging area
package: binutils gcc package-main package-sdimage package-host

package-main:
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
	-cp bin/* $(STAGING)/bin
	rsync -Ca examples $(STAGING)
	cp -a share $(STAGING)
	cp $(GRIFO)/lds/* $(STAGING)/share
	cp -a loader $(STAGING)

package-sdimage:
	mkdir -p $(STAGING)/share/sdimage
	cp $(GRIFO)/grifo.elf $(STAGING)/share/sdimage/kernel.elf
	cp init.ini $(GRIFO)/examples/init/init.app $(STAGING)/share/sdimage
	for i in $(SUBDIRS); do $(MAKE) -C $$i install INSTALL_DIR=$(STAGING)/share/sdimage; done
	cd $(STAGING)/share/sdimage && sha256sum * > sha256.txt
	cp README $(STAGING)
	for i in $(SUBDIRS); do $(MAKE) -C $(STAGING)/$$i clean; done

# Archive up the final build
archive:
	tar czf wrdk-linux-$(VERSION).tar.gz -C build/linux wrdk
