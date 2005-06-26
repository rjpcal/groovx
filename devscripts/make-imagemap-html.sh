#!/bin/sh

# $Id: make-imagemap-html.sh 4754 2005-06-14 23:33:05Z rjpeters $

# Script to build an html file that shows an image with an associated
# client-side imagemap (e.g. from a .gif and a .map generated by
# 'dot').

imgfile=$1
mapfile=$2

imgfile=${imgfile##*/}

stem=${imgfile%%.*}

echo "<html>"
echo "<head>"
echo "<title>$stem</title>"
echo "</head>"
echo "<body>"
echo "<img border=\"0\" src=\"$imgfile\" usemap=#$stem>"
cat $mapfile
echo "</body>"
echo "</html>"