#!/usr/bin/env tclsh

namespace eval cdeps {

    variable sys_regex "^#\\s*include\\s*(?:\"|<)(.*)(?:\"|>)\$"
    variable user_regex "^#\\s*include\\s*\"(.*)\"\$"

    proc grep_all_1st_submatches {str exp} {
	set res [list]
	set i 0
	while {[regexp -start $i -indices -line -- \
		    $exp $str fullmatch submatch]} {
	    foreach {from to} $submatch {
		lappend res [string range $str $from $to]
		set i [incr to]
	    }
	}
	return $res
    }

    # return the raw filenames from the #include directives in the given file
    proc get_raw_includes { filename } {
	if { ![info exists ::DIRECT_INCLUDES($filename)] } {

	    set f [open $filename r]
	    set text [read $f]
	    close $f

	    set ::DIRECT_INCLUDES($filename) \
		[grep_all_1st_submatches $text $cdeps::user_regex]
	}

	return $::DIRECT_INCLUDES($filename)
    }

    # Processes "includes_list" (i.e. a list of partial filenames from
    # #include directives ), and splits it into two lists: (1) a list of full
    # filenames that do exist in the given "search_dir", and (2) a list of the
    # remaining partial filenames do not exist there
    proc resolve_one_search_dir { includes_list search_dir } {
	set resolved [list]
	set unresolved [list]

	foreach filename $includes_list {
	    set fullpath [file join $search_dir $filename]
	    if { [file exists $fullpath] } {
		lappend resolved $fullpath
	    } else {
		lappend unresolved $filename
	    }
	}

	return [list $resolved $unresolved]
    }

    # Finds the files that are #include'd in "filename", and tries to resolve
    # those files to full pathnames using search directories in "search_path"
    proc get_resolved_includes { filename search_path } {

	set resolved [list]
	set unresolved [get_raw_includes $filename]

	# prepend the source file's containing directory to the search path 
	set search_path [concat [file dirname $filename] $search_path]

	foreach search_dir $search_path {
	    set result [resolve_one_search_dir $unresolved $search_dir]
	    set resolved [concat $resolved [lindex $result 0]]
	    set unresolved [lindex $result 1]
	}

	if { [llength $unresolved] > 0 } {
	    puts stderr "warning: missing \#include's for $filename:"
	    puts stderr "\t[join $unresolved \n\t]\n"
	}

	return $resolved
    }

    proc has_extension { filename ext_list } {
	set ext [file extension $filename]

	foreach x $ext_list { if { [string equal $ext $x] } { return 1 } }

	return 0
    }

    proc is_source_file { filename } {

	set ext_list [list .cc .h .C .H .c .h]

	return [has_extension $filename $ext_list]
    }

    proc is_cc_file { filename } {
	set ext_list [list .cc .C .c]

	return [has_extension $filename $ext_list]
    }

    proc get_nested_includes { filename search_path } {
	if { ![info exists ::NESTED_INCLUDES($filename)] } {

	    set nincludes [list $filename]

	    foreach dep [cdeps::get_resolved_includes $filename $search_path] {
		# check for recursion: if so, then skip on ahead
		if { [info exists ::CDEPS(${filename},${dep})] } {
		    continue
		}
		set ::CDEPS(${filename},${dep}) 1
		foreach inc [get_nested_includes $dep $search_path] {
		    lappend nincludes $inc
		}
	    }

	    set ::NESTED_INCLUDES($filename) [lsort -unique $nincludes]
	}

	return $::NESTED_INCLUDES($filename)
    }

    proc batch_build { filename_list search_path } {

	foreach f $filename_list {
	    if { [file isdirectory $f] } {

		if { [regexp {(RCS|CVS)} [file tail $f] ""] } {
		    continue
		}

		batch_build [lsort [glob -nocomplain ${f}/*]] $search_path

	    } elseif { [file isfile $f] } {

		if { [is_cc_file $f] } {
		    get_nested_includes $f $search_path
		}
	    }
	}
    }

    proc batch_print { filename_list search_path } {
	puts -nonewline "# Do not edit this file! "
	puts -nonewline "It is automatically generated. "
	puts -nonewline "Changes will be lost.\n"

	batch_build $filename_list $search_path

	foreach f [lsort [array names ::NESTED_INCLUDES]] {
	    if { ![is_cc_file $f] } continue

	    set stem [file rootname $f]
	    set ofile [string replace $stem 0 3 obj/i686/]

	    puts -nonewline "\n${ofile}.do ${ofile}.o:"
	    foreach dep $::NESTED_INCLUDES($f) {
		puts -nonewline " \\\n\t$dep"
	    }
	    puts ""
	}
    }

}

set lmapping {
    {src/* obj/i686/*}
    {}
}

namespace eval ldeps {
}

set USER_IPATH [list]

set SYS_IPATH {
    /cit/rjpeters/local/i686/include
    /cit/rjpeters/local/i686/gcc-3.0.2/include/
    /cit/rjpeters/local/i686/gcc-3.0.2/include/g++-v3
    /cit/rjpeters/local/i686/gcc-3.0.2/include/g++-v3/backward
    /cit/rjpeters/local/i686/gcc-3.0.2/include/g++-v3/i686-pc-linux-gnu
    /usr/include
    /usr/include/linux
    /usr/local/matlab/extern/include
}

set SRC_DIRS [list]

for {set i 0} {$i < [llength $argv]} {incr i} {
    set arg [lindex $argv $i]
    if { [string equal $arg "--include"] } {
	lappend ::USER_IPATH [lindex $argv [incr i]]
    } elseif { [string equal $arg "--src"] } {
	set dir [lindex $argv [incr i]]
	lappend ::SRC_DIRS $dir
	lappend ::USER_IPATH $dir
    }
}

cdeps::batch_print $argv $USER_IPATH