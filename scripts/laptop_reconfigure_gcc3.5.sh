#!/bin/sh

gccdir=/home/rjpeters/local/gcc-3.5-20040808
export CXX=${gccdir}/bin/g++-3.5-20040808
export LD_LIBRARY_PATH="${gccdir}/lib:${LD_LIBRARY_PATH}"

./configure --prefix=/home/rjpeters/local/groovx-3.5 \
    --with-tcltk=/home/rjpeters/local/tcl8.5a1 \
    --without-matlab
