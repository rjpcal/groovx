#!/bin/sh
# \
    exec `dirname $0`/groovx "$0" "$@"

# Copyright (c) 2002-2004 Rob Peters <rjpeters at klab dot caltech dot edu>

# $Id$

# rseq == random sequence
namespace eval rseq {
    variable current
    variable next

    proc init {} {
	::srand [expr [clock seconds] / 2]

	set rseq::current [::rand 0.0 1.0]
	set rseq::next [::rand 0.0 1.0]
    }

    proc irange { rval min max } {
	return [expr int(floor(($rval * ($max - $min)) + $min))]
    }

    proc inext { min max } {
	set rseq::current $rseq::next
	while { $rseq::next == $rseq::current } {
	    set rseq::next [::rand 0.0 1.0]
	}
	return [rseq::irange $rseq::current $min $max]
    }

    proc ipeek { min max } {
	return [rseq::irange $rseq::next $min $max]
    }
}

namespace eval aux {

    variable FAST_RESIZE_CUTOFF 0

    proc msg { tag content } {
	puts [format "%20s: %s" $tag $content]
    }

    proc image_file_exists { fname } {
	if { ![file exists $fname] } { return 0 }
	if { [file size $fname] == 0 } {
	    aux::msg "empty file" $fname
	    file delete $fname
	    return 0
	}
	return 1
    }

    proc build_scaled_pixmap { fname size } {
	set px [new GxPixmap]
	-> $px purgeable 1

	set code [catch {
	    set fsize [file size $fname]
	    if { $fsize < $aux::FAST_RESIZE_CUTOFF } {
		set cmd "|anytopnm $fname | pnmscale -xysize $size"
		aux::msg "resize load" $fname
		set fd [open $cmd r]

		fconfigure $fd \
		    -encoding binary -translation {binary binary}

		-> $px loadImageStream $fd

		catch {close $fd}
	    } else {
		aux::msg "fast load" $fname
		-> $px loadImage $fname
		-> $px zoomTo $size
	    }
	} result]

	if { $code != 0 } {
	    aux::msg "load error" $fname
	    aux::msg "error detail" $result
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
		    aux::get_dir_contents $f 1 result
		}
	    } else {
		if { [aux::is_img_file $f] } {
		    lappend result $f
		}
	    }
	}

	return $result
    }
}

# pl == playlist
namespace eval pl {
    variable m_list_file
    variable m_list
    variable m_idx
    variable m_guess_next
    variable m_widget
    variable m_pixmap
    variable m_pixmap_cache
    variable m_transform
    variable m_message
    variable m_scene
    variable m_match
    variable m_purge_list
    variable m_mode
    variable m_ndeleted
    variable m_nshown
    variable m_nmissed
    variable m_last_spin

    proc init { argv widget } {
	set fname [lindex $argv end]

	set pl::m_match "*"

	set doRecurse 0
	set doFresh 0

	for {set i 0} {$i < [llength $argv]} {incr i} {
	    switch -- [lindex $argv $i] {
		-match {
		    set pl::m_match [lindex $argv [expr $i+1]]
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
	    aux::msg "rc file" ./.slideshowrc.tcl
	    uplevel #0 {source ./.slideshowrc.tcl}
	}

	if { [file isdirectory $fname] } {
	    set pl::m_list_file ${fname}/.playlist

	    if { $doFresh } {
		file delete $pl::m_list_file
	    }

	    if { ![file exists $pl::m_list_file] } {
		set newlist [list]
		aux::get_dir_contents $fname $doRecurse newlist
		set pl::m_list $newlist
		pl::save
	    }

	} else {
	    set pl::m_list_file $fname
	}

	set fd [open $pl::m_list_file r]
	set items [lrange [split [read $fd] "\n"] 0 end-1]
	close $fd

	set i 0
	set N [llength $items]
	if { [string equal $pl::m_match "*"] } {
	    set pl::m_list $items
	} else {
	    foreach item $items {
		incr i
		if { [string match $pl::m_match $item] } {
		    lappend pl::m_list $item
		}
		if { [expr $i % 100] == 0 } {
		    puts -nonewline "$i of $N ...\r"
		    flush stdout
		}
	    }
	}

	set pl::m_idx 0
	set pl::m_guess_next 1
	set pl::m_widget $widget
	set pl::m_pixmap [new GxPixmap]
	-> $pl::m_pixmap purgeable 1
	set pl::m_scene [new GxSeparator]
	set pl::m_transform [new GxTransform]
	set pl::m_message [new GxText]
	-> $pl::m_message font "vector"

	set pl::m_purge_list [list]

	rseq::init

	set pl::m_mode spinning
	set pl::m_ndeleted 0
	set pl::m_nshown 0
	set pl::m_nmissed 0
	set pl::m_last_spin 1
    }

    proc save {} {
	aux::msg "write playlist" $pl::m_list_file
	if { [file exists ${pl::m_list_file}.bkp] } {
	    file delete ${pl::m_list_file}.bkp
	}
	if { [file exists $pl::m_list_file] } {
	    file rename $pl::m_list_file ${pl::m_list_file}.bkp
	}
	set fd [open $pl::m_list_file w]
	puts $fd [join $pl::m_list "\n"]
	close $fd
    }

    proc spin { step } {
	set pl::m_idx \
	    [expr ($pl::m_idx + $step) % [llength $pl::m_list]]

	set pl::m_last_spin $step

	set guesstep $step
	if { $guesstep == 0 } { set guesstep 1 }

	set pl::m_guess_next \
	    [expr ($pl::m_idx + $guesstep) % [llength $pl::m_list]]

	set pl::m_mode "spinning"
    }

    proc jump { {oldlength -1 } {adjust 0} } {
	if { $oldlength == -1 } {
	    set oldlength [llength $pl::m_list]
	}
	set pl::m_idx [expr [rseq::inext 0 $oldlength] + $adjust]
	set pl::m_guess_next [rseq::ipeek 0 [llength $pl::m_list]]
	set pl::m_mode "jumping"
    }

    proc filename {} {
	return [lindex $pl::m_list $pl::m_idx]
    }

    proc status {} {
	return "([expr $pl::m_idx + 1] of [llength $pl::m_list]) [pl::filename]"
    }

    proc mode { m } { set pl::m_mode $m }

    proc remove_helper { do_purge } {
	set target [lindex $pl::m_list $pl::m_idx]
	aux::msg "hide file\[$pl::m_idx\]" $target
	if { $do_purge } {
	    lappend pl::m_purge_list $target
	}

	set oldlength [llength $pl::m_list]

	set pl::m_list [lreplace $pl::m_list $pl::m_idx $pl::m_idx]

	switch -- $pl::m_mode {
	    jumping {
		if { $pl::m_idx < $pl::m_guess_next } {
		    aux::msg "jump offset" -1
		    pl::jump $oldlength -1
		} else {
		    aux::msg "jump offset" 0
		    pl::jump $oldlength 0
		}
	    }
	    spinning {
		if { $pl::m_last_spin == -1 } {
		    pl::spin -1
		} else {
		    pl::spin 0
		}
	    }
	    default {
		if { $pl::m_last_spin == -1 } {
		    pl::spin -1
		} else {
		    pl::spin 0
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
	set N [llength $pl::m_purge_list]
	set n 0
	foreach f $pl::m_purge_list {
	    aux::msg "purging" "[incr n] of $N"
	    if { [llength [info proc ::slideshowDeleteHook]] > 0 } {
		aux::msg "deleteHook" $f
		::slideshowDeleteHook $f
	    } else {
		aux::msg "delete file" $f
		set dir [file dirname $f]
		set tail [file tail $f]
		set stubfile ${dir}/.${tail}.deleted
		set fd [open $stubfile w]
		close $fd
		file delete $f
	    }
	    incr pl::m_ndeleted

	    reshow 0
	}
	set pl::m_purge_list [list]
	pl::save
	aux::msg "files deleted" $pl::m_ndeleted
	aux::msg "files shown" $pl::m_nshown
	aux::msg "cache misses" $pl::m_nmissed
	aux::msg "percent kept" \
	    [format "%.2f%%" \
		 [expr 100 * \
		      (1.0-double($pl::m_ndeleted)/$pl::m_nshown)]]
    }

    proc shuffle {} {
	set pl::m_list [dlist::shuffle $pl::m_list [clock clicks]]
    }

    proc sort {} {
	set pl::m_list [lsort -dictionary $pl::m_list]
    }

    proc cachenext {} {

	set i $pl::m_guess_next
	set f [lindex $pl::m_list $i]
	while { ![aux::image_file_exists $f] } {
	    aux::msg "no such file\[$i\]" $f
	    set pl::m_list [lreplace $pl::m_list $i $i]
	    set i [expr $i % [llength $pl::m_list]]
	    set f [lindex $pl::m_list $i]
	}
	if { ![info exists pl::m_pixmap_cache($f)] } {

	    set pl::m_pixmap_cache($f) \
		[aux::build_scaled_pixmap $f \
		     [-> $pl::m_widget size]]

	    aux::msg "cache insert\[$i\]" $f
	} else {
	    aux::msg "cache exists\[$i\]" "$pl::m_pixmap_cache($f) $f"
	}
    }

    proc reshow { show_image } {
	-> $pl::m_widget allowRefresh 0

	-> $pl::m_scene removeAllChildren
	if { $show_image } {
	    -> $pl::m_scene addChild $pl::m_pixmap
	}
	-> $pl::m_scene addChild $pl::m_transform
	-> $pl::m_scene addChild $pl::m_message
	-> $pl::m_transform translation \
	    [-> [-> $pl::m_widget canvas] topLeft]
	-> $pl::m_message text "[pl::filename]\n[llength $pl::m_list] c\n[llength $pl::m_purge_list] p\n$pl::m_ndeleted d\n$pl::m_nshown s\n$pl::m_nmissed m"
	-> $pl::m_message strokeWidth 2
	-> $pl::m_message alignmentMode $GxShapeKit::NW_ON_CENTER

#	-> $pl::m_widget see $pl::m_pixmap
	-> $pl::m_widget see $pl::m_scene
	-> $pl::m_widget allowRefresh 1
    }

    proc show {} {
	set f [pl::filename]

	while { ![aux::image_file_exists $f] } {
	    aux::msg "no such file\[$pl::m_idx\]" $f
	    set pl::m_list [lreplace $pl::m_list $pl::m_idx $pl::m_idx]
	    set pl::m_idx [expr $pl::m_idx % [llength $pl::m_list]]
	    set f [pl::filename]
	}

	aux::msg "index" "$pl::m_idx of [llength $pl::m_list]"
	aux::msg "show file\[$pl::m_idx\]" $f

	set old $pl::m_pixmap
	if { [info exists pl::m_pixmap_cache($f)] \
		 && [GxPixmap::is $pl::m_pixmap_cache($f)] } {
	    set pl::m_pixmap $pl::m_pixmap_cache($f)
	    unset pl::m_pixmap_cache($f)
	    aux::msg "cache hit\[$pl::m_idx\]" $f
	} else {
	    aux::msg "cache miss\[$pl::m_idx\]" $f
	    incr pl::m_nmissed
	    set pl::m_pixmap \
		[aux::build_scaled_pixmap $f [-> $pl::m_widget size]]
	}

	pl::reshow 1

	delete $old

	update idletasks

	ObjDb::clear

	aux::msg "pixmaps" [GxPixmap::findAll]

	incr pl::m_nshown

	after idle pl::cachenext
    }

    proc rotate {angle} {
	set f [pl::filename]

	if { ![file exists ${f}.orig] } {
	    file copy $f ${f}.orig
	}

	file delete ${f}.tmp
	file rename $f ${f}.tmp

	exec jpegtran -rotate $angle -copy all ${f}.tmp > $f

	pl::show
    }
}

proc looper { delay } {
    after cancel $::LOOP_HANDLE
    set ::LOOP_HANDLE [after $delay "::looper 4000"]
    pl::spin 1
    pl::show
}

proc hide {} {
    pl::remove
    pl::save
    pl::show
}

proc spinInput {} {
    pl::spin [.f.spinner get]
    pl::show
    .f.spinner clear
    return 1
}

set LOOP_HANDLE 0

frame .f

button .f.loop -text "loop" -command "::looper 4000"
button .f.noloop -text "stop loop" -command {after cancel $::LOOP_HANDLE}

button .f.shuffler -text "shuffle" -command {pl::shuffle}
button .f.sorter -text "sort" -command {pl::sort}

button .f.hide -text "hide" -command hide

button .f.rot90 -text "rotate cw" -command {pl::rotate 90}
button .f.rot270 -text "rotate ccw" -command {pl::rotate 270}

bind Canvas <ButtonRelease> {pl::show}

pack .f.loop .f.noloop .f.shuffler .f.sorter \
    .f.hide .f.rot90 .f.rot270 \
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

pl::init $argv $t

-> $t bind <ButtonPress-1> {pl::spin -1; pl::show}
-> $t bind <ButtonPress-3> {pl::spin 1; pl::show}
-> $t bind <ButtonPress-2> {pl::jump; pl::show}

bind all <Key-Left> {pl::spin -1; pl::show}
bind all <Key-Right> {pl::spin 1; pl::show}
bind all <Key-Up> {pl::jump; pl::show}
bind all <Key-Down> {pl::remove; pl::show}
bind all <Key-e> {pl::remove_no_purge; pl::show}
bind all <Key-0> {pl::remove_no_purge; pl::show}
bind all <Shift-Key-Down> {pl::mode spinning; pl::remove; pl::show}
bind all <Key-Return> {pl::save}
bind all <Key-x> {pl::purge; pl::reshow 1}
bind all <Key-Escape> { -> [Toglet::current] setVisible 0; pl::purge; exit }

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

pl::show
