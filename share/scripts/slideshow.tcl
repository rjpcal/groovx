#!/usr/bin/env groovx

# Copyright (c) 2002-2002 Rob Peters <rjpeters@klab.caltech.edu>

# $Id$

package require Iwidgets

Togl::destroy

set files [lsort -dictionary [glob ${argv}/*]]

set prev 0

set IDX 0

proc min {a b} {
    if {$a < $b} { return $a }
    return $b
}

frame .f

button .f.loop -text "loop" -command looper
button .f.noloop -text "stop loop" -command {after cancel $::AFTER}

proc blockInput {args} {
    return 0
}

set WAITER 0

proc spinPic {step} {
    incr ::IDX $step

    if { $::IDX < 0 } { set ::IDX [expr [llength $::files]-1] }

    if { $::IDX >= [llength $::files] } { set ::IDX 0 }

    updateText
}

iwidgets::spinner .f.spinner -width 5 -fixed 5 \
    -validate blockInput -labelvariable FILENAME \
    -decrement {spinPic -1} -increment {spinPic 1}

.f.spinner insert 0 $::IDX

bind Canvas <ButtonRelease> {showPic}

pack .f.loop .f.noloop .f.spinner -side left -expand yes

pack .f -side top

Toglet::currentToglet [new Toglet]

Togl::bind <ButtonPress-1> {spinPic -1; showPic}
Togl::bind <ButtonPress-3> {spinPic 1; showPic}

Togl::height 975
Togl::width 1400

set prev 0

proc updateText {} {

    set f [lindex $::files $::IDX]

    .f.spinner delete 0 end
    .f.spinner insert 0 $::IDX

    set ::FILENAME "([expr $::IDX + 1] of [llength $::files]) $f"
}

proc showPic {} {
    set f [lindex $::files $::IDX]

    puts $f
    set b [new GxPixmap]
    -> $b loadImage $f
    set s [-> $b size]
    puts $s
    set ratio [min \
		   [expr ([Togl::width]-10) / double([lindex $s 0])]  \
		   [expr ([Togl::height]-10) / double([lindex $s 1]) ]]
    set ratio [min $ratio 1.0]
    puts $ratio
    -> $b usingZoom 1
    -> $b zoom "$ratio $ratio"
    see $b
    delete $::prev
    set ::prev $b
}

set DELAY 3000

set AFTER 0

proc looper {} {
    set ::AFTER [after $::DELAY ::looper]
    spinPic 1
    showPic
}

updateText
showPic
