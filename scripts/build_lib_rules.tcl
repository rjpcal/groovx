#!/usr/bin/env tclsh

set LIBDIR ""
set LIBPREFIX ""
set LIBEXT ""
set SRCROOT ""
set OBJROOT ""
set OBJEXT ""
set MODULES [list]

set PROJECT_LIBS [list]

proc process_module { module } {
    set srcdir [file join $::SRCROOT [string tolower $module]]
    set objdir [file join $::OBJROOT [string tolower $module]]

    set libname ${::LIBDIR}/${::LIBPREFIX}${module}${::LIBEXT}

    lappend ::PROJECT_LIBS $libname

    set srcfiles [glob ${srcdir}/*.cc ${srcdir}/*.C ${srcdir}/*.c]

    foreach f $srcfiles {
	set stem [file rootname [file tail $f]]
	set objfile ${objdir}/${stem}${::OBJEXT}
	lappend ::LIBRULES($libname) $objfile
    }
}

proc print_makefile_output { } {

    foreach lib [array names ::LIBRULES] {
	puts -nonewline "${lib}: \\\n  "
	puts [join $::LIBRULES($lib) " \\\n  "]
    }

    puts -nonewline "PROJECT_LIBS := \\\n  "
    puts [join $::PROJECT_LIBS " \\\n  "]
}

proc run { argv } {

    for {set i 0} {$i < [llength $argv]} { incr i } {
	set arg [lindex $argv $i]

	if { [string equal $arg "--libdir"] } {
	    set ::LIBDIR [lindex $argv [incr i]]

	} elseif { [string equal $arg "--libprefix"] } {
	    set ::LIBPREFIX [lindex $argv [incr i]]

	} elseif { [string equal $arg "--libext"] } {
	    set ::LIBEXT [lindex $argv [incr i]]

	} elseif { [string equal $arg "--srcroot"] } {
	    set ::SRCROOT [lindex $argv [incr i]]

	} elseif { [string equal $arg "--objroot"] } {
	    set ::OBJROOT [lindex $argv [incr i]]

	} elseif { [string equal $arg "--objext"] } {
	    set ::OBJEXT [lindex $argv [incr i]]

	} elseif { [string equal $arg "--module"] } {
	    lappend ::MODULES [lindex $argv [incr i]]
	}
    }

    foreach module $::MODULES {
	process_module $module
    }

    print_makefile_output
}

run $argv
