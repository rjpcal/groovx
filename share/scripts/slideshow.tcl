#!/usr/bin/env groovx

# Copyright (c) 2002-2004 Rob Peters <rjpeters at klab dot caltech dot edu>

# $Id$

package require Iwidgets

itcl::class Playlist {
    private variable itsListFile
    private variable itsList
    private variable itsIdx
    private variable itsWidget
    private variable itsPixmap
    private variable itsMatch

    constructor { argv widget } {
	set fname [lindex $argv end]

	set itsMatch "*"

	for {set i 0} {$i < [llength $argv]} {incr i} {
	    switch -- [lindex $argv $i] {
		-match {
		    set itsMatch [lindex $argv [expr $i+1]]
		}
	    }
	}

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
	set items [lrange [split [read $fd] "\n"] 0 end-1]
	close $fd

	set i 0
	set N [llength $items]
	foreach item $items {
	    incr i
	    if { [string match $itsMatch $item] } {
		lappend itsList $item
	    }
	    if { [expr $i % 100] == 0 } {
		puts -nonewline "$i of $N ...\r"
		flush stdout
	    }
	}

	set itsIdx 0
	set itsWidget $widget
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

	-> $itsWidget allowRefresh 0

	-> $itsPixmap loadImage $f
	-> $itsPixmap zoomTo [-> $itsWidget size]

	-> $itsWidget see $itsPixmap

	-> $itsWidget allowRefresh 1
    }

    public method rotate {angle} {
	set f [$this filename]

	if { ![file exists ${f}.orig] } {
	    file copy $f ${f}.orig
	}

	file delete ${f}.tmp
	file rename $f ${f}.tmp

	exec jpegtran -rotate $angle -copy all ${f}.tmp > $f

	$this show
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

proc spinInput {} {
    $::PLAYLIST spin [.f.spinner get]
    updateText
    $::PLAYLIST show
    .f.spinner clear
    return 1
}

proc spinPic {step} {
    $::PLAYLIST spin $step

    updateText
}

proc updateText {} {
    set ::FILENAME [$::PLAYLIST status]
}

set DELAY 4000

set LOOP_HANDLE 0

# PLAYLIST is the name of the global Playlist object

frame .f

button .f.loop -text "loop" -command looper
button .f.noloop -text "stop loop" -command {after cancel $::LOOP_HANDLE}

button .f.shuffler -text "shuffle" -command {PLAYLIST shuffle}
button .f.sorter -text "sort" -command {PLAYLIST sort}

button .f.hide -text "hide" -command hide

button .f.rot90 -text "rotate cw" -command {PLAYLIST rotate 90}
button .f.rot270 -text "rotate ccw" -command {PLAYLIST rotate 270}

iwidgets::spinner .f.spinner -width 0 -fixed 0 \
    -command spinInput  -labelvariable FILENAME \
    -decrement {spinPic -1} -increment {spinPic 1}

bind Canvas <ButtonRelease> {PLAYLIST show}

pack .f.loop .f.noloop .f.shuffler .f.sorter .f.hide .f.rot90 .f.rot270 \
    .f.spinner \
    -side left -expand yes

pack .f -side top

-> [Toglet::current] destroy

set t [new Toglet]

set PLAYLIST [Playlist PLAYLIST $argv $t]

-> $t bind <ButtonPress-1> {spinPic -1; PLAYLIST show}
-> $t bind <ButtonPress-3> {spinPic 1; PLAYLIST show}

-> $t height 975
-> $t width 1400

-> $t repack "-side bottom"

update

updateText
PLAYLIST show
