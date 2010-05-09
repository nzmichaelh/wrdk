SUBDIRS = loader examples/wrinv examples/baby

.PHONY: $(SUBDIRS)

all: build

build:
	for i in $(SUBDIRS); do make -C $$i; done

clean:
	for i in $(SUBDIRS); do make -C $$i clean; done

