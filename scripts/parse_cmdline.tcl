# Tcl functions for parsing command-line options
#
# Copyright (c) 2002-2005
# Rob Peters rjpeters at klab dot caltech dot edu
#
# $Revision$

proc parse_cmdline { argv } {

    for {set i 0} {$i < [llength $argv]} {incr i} {

	set arg [lindex $argv $i]

	if { [string equal "--" [string range $arg 0 1]] } {
	    set name [string toupper [string range $arg 2 end]]

	    lappend ::${name} [lindex $argv [incr i]]
	}
    }
}
