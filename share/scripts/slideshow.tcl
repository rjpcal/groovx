#!/bin/sh
# \
    exec `dirname $0`/groovx "$0" "$@"

# Copyright (c) 2002-2004 Rob Peters <rjpeters at klab dot caltech dot edu>

# $Id$

package require Iwidgets

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

itcl::class RandSeq {
    private variable current
    private variable next

    constructor {} {
	::srand [expr [clock clicks] / 2]

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
    private variable itsTransform
    private variable itsMessage
    private variable itsScene
    private variable itsMatch
    private variable itsRandSeq
    private variable itsPurgeList
    private variable itsMode
    private variable itsDelCount
    private variable itsShowCount
    private variable itsMissCount
    private variable itsLastSpin

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

	if { [file exists ./.slideshowrc.tcl] } {
	    slideshow::msg "rc file" ./.slideshowrc.tcl
	    uplevel #0 {source ./.slideshowrc.tcl}
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
	set itsScene [new GxSeparator]
	set itsTransform [new GxTransform]
	set itsMessage [new GxText]
	-> $itsMessage font "vector"

	set itsPurgeList [list]

	set itsRandSeq [RandSeq \#auto]

	set itsMode spinning
	set itsDelCount 0
	set itsShowCount 0
	set itsMissCount 0
	set itsLastSpin 1
    }

    public method save {} {
	slideshow::msg "write playlist" $itsListFile
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

	set itsLastSpin $step

	set guesstep $step
	if { $guesstep == 0 } { set guesstep 1 }

	set itsGuessNext [expr ($itsIdx + $guesstep) % [llength $itsList]]

	set itsMode "spinning"
    }

    public method jump { {oldlength -1 } {adjust 0} } {
	if { $oldlength == -1 } {
	    set oldlength [llength $itsList]
	}
	set itsIdx [expr [$itsRandSeq inext 0 $oldlength] + $adjust]
	set itsGuessNext [$itsRandSeq ipeek 0 [llength $itsList]]
	set itsMode "jumping"
    }

    public method filename {} {
	return [lindex $itsList $itsIdx]
    }

    public method status {} {
	return "([expr $itsIdx + 1] of [llength $itsList]) [$this filename]"
    }

    public method mode { m } { set itsMode $m }

    public method remove_helper { do_purge } {
	set target [lindex $itsList $itsIdx]
	slideshow::msg "hide file\[$itsIdx\]" $target
	if { $do_purge } {
	    lappend itsPurgeList $target
	}
	set oldlength [llength $itsList]
	set itsList [lreplace $itsList $itsIdx $itsIdx]
	switch -- $itsMode {
	    jumping {
		if { $itsIdx < $itsGuessNext } {
		    slideshow::msg "jump offset" -1
		    $this jump $oldlength -1
		} else {
		    slideshow::msg "jump offset" 0
		    $this jump $oldlength 0
		}
	    }
	    spinning {
		if { $itsLastSpin == -1 } {
		    $this spin -1
		} else {
		    $this spin 0
		}
	    }
	    default {
		if { $itsLastSpin == -1 } {
		    $this spin -1
		} else {
		    $this spin 0
		}
	    }
	}
    }

    public method remove {} {
	remove_helper 1
    }

    public method remove_no_purge {} {
	remove_helper 0
    }

    public method purge {} {
	puts "purging..."
	set N [llength $itsPurgeList]
	set n 0
	foreach f $itsPurgeList {
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
	    incr itsDelCount

	    reshow 0
	}
	set itsPurgeList [list]
	save
	slideshow::msg "files deleted" $itsDelCount
	slideshow::msg "files shown" $itsShowCount
	slideshow::msg "cache misses" $itsMissCount
	slideshow::msg "percent kept" [format "%.2f%%" \
				[expr 100 * (1.0 - double($itsDelCount)/$itsShowCount)]]
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
	while { ![image_file_exists $f] } {
	    slideshow::msg "no such file\[$i\]" $f
	    set itsList [lreplace $itsList $i $i]
	    set i [expr $i % [llength $itsList]]
	    set f [lindex $itsList $i]
	}
	if { ![info exists itsPixmapCache($f)] } {
	    set itsPixmapCache($f) [build_scaled_pixmap $f [-> $itsWidget size]]
	    slideshow::msg "cache insert\[$i\]" $f
	} else {
	    slideshow::msg "cache exists\[$i\]" "$itsPixmapCache($f) $f"
	}
    }

    public method reshow { show_image } {
	-> $itsWidget allowRefresh 0

	-> $itsScene removeAllChildren
	if { $show_image } {
	    -> $itsScene addChild $itsPixmap
	}
	-> $itsScene addChild $itsTransform
	-> $itsScene addChild $itsMessage
	-> $itsTransform translation [-> [-> $itsWidget canvas] topLeft]
	-> $itsMessage text "[$this filename]\n[llength $itsList] c\n[llength $itsPurgeList] p\n$itsDelCount d\n$itsShowCount s\n$itsMissCount m"
	-> $itsMessage strokeWidth 2
	-> $itsMessage alignmentMode $GxShapeKit::NW_ON_CENTER

#	-> $itsWidget see $itsPixmap
	-> $itsWidget see $itsScene
	-> $itsWidget allowRefresh 1
    }

    public method show {} {
	set f [$this filename]

	while { ![image_file_exists $f] } {
	    slideshow::msg "no such file\[$itsIdx\]" $f
	    set itsList [lreplace $itsList $itsIdx $itsIdx]
	    set itsIdx [expr $itsIdx % [llength $itsList]]
	    set f [$this filename]
	}

	slideshow::msg "index" "$itsIdx of [llength $itsList]"
	slideshow::msg "show file\[$itsIdx\]" $f

	set old $itsPixmap
	if { [info exists itsPixmapCache($f)] \
		 && [GxPixmap::is $itsPixmapCache($f)] } {
	    set itsPixmap $itsPixmapCache($f)
	    unset itsPixmapCache($f)
	    slideshow::msg "cache hit\[$itsIdx\]" $f
	} else {
	    slideshow::msg "cache miss\[$itsIdx\]" $f
	    incr itsMissCount
	    set itsPixmap [build_scaled_pixmap $f [-> $itsWidget size]]
	}

	$this reshow 1

	delete $old

	update idletasks

	ObjDb::clear

	slideshow::msg "pixmaps" [GxPixmap::findAll]

	incr itsShowCount

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
    -side left -expand no

set show_buttons [expr ![string equal [exec /usr/bin/uname] "Darwin"]]

if { $show_buttons } {
    pack .f -side top -anchor nw -expand no
}

-> [Toglet::current] destroy

set t [new Toglet]
set c [new GxFixedScaleCamera]
-> $c pixelsPerUnit 8
-> $t camera $c

set PLAYLIST [Playlist PLAYLIST $argv $t]

-> $t bind <ButtonPress-1> {spinPic -1; PLAYLIST show}
-> $t bind <ButtonPress-3> {spinPic 1; PLAYLIST show}
-> $t bind <ButtonPress-2> {jumpPic; PLAYLIST show}

bind all <Key-Left> {spinPic -1; PLAYLIST show}
bind all <Key-Right> {spinPic 1; PLAYLIST show}
bind all <Key-Up> {jumpPic; PLAYLIST show}
bind all <Key-Down> {PLAYLIST remove; updateText; PLAYLIST show}
bind all <Key-e> {PLAYLIST remove_no_purge; updateText; PLAYLIST show}
bind all <Key-0> {PLAYLIST remove_no_purge; updateText; PLAYLIST show}
bind all <Shift-Key-Down> {PLAYLIST mode spinning; PLAYLIST remove; updateText; PLAYLIST show}
bind all <Key-Return> {PLAYLIST save}
bind all <Key-x> {PLAYLIST purge; PLAYLIST reshow 1}
bind all <Key-Escape> { -> [Toglet::current] setVisible 0; PLAYLIST purge; exit }

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
PLAYLIST show
