#!/bin/sh

# $Id: list-sources.sh 4709 2005-06-13 05:46:46Z rjpeters $

# Just print a list of all source files to stdout, for use in other
# scripts.

find `dirname $0`/../src \
    -name \*.H \
    -or -name \*.C \
    -or -name \*.cc \
    -or -name \*.hh \
    -or -name \*.c \
    -or -name \*.h \
    -or -name \*.hpp \
    -or -name \*.cpp
