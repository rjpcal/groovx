#!/bin/sh
# \
    exec `dirname $0`/groovx "$0" "$@"

# Copyright (c) 2002-2004 Rob Peters <rjpeters at klab dot caltech dot edu>

# $Id$

namespace eval slideshow {
    proc msg { tag content } {
	puts [format "%20s: %s" $tag $content]
    }
}

proc split_lines { text linelength indent } {
}

proc image_file_exists { fname } {
    if { ![file exists $fname] } { return 0 }
    if { [file size $fname] == 0 } {
	slideshow::msg "empty file" $fname
	file delete $fname
	return 0
    }
    return 1
}

set FAST_RESIZE_CUTOFF 0

proc build_scaled_pixmap { fname size } {
    set px [new GxPixmap]
    -> $px purgeable 1

    set code [catch {
	set fsize [file size $fname]
	if { $fsize < $::FAST_RESIZE_CUTOFF } {
	    set cmd "|anytopnm $fname | pnmscale -xysize $size"
	    slideshow::msg "resize load" $fname
	    set fd [open $cmd r]

	    fconfigure $fd -encoding binary -translation {binary binary}

	    -> $px loadImageStream $fd

	    catch {close $fd}
	} else {
	    slideshow::msg "fast load" $fname
	    -> $px loadImage $fname
	    -> $px zoomTo $size
	}
    } result]

    if { $code != 0 } {
	slideshow::msg "load error" $fname
	slideshow::msg "error detail" $result
	error "couldn't load image $fname"
    }

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

namespace eval randseq {
    variable current [::rand 0.0 1.0]
    variable next [::rand 0.0 1.0]

    proc init {} {
	::srand [expr [clock seconds] / 2]

	set randseq::current [::rand 0.0 1.0]
	set randseq::next [::rand 0.0 1.0]
    }

    proc inext { min max } {
	set randseq::current $randseq::next
	while { $randseq::next == $randseq::current } {
	    set randseq::next [::rand 0.0 1.0]
	}
	return [expr int(floor(($randseq::current * ($max - $min)) + $min))]
    }

    proc ipeek { min max } {
	return [expr int(floor(($randseq::next * ($max - $min)) + $min))]
    }
}

namespace eval playlist {
    variable itsListFile
    variable itsList
    variable itsIdx
    variable itsGuessNext
    variable itsWidget
    variable itsPixmap
    variable itsPixmapCache
    variable itsTransform
    variable itsMessage
    variable itsScene
    variable itsMatch
    variable itsPurgeList
    variable itsMode
    variable itsDelCount
    variable itsShowCount
    variable itsMissCount
    variable itsLastSpin

    proc init { argv widget } {
	set fname [lindex $argv end]

	set playlist::itsMatch "*"

	set doRecurse 0
	set doFresh 0

	for {set i 0} {$i < [llength $argv]} {incr i} {
	    switch -- [lindex $argv $i] {
		-match {
		    set playlist::itsMatch [lindex $argv [expr $i+1]]
		}
		-recurse {
		    set doRecurse 1
		}
		-fresh {
		    set doFresh 1
		}
	    }
	}

	if { [file exists ./.slideshowrc.tcl] } {
	    slideshow::msg "rc file" ./.slideshowrc.tcl
	    uplevel #0 {source ./.slideshowrc.tcl}
	}

	if { [file isdirectory $fname] } {
	    set playlist::itsListFile ${fname}/.playlist

	    if { $doFresh } {
		file delete $playlist::itsListFile
	    }

	    if { ![file exists $playlist::itsListFile] } {
		set newlist [list]
		get_dir_contents $fname $doRecurse newlist
		set playlist::itsList $newlist
		playlist::save
	    }

	} else {
	    set playlist::itsListFile $fname
	}

	set fd [open $playlist::itsListFile r]
	set items [lrange [split [read $fd] "\n"] 0 end-1]
	close $fd

	set i 0
	set N [llength $items]
	if { [string equal $playlist::itsMatch "*"] } {
	    set playlist::itsList $items
	} else {
	    foreach item $items {
		incr i
		if { [string match $playlist::itsMatch $item] } {
		    lappend playlist::itsList $item
		}
		if { [expr $i % 100] == 0 } {
		    puts -nonewline "$i of $N ...\r"
		    flush stdout
		}
	    }
	}

	set playlist::itsIdx 0
	set playlist::itsGuessNext 1
	set playlist::itsWidget $widget
	set playlist::itsPixmap [new GxPixmap]
	-> $playlist::itsPixmap purgeable 1
	set playlist::itsScene [new GxSeparator]
	set playlist::itsTransform [new GxTransform]
	set playlist::itsMessage [new GxText]
	-> $playlist::itsMessage font "vector"

	set playlist::itsPurgeList [list]

	randseq::init

	set playlist::itsMode spinning
	set playlist::itsDelCount 0
	set playlist::itsShowCount 0
	set playlist::itsMissCount 0
	set playlist::itsLastSpin 1
    }

    proc save {} {
	slideshow::msg "write playlist" $playlist::itsListFile
	if { [file exists ${playlist::itsListFile}.bkp] } {
	    file delete ${playlist::itsListFile}.bkp
	}
	if { [file exists $playlist::itsListFile] } {
	    file rename $playlist::itsListFile ${playlist::itsListFile}.bkp
	}
	set fd [open $playlist::itsListFile w]
	puts $fd [join $playlist::itsList "\n"]
	close $fd
    }

    proc spin { step } {
	set playlist::itsIdx [expr ($playlist::itsIdx + $step) % [llength $playlist::itsList]]

	set playlist::itsLastSpin $step

	set guesstep $step
	if { $guesstep == 0 } { set guesstep 1 }

	set playlist::itsGuessNext [expr ($playlist::itsIdx + $guesstep) % [llength $playlist::itsList]]

	set playlist::itsMode "spinning"
    }

    proc jump { {oldlength -1 } {adjust 0} } {
	if { $oldlength == -1 } {
	    set oldlength [llength $playlist::itsList]
	}
	set playlist::itsIdx [expr [randseq::inext 0 $oldlength] + $adjust]
	set playlist::itsGuessNext [randseq::ipeek 0 [llength $playlist::itsList]]
	set playlist::itsMode "jumping"
    }

    proc filename {} {
	return [lindex $playlist::itsList $playlist::itsIdx]
    }

    proc status {} {
	return "([expr $playlist::itsIdx + 1] of [llength $playlist::itsList]) [playlist::filename]"
    }

    proc mode { m } { set playlist::itsMode $m }

    proc remove_helper { do_purge } {
	set target [lindex $playlist::itsList $playlist::itsIdx]
	slideshow::msg "hide file\[$playlist::itsIdx\]" $target
	if { $do_purge } {
	    lappend playlist::itsPurgeList $target
	}
	set oldlength [llength $playlist::itsList]
	set playlist::itsList [lreplace $playlist::itsList $playlist::itsIdx $playlist::itsIdx]
	switch -- $playlist::itsMode {
	    jumping {
		if { $playlist::itsIdx < $playlist::itsGuessNext } {
		    slideshow::msg "jump offset" -1
		    playlist::jump $oldlength -1
		} else {
		    slideshow::msg "jump offset" 0
		    playlist::jump $oldlength 0
		}
	    }
	    spinning {
		if { $playlist::itsLastSpin == -1 } {
		    playlist::spin -1
		} else {
		    playlist::spin 0
		}
	    }
	    default {
		if { $playlist::itsLastSpin == -1 } {
		    playlist::spin -1
		} else {
		    playlist::spin 0
		}
	    }
	}
    }

    proc remove {} {
	remove_helper 1
    }

    proc remove_no_purge {} {
	remove_helper 0
    }

    proc purge {} {
	puts "purging..."
	set N [llength $playlist::itsPurgeList]
	set n 0
	foreach f $playlist::itsPurgeList {
	    slideshow::msg "purging" "[incr n] of $N"
	    if { [llength [info proc ::slideshowDeleteHook]] > 0 } {
		slideshow::msg "deleteHook" $f
		::slideshowDeleteHook $f
	    } else {
		slideshow::msg "delete file" $f
		set dir [file dirname $f]
		set tail [file tail $f]
		set stubfile ${dir}/.${tail}.deleted
		set fd [open $stubfile w]
		close $fd
		file delete $f
	    }
	    incr playlist::itsDelCount

	    reshow 0
	}
	set playlist::itsPurgeList [list]
	playlist::save
	slideshow::msg "files deleted" $playlist::itsDelCount
	slideshow::msg "files shown" $playlist::itsShowCount
	slideshow::msg "cache misses" $playlist::itsMissCount
	slideshow::msg "percent kept" [format "%.2f%%" \
				[expr 100 * (1.0 - double($playlist::itsDelCount)/$playlist::itsShowCount)]]
    }

    proc shuffle {} {
	set playlist::itsList [dlist::shuffle $playlist::itsList [clock clicks]]
    }

    proc sort {} {
	set playlist::itsList [lsort -dictionary $playlist::itsList]
    }

    proc cachenext {} {

	set i $playlist::itsGuessNext
	set f [lindex $playlist::itsList $i]
	while { ![image_file_exists $f] } {
	    slideshow::msg "no such file\[$i\]" $f
	    set playlist::itsList [lreplace $playlist::itsList $i $i]
	    set i [expr $i % [llength $playlist::itsList]]
	    set f [lindex $playlist::itsList $i]
	}
	if { ![info exists playlist::itsPixmapCache($f)] } {
	    set playlist::itsPixmapCache($f) [build_scaled_pixmap $f [-> $playlist::itsWidget size]]
	    slideshow::msg "cache insert\[$i\]" $f
	} else {
	    slideshow::msg "cache exists\[$i\]" "$playlist::itsPixmapCache($f) $f"
	}
    }

    proc reshow { show_image } {
	-> $playlist::itsWidget allowRefresh 0

	-> $playlist::itsScene removeAllChildren
	if { $show_image } {
	    -> $playlist::itsScene addChild $playlist::itsPixmap
	}
	-> $playlist::itsScene addChild $playlist::itsTransform
	-> $playlist::itsScene addChild $playlist::itsMessage
	-> $playlist::itsTransform translation [-> [-> $playlist::itsWidget canvas] topLeft]
	-> $playlist::itsMessage text "[playlist::filename]\n[llength $playlist::itsList] c\n[llength $playlist::itsPurgeList] p\n$playlist::itsDelCount d\n$playlist::itsShowCount s\n$playlist::itsMissCount m"
	-> $playlist::itsMessage strokeWidth 2
	-> $playlist::itsMessage alignmentMode $GxShapeKit::NW_ON_CENTER

#	-> $playlist::itsWidget see $playlist::itsPixmap
	-> $playlist::itsWidget see $playlist::itsScene
	-> $playlist::itsWidget allowRefresh 1
    }

    proc show {} {
	set f [playlist::filename]

	while { ![image_file_exists $f] } {
	    slideshow::msg "no such file\[$playlist::itsIdx\]" $f
	    set playlist::itsList [lreplace $playlist::itsList $playlist::itsIdx $playlist::itsIdx]
	    set playlist::itsIdx [expr $playlist::itsIdx % [llength $playlist::itsList]]
	    set f [playlist::filename]
	}

	slideshow::msg "index" "$playlist::itsIdx of [llength $playlist::itsList]"
	slideshow::msg "show file\[$playlist::itsIdx\]" $f

	set old $playlist::itsPixmap
	if { [info exists playlist::itsPixmapCache($f)] \
		 && [GxPixmap::is $playlist::itsPixmapCache($f)] } {
	    set playlist::itsPixmap $playlist::itsPixmapCache($f)
	    unset playlist::itsPixmapCache($f)
	    slideshow::msg "cache hit\[$playlist::itsIdx\]" $f
	} else {
	    slideshow::msg "cache miss\[$playlist::itsIdx\]" $f
	    incr playlist::itsMissCount
	    set playlist::itsPixmap [build_scaled_pixmap $f [-> $playlist::itsWidget size]]
	}

	playlist::reshow 1

	delete $old

	update idletasks

	ObjDb::clear

	slideshow::msg "pixmaps" [GxPixmap::findAll]

	incr playlist::itsShowCount

	after idle playlist::cachenext
    }

    proc rotate {angle} {
	set f [playlist::filename]

	if { ![file exists ${f}.orig] } {
	    file copy $f ${f}.orig
	}

	file delete ${f}.tmp
	file rename $f ${f}.tmp

	exec jpegtran -rotate $angle -copy all ${f}.tmp > $f

	playlist::show
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
    playlist::show
}

proc hide {} {
    playlist::remove
    playlist::save
    updateText
    playlist::show
}

proc blockInput {args} {
    return 0
}

proc spinInput {} {
    playlist::spin [.f.spinner get]
    updateText
    playlist::show
    .f.spinner clear
    return 1
}

proc spinPic {step} {
    playlist::spin $step

    updateText
}

proc jumpPic {} {
    playlist::jump
    updateText
}

proc updateText {} {
    set ::FILENAME [playlist::status]
}

set DELAY 4000

set LOOP_HANDLE 0

frame .f

button .f.loop -text "loop" -command looper
button .f.noloop -text "stop loop" -command {after cancel $::LOOP_HANDLE}

button .f.shuffler -text "shuffle" -command {playlist::shuffle}
button .f.sorter -text "sort" -command {playlist::sort}

button .f.hide -text "hide" -command hide

button .f.rot90 -text "rotate cw" -command {playlist::rotate 90}
button .f.rot270 -text "rotate ccw" -command {playlist::rotate 270}

bind Canvas <ButtonRelease> {playlist::show}

pack .f.loop .f.noloop .f.shuffler .f.sorter .f.hide .f.rot90 .f.rot270 \
    -side left -expand no

set show_buttons [expr ![string equal [exec uname] "Darwin"]]

if { $show_buttons } {
    pack .f -side top -anchor nw -expand no
}

-> [Toglet::current] destroy

set t [new Toglet]
set c [new GxFixedScaleCamera]
-> $c pixelsPerUnit 8
-> $t camera $c

playlist::init $argv $t

-> $t bind <ButtonPress-1> {spinPic -1; playlist::show}
-> $t bind <ButtonPress-3> {spinPic 1; playlist::show}
-> $t bind <ButtonPress-2> {jumpPic; playlist::show}

bind all <Key-Left> {spinPic -1; playlist::show}
bind all <Key-Right> {spinPic 1; playlist::show}
bind all <Key-Up> {jumpPic; playlist::show}
bind all <Key-Down> {playlist::remove; updateText; playlist::show}
bind all <Key-e> {playlist::remove_no_purge; updateText; playlist::show}
bind all <Key-0> {playlist::remove_no_purge; updateText; playlist::show}
bind all <Shift-Key-Down> {playlist::mode spinning; playlist::remove; updateText; playlist::show}
bind all <Key-Return> {playlist::save}
bind all <Key-x> {playlist::purge; playlist::reshow 1}
bind all <Key-Escape> { -> [Toglet::current] setVisible 0; playlist::purge; exit }

if { $show_buttons } {
    -> $t height [expr [winfo screenheight .] - 100]
} else {
    -> $t height [expr [winfo screenheight .] - 50]
}
-> $t width [expr [winfo screenwidth .] - 10]

-> $t repack "-side bottom"

glClearColor 0 0 0 0
glColor 0.7 0.7 0.1 1

update

updateText
playlist::show
