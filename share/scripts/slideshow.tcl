#!/usr/bin/env groovx

# Copyright (c) 2002-2004 Rob Peters <rjpeters at klab dot caltech dot edu>

# $Id$

package require Iwidgets

set TMPCOUNTER 0

proc msg { tag content } {
    puts [format "%15s: %s" $tag $content]
}

proc split_lines { text linelength indent } {
}

proc build_scaled_pixmap { fname size } {
    set px [new GxPixmap]
    -> $px purgeable 1

    set cmd "|anytopnm $fname | pnmscale -xysize $size"
    msg "resize load" $fname
    set fd [open $cmd r]

    fconfigure $fd -encoding binary -translation {binary binary}

    -> $px loadImageStream $fd

    catch {close $fd}

    return $px
}

proc is_img_file { fname } {
    switch -- [file extension $fname] {
	.jpg -
	.jpeg -
	.JPG -
	.JPEG -
	.gif -
	.GIF -
	.pnm -
	.PNM -
	.png -
	.PNG {
	    return 1
	}
	default {
	    return 0
	}
    }
}

proc get_dir_contents { dirname recursive {varname ""} } {
    if { [string length $varname] == 0 } {
	set result [list]
    } else {
	upvar $varname result
    }

    puts -nonewline "getting contents of directory: ${dirname}\r"
    flush stdout

    foreach f [lsort -dictionary [glob -nocomplain ${dirname}/*]] {
	if { [file isdirectory $f] } {
	    if { $recursive != 0 } {
		get_dir_contents $f 1 result
	    }
	} else {
	    if { [is_img_file $f] } {
		lappend result $f
	    }
	}
    }

    return $result
}

itcl::class RandSeq {
    private variable current
    private variable next

    constructor {} {
	::srand [clock clicks]

	set current [::rand 0.0 1.0]
	set next [::rand 0.0 1.0]
    }

    public method inext { min max } {
	set current $next
	while { $next == $current } {
	    set next [::rand 0.0 1.0]
	}
	return [expr int(floor(($current * ($max - $min)) + $min))]
    }

    public method ipeek { min max } {
	return [expr int(floor(($next * ($max - $min)) + $min))]
    }
}

itcl::class Playlist {
    private variable itsListFile
    private variable itsList
    private variable itsIdx
    private variable itsGuessNext
    private variable itsWidget
    private variable itsPixmap
    private variable itsPixmapCache
    private variable itsMatch
    private variable itsRandSeq
    private variable itsPurgeList

    constructor { argv widget } {
	set fname [lindex $argv end]

	set itsMatch "*"

	set doRecurse 0
	set doFresh 0

	for {set i 0} {$i < [llength $argv]} {incr i} {
	    switch -- [lindex $argv $i] {
		-match {
		    set itsMatch [lindex $argv [expr $i+1]]
		}
		-recurse {
		    set doRecurse 1
		}
		-fresh {
		    set doFresh 1
		}
	    }
	}

	if { [file isdirectory $fname] } {
	    set itsListFile ${fname}/.playlist

	    if { $doFresh } {
		file delete $itsListFile
	    }

	    if { ![file exists $itsListFile] } {
		set newlist [list]
		get_dir_contents $fname $doRecurse newlist
		set itsList $newlist
		$this save
	    }

	} else {
	    set itsListFile $fname
	}

	set fd [open $itsListFile r]
	set items [lrange [split [read $fd] "\n"] 0 end-1]
	close $fd

	set i 0
	set N [llength $items]
	if { [string equal $itsMatch "*"] } {
	    set itsList $items
	} else {
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
	}

	set itsIdx 0
	set itsGuessNext 1
	set itsWidget $widget
	set itsPixmap [new GxPixmap]
	-> $itsPixmap purgeable 1

	set itsPurgeList [list]

	set itsRandSeq [RandSeq \#auto]
	::srand [clock clicks]
    }

    public method save {} {
	msg "write playlist" $itsListFile
	if { [file exists ${itsListFile}.bkp] } {
	    file delete ${itsListFile}.bkp
	}
	if { [file exists $itsListFile] } {
	    file rename $itsListFile ${itsListFile}.bkp
	}
	set fd [open $itsListFile w]
	puts $fd [join $itsList "\n"]
	close $fd
    }

    public method spin { step } {
	set itsIdx [expr ($itsIdx + $step) % [llength $itsList]]

	set guesstep $step
	if { $guesstep == 0 } { set guesstep 1 }

	set itsGuessNext [expr ($itsIdx + $guesstep) % [llength $itsList]]
    }

    public method jump {} {
	set itsIdx [$itsRandSeq inext 0 [llength $itsList]]
	set itsGuessNext [$itsRandSeq ipeek 0 [llength $itsList]]
    }

    public method filename {} {
	return [lindex $itsList $itsIdx]
    }

    public method status {} {
	return "([expr $itsIdx + 1] of [llength $itsList]) [$this filename]"
    }

    public method remove {} {
	set target [lindex $itsList $itsIdx]
	msg "hide file" $target
	lappend itsPurgeList $target
	set itsList [lreplace $itsList $itsIdx $itsIdx]
	$this spin 0
    }

    public method purge {} {
	puts "purging..."
	foreach f $itsPurgeList {
	    msg "delete file" $f
	    set dir [file dirname $f]
	    set tail [file tail $f]
	    set stubfile ${dir}/.${tail}.deleted
	    set fd [open $stubfile w]
	    close $fd
	    file delete $f
	}
	set itsPurgeList [list]
	save
    }

    public method shuffle {} {
	set itsList [dlist::shuffle $itsList [clock clicks]]
    }

    public method sort {} {
	set itsList [lsort -dictionary $itsList]
    }

    public method cachenext {} {

	set i $itsGuessNext
	set f [lindex $itsList $i]
	while { ![file exists $f] } {
	    msg "no such file" $f
	    set itsList [lreplace $itsList $i $i]
	    set i [expr $i % [llength $itsList]]
	    set f [lindex $itsList $i]
	}
	set itsPixmapCache($f) \
	    [build_scaled_pixmap $f [-> $itsWidget size]]
	msg "cache insert" $f
    }

    public method show {} {
	set f [$this filename]

	while { ![file exists $f] } {
	    msg "no such file" $f
	    set itsList [lreplace $itsList $itsIdx $itsIdx]
	    set itsIdx [expr $itsIdx % [llength $itsList]]
	    set f [$this filename]
	}

	msg "show file" $f

	set old $itsPixmap
	if { [info exists itsPixmapCache($f)] \
		 && [GxPixmap::is $itsPixmapCache($f)] } {
	    set itsPixmap $itsPixmapCache($f)
	    unset itsPixmapCache($f)
	    msg "cache hit" $f
	} else {
	    set itsPixmap [build_scaled_pixmap $f [-> $itsWidget size]]
	}

	-> $itsWidget allowRefresh 0
	-> $itsWidget see $itsPixmap
	-> $itsWidget allowRefresh 1

	delete $old

	update idletasks

	ObjDb::clear

	msg "pixmaps" [GxPixmap::findAll]

	after idle [itcl::code $this cachenext]
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
    $::PLAYLIST save
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

proc jumpPic {} {
    $::PLAYLIST jump
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
-> $t bind <ButtonPress-2> {jumpPic; PLAYLIST show}

-> $t bind <Key-Left> {spinPic -1; PLAYLIST show}
-> $t bind <Key-Right> {spinPic 1; PLAYLIST show}
-> $t bind <Key-Up> {jumpPic; PLAYLIST show}
-> $t bind <Key-Down> {PLAYLIST remove; updateText; PLAYLIST show}
-> $t bind <Key-Return> {PLAYLIST save}
-> $t bind <Key-x> {PLAYLIST purge}
-> $t bind <Key-Escape> {PLAYLIST purge; exit }

-> $t height 975
-> $t width 1400

-> $t repack "-side bottom"

glClearColor 0 0 0 0

update

updateText
PLAYLIST show
