#!/usr/bin/tclsh

# $Id: dot-ldep-internal.tcl 4890 2005-06-23 22:40:40Z rjpeters $

# Script use to transform the output of devscripts/cdeps
# --output-ldep-raw into a format that is usable the graph-drawing
# tool 'dot'. This particular script extracts only the dependencies
# within a particular src subdirectory, and illustrates dependencies
# on external directories with a single graph edge. See 'make
# ldep-internals'.

proc strip_src_pfx { dirname } {
    if { [regexp {^src/(.*)} $dirname - stripped] } {
	return $stripped
    }
    return $dirname
}

if { [llength $argv] != 2 } {
    puts stderr "usage: $argv dirpfx ?filename?"
    puts stderr "\tFilters raw output from 'devscripts/cdeps --output-ldep-raw'"
    puts stderr "\tinto a format readable by 'dot'. If no 'filename' is given,"
    puts stderr "\tthen the program reads from stdin."
    exit 1
}

set dirpfx [string trimright [lindex $argv 0] "/"]

set fname [lindex $argv 1]
set fd [open $fname "r"]

set allowsub 1
if { [string equal $dirpfx "src"] } {

    # if we're examining dependencies in src itself, then don't
    # consider subdirectories of src (since we'll be handling those in
    # a separate run of this script)
    set allowsub 0
}

while { [gets $fd line] >= 0 } {

    set f1 [lindex $line 0]
    set f2 [lindex $line 1]

    set s1 [strip_src_pfx $f1]
    set s2 [strip_src_pfx $f2]

    if { !$allowsub } {
	if { [regexp "/" $s1] } {
	    continue
	}
    }

    if { [string match "${dirpfx}*" $f1] } {
	if { ($allowsub && [string match "${dirpfx}*" $f2]) \
		 || (![regexp "/" $s2]) } {

	    set DEPS([list $s1 $s2]) 1

	} else {

	    set extdir [file dirname $s2]

	    set EXTDEPS([list $s1 $extdir]) 1

	    set EXTDIRS($extdir) 1
	}
    }
}

set out stdout

set graphname [strip_src_pfx $dirpfx]

# Hard-coded values for which way to run the graph... most directories
# look best running top->bottom, but a few are really wide
# (e.g. Channels with lots of sibling channel classes) and so look
# better left->right.

set RANKDIR(AppDevices) RL
set RANKDIR(AppMedia) RL
set RANKDIR(AppNeuro) RL
set RANKDIR(BeoSub) RL
set RANKDIR(Beobot) RL
set RANKDIR(Channels) RL
set RANKDIR(Devices) RL
set RANKDIR(Gist) RL
set RANKDIR(INVT) RL
set RANKDIR(IRoom) RL
set RANKDIR(Neuro) RL
set RANKDIR(Parallel) RL
set RANKDIR(Psycho) RL
set RANKDIR(TestSuite) RL
set RANKDIR(Util) RL
set RANKDIR(VFAT) RL
set RANKDIR(qt) RL

if { [info exists RANKDIR($graphname)] } {
    set rankdir $RANKDIR($graphname)
} else {
    set rankdir TB
}

set ext_bordercolor "royalblue3"
set ext_fillcolor   "lightskyblue1"
set ext_edgecolor   "gray60"

set int_bordercolor "saddlebrown"
set int_fillcolor   "sandybrown"
set int_edgecolor   "saddlebrown"

puts $out "digraph $graphname {"
puts $out "\tgraph \[rankdir=$rankdir, concentrate=true\];"
puts $out "\tnode \[shape=box, height=0.25, fontsize=9,x fontname=\"Helvetica-Bold\", peripheries=1, color=$int_bordercolor, fillcolor=$int_fillcolor, fontcolor=black, style=\"bold,filled\"\];"
foreach dep [array names DEPS] {
    set f1 [lindex $dep 0]
    set f2 [lindex $dep 1]
    puts $out "\t\"$f1\" -> \"$f2\" \[color=$int_bordercolor\];"
}
foreach dir [array names EXTDIRS] {
    puts $out "\t\"$dir\" \[URL=\"${dir}.html\", label=\"\[ext\] $dir\", peripheries=2, color=$ext_bordercolor, fillcolor=$ext_fillcolor, fontcolor=black, shape=box, peripheries=2, style=\"bold,filled\"\];"
}
puts -nonewline $out "\t{ rank = sink; "
foreach dir [array names EXTDIRS] {
    puts -nonewline "\"$dir\"; "
}
puts $out "}"
foreach dep [array names EXTDEPS] {
    set f1 [lindex $dep 0]
    set d2 [lindex $dep 1]
    puts $out "\t\"$f1\" -> \"$d2\" \[color=$ext_edgecolor, style=dashed\];"
}
puts $out "}"

close $fd
