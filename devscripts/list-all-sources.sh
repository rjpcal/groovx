#!/bin/sh


# Just print a list of all source files to stdout, for use in other
# scripts.

flz=$(find `dirname $0`/../src \
    -name \*.H \
    -or -name \*.C \
    -or -name \*.cc \
    -or -name \*.hh \
    -or -name \*.c \
    -or -name \*.h \
    -or -name \*.hpp \
    -or -name \*.cpp)

for f in $flz README TODO TUTORIAL doc/groovx.dxy; do
    g=${f#`dirname $0`/../}
    echo $g
done
