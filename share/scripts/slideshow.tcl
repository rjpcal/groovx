#!/usr/bin/env groovx

# Copyright (c) 2002-2002 Rob Peters <rjpeters@klab.caltech.edu>

# $Id$

package require Iwidgets

Togl::destroy

itcl::class Playlist {
    private variable itsDir
    private variable itsList
    private variable itsIdx
    private variable itsPrev

    constructor { dir } {
	set itsDir $dir
	if { ![file exists ${dir}/.playlist] } {
	    set names [lsort -dictionary [glob ${dir}/*]]
	    set itsList [list]
	    foreach n $names {
		lappend itsList $n
	    }
	    $this save
	} else {
	    set fd [open ${dir}/.playlist r]
	    set itsList [lrange [split [read $fd] "\n"] 0 end-1]
	    close $fd
	}
	set itsIdx 0
	set itsPrev 0
    }

    public method save {} {
	puts "writing playlist"
	set fd [open ${itsDir}/.playlist w]
	puts $fd [join $itsList "\n"]
	close $fd
    }

    public method spin { step } {
	incr itsIdx $step

	if { $itsIdx < 0 } { set itsIdx [expr [llength $itsList]-1] }

	if { $itsIdx >= [llength $itsList] } { set itsIdx 0 }
    }

    public method filename {} {
	return [lindex $itsList $itsIdx]
    }

    public method status {} {
	return "([expr $itsIdx + 1] of [llength $itsList]) [$this filename]"
    }

    public method remove {} {
	set itsList [lreplace $itsList $itsIdx $itsIdx]
	$this save
	$this spin 0
    }

    public method shuffle {} {
	set itsList [dlist::shuffle $itsList]
    }

    public method sort {} {
	set itsList [lsort -dictionary $itsList]
    }

    public method show {} {
	set f [$this filename]

	puts $f

	set b [new GxPixmap]
	-> $b loadImage $f
	-> $b zoomTo [Togl::size]

	see $b

	delete $itsPrev
	set itsPrev $b
    }
}

set PLAYLIST [Playlist PLAYLIST [lindex $argv end]]

set DELAY 4000

set LOOP_HANDLE 0

proc min {a b} {
    if {$a < $b} { return $a }
    return $b
}

proc looper {} {
    set ::LOOP_HANDLE [after $::DELAY ::looper]
    spinPic 1
    $::PLAYLIST show
}

proc hide {} {
    $::PLAYLIST remove
    updateText
    $::PLAYLIST show
}

frame .f

button .f.loop -text "loop" -command looper
button .f.noloop -text "stop loop" -command {after cancel $::LOOP_HANDLE}

button .f.shuffler -text "shuffle" -command {$::PLAYLIST shuffle}
button .f.sorter -text "sort" -command {$::PLAYLIST sort}

button .f.hide -text "hide" -command hide

proc blockInput {args} {
    return 0
}

proc spinPic {step} {
    $::PLAYLIST spin $step

    updateText
}

proc updateText {} {
    set ::FILENAME [$::PLAYLIST status]
}

iwidgets::spinner .f.spinner -width 0 -fixed 0 \
    -validate blockInput -labelvariable FILENAME \
    -decrement {spinPic -1} -increment {spinPic 1}

bind Canvas <ButtonRelease> {$::PLAYLIST show}

pack .f.loop .f.noloop .f.shuffler .f.sorter .f.hide .f.spinner -side left -expand yes

pack .f -side top

Toglet::currentToglet [new Toglet]

Togl::bind <ButtonPress-1> {spinPic -1; $::PLAYLIST show}
Togl::bind <ButtonPress-3> {spinPic 1; $::PLAYLIST show}

Togl::height 975
Togl::width 1400

updateText
$::PLAYLIST show
