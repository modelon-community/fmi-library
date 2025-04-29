The contents of the zlib-1.3.1 folder correspond to zlib release 1.3.1,
https://github.com/madler/zlib/tree/v1.3.1,
plus application of the "zlib_CMakeLists_changes.patch" patch.
And include unistd.h in gzguts.h for non-windows platforms to work around https://github.com/madler/zlib/issues/1044 
