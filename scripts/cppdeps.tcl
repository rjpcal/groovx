#!/usr/bin/env tclsh

source [file dirname [info script]]/deps.tcl

proc run { argv } {

    set user_ipath [list]

    set sys_ipath {
	/usr/include
	/usr/include/linux
	/usr/local/matlab/extern/include
    }

    set src_dir ""
    set obj_dir ""

    for {set i 0} {$i < [llength $argv]} {incr i} {
	set arg [lindex $argv $i]
	if { [string equal $arg "--include"] } {
	    lappend user_ipath [lindex $argv [incr i]]
	} elseif { [string equal $arg "--src"] } {
	    set dir [lindex $argv [incr i]]
	    set src_dir $dir
	    lappend user_ipath $dir
	} elseif { [string equal $arg "--objdir"] } {
	    set obj_dir [lindex $argv [incr i]]
	}
    }

    cdeps::batch_print $src_dir $obj_dir $user_ipath
}

run $argv

set lmapping {
    {src/(.*)\\.cc obj/i686/\\1.do}
    {obj/i686/gfx/.*\\.do ~/local/i686/lib/libDeepGfx.d.so}
    {obj/i686/gwt/.*\\.do ~/local/i686/lib/libDeepGwt.d.so}
    {obj/i686/gx/.*\\.do ~/local/i686/lib/libDeepGx.d.so}
    {obj/i686/io/.*\\.do ~/local/i686/lib/libDeepIO.d.so}
    {obj/i686/system/.*\\.do ~/local/i686/lib/libDeepSystem.d.so}
    {obj/i686/tcl/.*\\.do ~/local/i686/lib/libDeepTcl.d.so}
    {obj/i686/togl/.*\\.do ~/local/i686/lib/libDeepTogl.d.so}
    {obj/i686/util/.*\\.do ~/local/i686/lib/libDeepUtil.d.so}
    {obj/i686/visx/.*\\.do ~/local/i686/lib/libDeepVisx.d.so}
}

#puts [ldeps::get_1 $argv src]
#puts [ldeps::get_1_mapped $argv src $lmapping]
