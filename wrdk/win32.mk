BINUTILS_FLAGS = --host=i586-mingw32msvc --build=i686-pc-linux-gnuaout  --program-prefix=c33-epson-elf- --program-suffix=.exe
GCC_FLAGS = --host=i586-mingw32msvc --build=i686-pc-linux-gnuaout 

$(STAMPS)/binutils-post:
	cd $(C33)/bin && (for i in *; do mv $$i $$i.exe; done)
	touch $@

package-host:
	cd $(STAGING)/bin; rm `grep -l python *`
	cp -a bin/dist/* $(STAGING)/bin
	find $(STAGING) -name README -exec mv {} {}.txt \;
	find $(STAGING) -name *.txt -exec flip -m {} \;

$(BUILD)/msys: msys.tar.bz2
	tar xjf $< -C $(@D)

installer: $(BUILD)/msys
	makensis wrdk.nsi
	mv wrdk-installer.exe wrdk-installer-$(VERSION).exe
