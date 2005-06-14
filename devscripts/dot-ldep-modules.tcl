#!/usr/bin/tclsh

# $Id$

# Script use to transform the output of devscripts/cdeps
# --output-ldep-raw into a format that is usable the graph-drawing
# tool 'dot'. See 'make ldeps.png'.

proc strip_qt_junk { dirname } {

    if { [regexp {^qt/(.*)} $dirname - stripped] } {
	return "qt"
    }

    return $dirname

    # could use this code if we want to show individual qt project
    # directories, but ignore the .ui and .moc junk
    if { 0 } {
	set tail [file tail $dirname]

	switch -exact -- $tail {
	    .ui {
		# strip the trailing ".ui"
		return [file dirname $dirname]
	    }
	    .moc {
		# strip the trailing ".moc"
		return [file dirname $dirname]
	    }
	}

	#default...
	return $dirname
    }
}

proc strip_src_pfx { dirname } {
    if { [regexp {^src/(.*)} $dirname - stripped] } {
	return $stripped
    }
    return $dirname
}

if { [llength $argv] == 0 } {
    set fd stdin
} elseif { [llength $argv] == 1 } {
    set fname [lindex $argv 0]
    set fd [open $fname "r"]
} else {
    puts stderr "usage: $argv ?filename?"
    puts stderr "\tFilters raw output from 'devscripts/cdeps --output-ldep-raw'"
    puts stderr "\tinto a format readable by 'dot'. If no 'filename' is given,"
    puts stderr "\tthen the program reads from stdin."
    exit 1
}

while { [gets $fd line] >= 0 } {

    set f1 [lindex $line 0]
    set f2 [lindex $line 1]

    set d1 [file dirname $f1]
    set d2 [file dirname $f2]

    set d1 [strip_src_pfx $d1]
    set d2 [strip_src_pfx $d2]

    set d1 [strip_qt_junk $d1]
    set d2 [strip_qt_junk $d2]

    if { ![string equal $d1 $d2] } {

	set DEPS([list $d1 $d2]) 1
    }
}

set out stdout

puts $out "digraph main {"
puts $out "\tgraph \[size=\"14,\"10\];"
puts $out "\tnode \[shape=box, fontname=\"Courier-Bold\", peripheries=2, color=salmon, fontcolor=black, style=filled\];"
puts $out "\tedge \[color=gray25\];"
foreach dep [array names DEPS] {
    set d1 [lindex $dep 0]
    set d2 [lindex $dep 1]
    puts $out "\t\"$d1\" -> \"$d2\";"
}
puts $out "}"

if { ![string equal $fd "stdin"] } {
    close $fd
}

