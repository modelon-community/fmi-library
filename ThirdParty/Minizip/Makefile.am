MINIZIP_DIR=minizip

$(abs_builddir)/../../minizip_install:
	cd $(MINIZIP_DIR) && $(MAKE) $(AM_MAKEFLAGS) install

all-local: $(abs_builddir)/../../minizip_install

check-local:
	cd $(MINIZIP_DIR) && $(MAKE) $(AM_MAKEFLAGS) test

clean-local:
	cd $(MINIZIP_DIR) && $(MAKE) $(AM_MAKEFLAGS) clean

