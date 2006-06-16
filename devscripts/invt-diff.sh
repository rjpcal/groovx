#!/bin/sh

# $Id$

# Quick script for checking that src files are in sync between groovx
# and invt.

if test $# -lt 1; then
    echo "usage: `basename $0` [-q] path/to/saliency"
    exit 1
fi

saliencydir=""
listmode=0

for arg in "$@"; do
    case "$arg" in
	-*)
	    case "$arg" in
		-l)
		    listmode=1
		    ;;
		*)
		    echo "unknown option '$arg'"
		    exit 1
		    ;;
	    esac
	    ;;
	*)
	    if test "x$saliencydir" != "x"; then
		echo "only one saliency directory can be given"
		exit 1
	    fi
	    saliencydir=$arg
	    ;;
    esac
done

saliencydir=$1

tosync="src/rutz \
    src/nub \
    src/tcl \
    devscripts/cdeps.cc \
    devscripts/change-includes.pl \
    devscripts/dot-ldep-internal.tcl \
    devscripts/dot-ldep-modules.tcl \
    devscripts/make-ldep-html.sh \
    devscripts/set-svn-props.sh"

for d in $tosync; do
    if test $listmode -ne 1; then
	echo "considering $d ..."
    fi
    if test -d "$d"; then
	files=`ls -1 $saliencydir/$d/*.{h,cc,dxy}`;
    else
	files="$saliencydir/$d";
    fi
    for f in $files; do
	localfile=${f#$saliencydir/}
	if test ! -f $localfile; then
	    echo "no such file: $localfile"
	else
	    diff -u $localfile $f \
		| grep "^\(+\|-\)" \
		| grep -v "^+++" \
		| grep -v "^---" \
		| fgrep -v '$Id' \
		| fgrep -v '$HeadURL' \
		| fgrep -v '$Revision' \
		> tmpdiff
	    if test -s tmpdiff; then
		if test $listmode -eq 1; then
		    echo "$localfile"
		else
		    diff -u $localfile $f
		fi
	    fi
	    rm tmpdiff
	fi
    done

done
