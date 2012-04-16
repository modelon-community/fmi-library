#! /bin/sh

ARGS=""
for i in $*
do
        case $1 in
                --srcdir=*)
                        srcdir=`expr "x$1" : 'x[^=]*=\(.*\)'`
                        ;;
                --prefix=*)
                        prefix=`expr "x$1" : 'x[^=]*=\(.*\)'`
                        ;;
                *)
                        ARGS="$ARGS $1"
        esac
        shift 1
done

build_dir=`pwd`

exec $srcdir/configure ${1+"$@"} --with-cflags=-fPIC --with-cxxflags=-fPIC --disable-shared --enable-static --prefix=$build_dir/../../../minizip_install --with-zlib=$build_dir/../../../zlib_install