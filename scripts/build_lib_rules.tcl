#!/usr/bin/env tclsh

# $Id$

set LIBDIR ""
set LIBPREFIX ""
set LIBEXT ""
set SRCROOT ""
set OBJROOT ""
set OBJEXT ""
set MODULES ""

set PROJECT_LIBS [list]

set PROJECT_OBJS [list]

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
	lappend ::PROJECT_OBJS $objfile
    }
}

proc print_makefile_output { } {

    foreach lib [array names ::LIBRULES] {
	puts -nonewline "${lib}:   "
	puts [join $::LIBRULES($lib) " "]
    }

    puts -nonewline "PROJECT_LIBS :=   "
    puts [join $::PROJECT_LIBS " "]

    puts -nonewline "PROJECT_OBJS :=   "
    puts [join $::PROJECT_OBJS " "]
}

source [file dirname [info script]]/parse_cmdline.tcl

# We are depending on the following command-line options:
# --libdir, --libprefix, --libext, --srcroot, --objroot, --objext, --module
parse_cmdline $argv

foreach module [split $::MODULES ","] {
    process_module $module
}

print_makefile_output
