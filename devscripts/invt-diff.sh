#!/bin/sh

# $Id$

# Quick script for checking that src files are in sync between groovx
# and invt.

if test $# -ne 1; then
    echo "usage: `basename $0` path/to/saliency"
    exit 1
fi

saliencydir=$1

tosync="src/rutz \
    src/nub \
    src/tcl \
    devscripts/cdeps.cc \
    devscripts/change-includes.pl \
    devscripts/dot-ldep-internal.tcl \
    devscripts/dot-ldep-modules.tcl \
    devscripts/set-svn-props.sh"

for d in $tosync; do
    echo "considering $d ..."
    if test -d "$d"; then
	files=`ls -1 ${d}/*.{h,cc}`;
    else
	files="$d";
    fi
    for f in $files; do
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
