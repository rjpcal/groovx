#!/usr/bin/env groovx

# Copyright (c) 2002-2003 Rob Peters <rjpeters at klab dot caltech dot edu>

# $Id$

package require Iwidgets

itcl::class Playlist {
    private variable itsListFile
    private variable itsList
    private variable itsIdx
    private variable itsToglet
    private variable itsPixmap

    constructor { fname widget } {
	if { [file isdirectory $fname] } {
	    set itsListFile ${fname}/.playlist

	    if { ![file exists $itsListFile] } {
		set names [lsort -dictionary [glob ${fname}/*]]
		set itsList [list]
		foreach n $names {
		    lappend itsList $n
		}
		$this save
	    }
	    
	} else {
	    set itsListFile ${fname}
	}

	set fd [open $itsListFile r]
	set itsList [lrange [split [read $fd] "\n"] 0 end-1]
	close $fd
	
	set itsIdx 0
	set itsToglet $widget
	set itsPixmap [new GxPixmap]
    }

    public method save {} {
	puts "writing playlist"
	set fd [open $itsListFile w]
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

	-> $itsPixmap loadImage $f
	-> $itsPixmap zoomTo [-> $itsWidget size]

	-> $itsWidget see $itsPixmap
    }
}

proc min {a b} {
    if {$a < $b} { return $a }
    return $b
}

proc looper {} {
    after cancel $::LOOP_HANDLE
    set ::LOOP_HANDLE [after $::DELAY ::looper]
    spinPic 1
    $::PLAYLIST show
}

proc hide {} {
    $::PLAYLIST remove
    updateText
    $::PLAYLIST show
}

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

-> [Toglet::current] destroy

set t [new Toglet]

set PLAYLIST [Playlist PLAYLIST [lindex $argv end] $t]

set DELAY 4000

set LOOP_HANDLE 0

frame .f

button .f.loop -text "loop" -command looper
button .f.noloop -text "stop loop" -command {after cancel $::LOOP_HANDLE}

button .f.shuffler -text "shuffle" -command {$::PLAYLIST shuffle}
button .f.sorter -text "sort" -command {$::PLAYLIST sort}

button .f.hide -text "hide" -command hide

iwidgets::spinner .f.spinner -width 0 -fixed 0 \
    -validate blockInput -labelvariable FILENAME \
    -decrement {spinPic -1} -increment {spinPic 1}

bind Canvas <ButtonRelease> {$::PLAYLIST show}

pack .f.loop .f.noloop .f.shuffler .f.sorter .f.hide .f.spinner -side left -expand yes

pack .f -side top

-> $t bind <ButtonPress-1> {spinPic -1; $::PLAYLIST show}
-> $t bind <ButtonPress-3> {spinPic 1; $::PLAYLIST show}

-> $t height 975
-> $t width 1400

update

updateText
$::PLAYLIST show
