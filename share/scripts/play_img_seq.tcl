#!/usr/bin/env groovx

# Copyright (c) 2004-2005
# Rob Peters <rjpeters at klab dot caltech dot edu>


if { [llength $argv] == 0 } {
    puts "usage: [file tail $argv0] <image filename(s)>..."
    puts ""
    puts "       shows a sequence of image files as a movie"
    exit 1
}

set x [new GxPixmap]

-> $x loadImage [lindex $argv 0]

set sz [-> $x size]

set geom [join $sz "x"]

wm geometry . $geom

update

see $x

foreach fname $argv {
    -> $x loadImage $fname
}
