#!/bin/sh

echo "@@@@@ START: `date`"

CROSS_TARGET=c33-epson-elf
INSTDIR=/usr/local/c33
PREFIX=c33-epson-elf-
TARGETS="ld as objdump"
TARGETS=$TARGETS" ar ranlib objcopy nm strip size strings"
TARGETS=$TARGETS" readelf addr2line cxxfilt"
WORKDIR=c33-binutils
ARCHIVE_DIR=$WORKDIR/archive
ARCHIVE_DATE=`date +%y%m%d`

SRCDIR=binutils-2.10.1


case `uname -s` in
CYGWIN*)
	EXE=.exe
	;;
Linux*)
	EXE=
	;;
*)
	EXE=
	;;
esac
LANG=C

clean=no
config=no
build=no
install=no
tags=no
archive=no

if [ $# -eq 0 ]; then
	clean=yes
	config=yes
	build=yes
	install=yes
	tags=yes
fi

while [ $# -gt 0 ]; do
	case $1 in
	all)
		clean=yes
		config=yes
		build=yes
		install=yes
		tags=yes
		;;
	clean)
		clean=yes
		;;
	config)
		config=yes
		;;
	build)
		build=yes
		;;
	install)
		install=yes
		;;
	tags)
		tags=yes
		;;
	archive)
		archive=yes
		;;
	esac

	shift
done

if [ "$clean" = "yes" ]; then
	echo "@@@@@ CLEAN: `date`"

	if [ -e $$WORKDIR ]; then
		mv $WORKDIR $WORKDIR-gomi
		rm -rf $WORKDIR-gomi
	fi
fi

if [ "$config" = "yes" ]; then
	echo "@@@@@ CONFIG: `date`"

	mkdir $WORKDIR
	(cd $WORKDIR; \
		../$SRCDIR/configure \
			--target=$CROSS_TARGET \
			--prefix=$INSTDIR ) || exit 1
fi

if [ "$build" = "yes" ]; then
	echo "@@@@@ BUILD: `date`"

#	(cd $WORKDIR; make) || exit 1
	(cd $WORKDIR; make clean all) || exit 1

	cp $WORKDIR/ld/ld-new$EXE c33-ld$EXE
	cp $WORKDIR/gas/as-new$EXE c33-as$EXE
	cp $WORKDIR/binutils/objdump$EXE c33-objdump$EXE

	cp $WORKDIR/binutils/addr2line$EXE c33-addr2line$EXE
	cp $WORKDIR/binutils/ar$EXE c33-ar$EXE
	cp $WORKDIR/binutils/cxxfilt$EXE c33-cxxfilt$EXE
	cp $WORKDIR/binutils/nm-new$EXE c33-nm$EXE
	cp $WORKDIR/binutils/objcopy$EXE c33-objcopy$EXE
	cp $WORKDIR/binutils/ranlib$EXE c33-ranlib$EXE
	cp $WORKDIR/binutils/readelf$EXE c33-readelf$EXE
	cp $WORKDIR/binutils/size$EXE c33-size$EXE
	cp $WORKDIR/binutils/strings$EXE c33-strings$EXE
	cp $WORKDIR/binutils/strip-new$EXE c33-strip$EXE


	for i in $TARGETS; do
		strip c33-$i$EXE
	done

	cp c33-as$EXE as_org$EXE
	cp c33-ld$EXE ld$EXE
	cp c33-objdump$EXE objdump$EXE

fi

if [ "$install" = "yes" ]; then
	echo "@@@@@ INSTALL: `date`"

	mkdir -p $INSTDIR/bin
	for i in $TARGETS; do
		cp -p c33-$i$EXE $INSTDIR/bin/$PREFIX$i$EXE
	done
fi

if [ "$tags" = "yes" ]; then
	echo "@@@@@ TAGS: `date`"

	rm -rf tags
	ctags -R $SRCDIR
fi

if [ "$archive" = "yes" ]; then
	echo "@@@@@ ARCHIVE: `date`"
	mkdir -p $ARCHIVE_DIR

	echo "Archive to '$ARCHIVE_DIR/$SRCDIR-$ARCHIVE_DATE.tgz'"
	if [ -f $ARCHIVE_DIR/$SRCDIR-$ARCHIVE_DATE.tgz ]; then
		echo -n "This file is exist.  Really over write? "
		read x
	fi
	tar cfz $ARCHIVE_DIR/$SRCDIR-$ARCHIVE_DATE.tgz \
		MAKE.sh \
		$SRCDIR
fi

echo "@@@@@ DONE: `date`"

exit 0
