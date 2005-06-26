#!/bin/sh

# $Id: make-ldep-html.sh 4916 2005-06-26 00:41:02Z rjpeters $

# Quick script to build a set of html pages showing the link
# dependencies, with clickable directed graphs made by 'dot'. You
# probably don't want to call this directly; instead use 'make
# ldep-html'.

if test $# -ne 3; then
    echo "usage: `basename $0` ldeps-file srcdir outdir"
    exit 1
fi

ldeps=$1
srcdir=$2
outdir=$3
thisdir=`dirname $0`

mkdir -p $outdir

echo "modules ..."
$thisdir/dot-ldep-modules.tcl $ldeps \
    | tred \
    > tmp-ldep.dot

dot -Tgif tmp-ldep.dot > $outdir/modules.gif
dot -Tcmapx tmp-ldep.dot > $outdir/modules.map

$thisdir/make-imagemap-html.sh \
    $outdir/modules.gif \
    $outdir/modules.map \
    > $outdir/modules.html

rm tmp-ldep.dot

echo "... done"

for d in ${srcdir}*; do
    if test -d $d; then

	echo "directory $d ..." ;

	stem=${d#${srcdir}};

	$thisdir/dot-ldep-internal.tcl \
	    $d $ldeps \
	    | tred \
	    > tmp-ldep-$stem.dot;

	dot -Tgif tmp-ldep-$stem.dot > $outdir/$stem.gif;
	dot -Tcmapx tmp-ldep-$stem.dot > $outdir/$stem.map;

	rm tmp-ldep-$stem.dot;

	$thisdir/make-imagemap-html.sh \
	    $outdir/$stem.gif \
	    $outdir/$stem.map \
	    > $outdir/$stem.html

	echo "... done";
    fi
done
