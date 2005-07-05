#!/bin/sh

# $Id$

# Quick script for checking that src files are in sync between groovx
# and invt.

if test $# -ne 1; then
    echo "usage: `basename $0` path/to/saliency"
    exit 1
fi

saliencydir=$1

for d in src/rutz src/nub src/tcl; do
    for f in ${d}/*.{h,cc}; do
	diff -u $f $saliencydir/$f \
	    | grep "^\(+\|-\)" \
	    | grep -v "^+++" \
	    | grep -v "^---" \
	    | fgrep -v '$Id' \
	    | fgrep -v '$HeadURL' \
	    | fgrep -v '$Revision' \
	    > tmpdiff
	if test -s tmpdiff; then
	    diff -u $f $saliencydir/$f
	fi
	rm tmpdiff
    done
done
