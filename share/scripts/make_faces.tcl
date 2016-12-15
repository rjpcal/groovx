#!/usr/bin/env groovx

# A little sample script that makes a bunch of Face objects, renders each
# of them in an OpenGL window, then grabs the bitmap data out of the window
# into a GxPixmap object, which in turn is used to save the image to a
# portable bitmap (PBM) file.

# Rob Peters <https://github.com/rjpcal/>

Togl::width 128
Togl::height 128

set cam [new GxFixedScaleCamera]
-> $cam pixelsPerUnit 48
Togl::camera $cam

update

file mkdir gridded3

set basename gridded3/gridded3

set img [new GxPixmap]

set counter 0

foreach eh { 0.25 0.40 0.55 } {
    foreach es { 0.35 0.55 0.75 } {
	foreach nl { 0.35 0.50 0.65 } {
	    foreach mh { -0.50 -0.65 -0.80 } {
		set f [new Face]
		-> $f eyeHeight $eh
		-> $f eyeDistance $es
		-> $f noseLength $nl
		-> $f mouthHeight $mh

		see $f

		-> $img grabScreenRect {0 128 128 0}

		set img_name [format "%s_%03d_%+4.2f_%+4.2f_%+4.2f_%+4.2f.pbm" \
				  $basename $counter $eh $es $nl $mh ]

		-> $img saveImage $img_name

		incr counter
	    }
	}
    }
}

exit
