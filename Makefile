PREFIX = /usr/local

build: build-config
	(cd build-fmil; make)

test: build-config
	(cd build-fmil; make test)

install: build-config
	(cd build-fmil; make install)

build-config: build-fmil
	(cd build-fmil; cmake -DFMILIB_INSTALL_PREFIX=$(PREFIX) ../)

build-fmil:
	mkdir build-fmil

clean:
	(cd build-fmil;	make clean)

dist-clean: 
	-rm -rf build-fmil

.PHONY: build test install build-config clean dist-clean
