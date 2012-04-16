ZLIB_BUILD_DIR=$(abs_builddir)/../../zlib_build
ZLIB_SRC_DIR=ThirdParty/Zlib/zlib-1.2.6
ZLIB_INSTALL_DIR=$(abs_top_builddir)/zlib_install

$(ZLIB_BUILD_DIR):
	mkdir -p $(ZLIB_BUILD_DIR) && \
	cd $(ZLIB_BUILD_DIR) && \
	case $(build) in \
	*-cygwin*) \
	cmake -G "MSYS Makefiles" $(abs_top_srcdir)/$(ZLIB_SRC_DIR) -DCMAKE_INSTALL_PREFIX=$(ZLIB_INSTALL_DIR) -DBUILD_SHARED_LIBS=OFF ;; \
  	*-mingw*) \
	cmake -G "MSYS Makefiles" $(abs_top_srcdir)/$(ZLIB_SRC_DIR) -DCMAKE_INSTALL_PREFIX=$(ZLIB_INSTALL_DIR) -DBUILD_SHARED_LIBS=OFF ;; \
  	*-apple*) \
	cmake $(abs_top_srcdir)/$(ZLIB_SRC_DIR) -DCMAKE_INSTALL_PREFIX=$(ZLIB_INSTALL_DIR) -DBUILD_SHARED_LIBS=OFF ;; \
  	*) \
	cmake $(abs_top_srcdir)/$(ZLIB_SRC_DIR) -DCMAKE_INSTALL_PREFIX=$(ZLIB_INSTALL_DIR) -DBUILD_SHARED_LIBS=OFF ;; \
	esac

all-local: $(ZLIB_BUILD_DIR)
	cd $(ZLIB_BUILD_DIR) && make $(AM_MAKEFLAGS) install

check-local:
	cd $(ZLIB_BUILD_DIR) && $(MAKE) $(AM_MAKEFLAGS) test

clean-local:
	cd $(ZLIB_BUILD_DIR) && $(MAKE) $(AM_MAKEFLAGS) clean

