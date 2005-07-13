# $Id$

# A tcl library for computing compile and link dependencies for c/c++
# source code.

namespace eval depconf {
    variable user_ipath [list]
    variable sys_ipath {
	/usr/include
	/usr/include/linux
	/usr/local/matlab/extern/include
    }

    variable srcdir ""
    variable objdir ""
    variable libdir ""

    # (1) Remove any "./" prefix, and (2) remove any trailing slashes
    proc trim_dirname { dirname } {

	if { [string range $dirname 0 1] == "./" } {
	    set dirname [string range $dirname 2 end]
	}

	if { [string length $dirname] > 1 && \
		 [string index $dirname end] == "/" } {
	    set dirname [string trimright $dirname "/"]
	}

	return $dirname
    }

    proc parse_argv { argv } {

	set remaining [list]

	for {set i 0} {$i < [llength $argv]} {incr i} {
	    set arg [lindex $argv $i]
	    if { [string equal $arg "--include"] } {
		lappend depconf::user_ipath [lindex $argv [incr i]]
	    } elseif { [string equal $arg "--src"] } {
		set dir [trim_dirname [lindex $argv [incr i]]]
		set depconf::srcdir $dir
		lappend depconf::user_ipath $dir
	    } elseif { [string equal $arg "--objdir"] } {
		set depconf::objdir [trim_dirname [lindex $argv [incr i]]]
	    } elseif { [string equal $arg "--libdir"] } {
		set depconf::libdir [trim_dirname [lindex $argv [incr i]]]
	    } else {
		lappend remaining $arg
	    }
	}

	return $remaining
    }
}

namespace eval cdeps {

    variable sys_regex "^#\\s*include\\s*(?:\"|<)(\[^ \]?)(?:\"|>)"
    variable user_regex "^#\\s*include\\s*\"(\[^ \]*?)\""

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
    # #include directives ), and splits it into two lists: (1) a list of
    # full filenames that do exist in the given "search_dir", and (2) a
    # list of the remaining partial filenames do not exist there
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

    proc is_cc_filename { filename } {
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

		if { [is_cc_filename $f] } {
		    get_nested_includes $f $search_path
		}
	    }
	}
    }

    proc batch_print { src_dir obj_dir search_path } {
	puts "# Do not edit this file! It is automatically generated. Changes will be lost."

	set src_dir [depconf::trim_dirname $src_dir]
	set obj_dir [depconf::trim_dirname $obj_dir]

	# Add an empty "directory name" to the search path... this
	# essentially includes the current working directory to the search
	# path.
	lappend search_path ""

	batch_build $src_dir $search_path

	# The string offset at which the real source file name will start
	# after the src_dir/ prefix
	set offset [expr [string length $src_dir]+1]

	foreach f [lsort [array names ::NESTED_INCLUDES]] {
	    if { ![is_cc_filename $f] } continue

	    set stem [file rootname $f]
	    set idx [string first ${src_dir}/ $stem]

	    if { $idx != 0 } {
		error "source file '$f' not within src path '${src_dir}/'"
	    }

	    set ofile [file join $obj_dir [string range $stem $offset end]]

	    puts -nonewline "${ofile}.o:  "
	    puts [join $::NESTED_INCLUDES($f) " "]
	}
    }

}

namespace eval ldeps {
    proc get_1 { filename search_path } {
	if { ![info exists ::LINK_DEPENDS($filename)] } {
	    set includes [cdeps::get_nested_includes $filename $search_path]

	    set deps [list $filename]

	    foreach f $includes {
		set cc_file [file rootname $f].cc
		if { [file exists $cc_file] } {
		    if { [info exists ::LDEPS(${filename},${cc_file})] } {
			continue
		    }
		    set ::LDEPS(${filename},${cc_file}) 1
		    foreach nested [get_1 $cc_file $search_path] {
			lappend deps $nested
		    }
		}
	    }

	    set ::LINK_DEPENDS($filename) [lsort -unique $deps]
	}
	return $::LINK_DEPENDS($filename)
    }

    proc get_1_mapped { filename search_path mapping } {
	set deps [get_1 $filename $search_path]

	set mapped_deps [list]

	foreach dep $deps {
	    set mapped $dep

	    foreach item $mapping {
		set exp [lindex $item 0]
		set subspec [lindex $item 1]

		if { [regsub $exp $mapped $subspec output] > 0 } {
		    set mapped $output
		}
	    }

	    lappend mapped_deps $mapped
	}

	return [lsort -unique $mapped_deps]
    }

    proc get_batch { filename_list search_path mapping } {
	set deps [list]

	foreach f $filename_list {

	    if { [file isdirectory $f] } {
		puts "dir: $f"
		set deps [concat $deps [get_batch [glob -nocomplain ${f}/*] \
					    $search_path $mapping]]

	    } elseif { [file isfile $f] } {
		set deps [concat $deps \
			      [get_1_mapped $f $search_path $mapping]]
	    }
	}

	set deps [lsort -unique $deps]
	set index [lsearch $deps {}]
	if { $index >= 0 } {
	    set deps [lreplace $deps $index $index]
	}
	return $deps
    }
}
