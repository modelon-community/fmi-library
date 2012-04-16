EXPAT_DIR=expat-2.0.1


$(abs_builddir)/../../expat_install:
	cd $(EXPAT_DIR) && $(MAKE) $(AM_MAKEFLAGS) install

all-local: $(abs_builddir)/../../expat_install

check-local:
	cd $(EXPAT_DIR) && $(MAKE) $(AM_MAKEFLAGS) test

clean-local:
	cd $(EXPAT_DIR) && $(MAKE) $(AM_MAKEFLAGS) clean

