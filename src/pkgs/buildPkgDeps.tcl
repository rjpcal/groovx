#!/usr/bin/env tclsh



proc getPkgNameFromPath { path } {

    set code [regexp {pkgs/([a-z]*)/} $path fullmatch pkgname]

    if { $code == 0 } {
	error "couldn't extract package name from path '$path'"
    }

    return $pkgname
}

proc extractPkgReqs { contents } {
    set lines [regexp -line -inline -all {include "pkgs/.*/.*"} $contents]

    set pkgs [list]

    foreach line $lines {
	lappend pkgs [string totitle [getPkgNameFromPath $line]]
    }

    return $pkgs
}

proc extractPkgRevision { contents } {

    set code [regexp {Revision: ([0-9\.]*)} $contents fullmatch revision]

    return $revision
}

proc getFileContents { filename } {
    set f [open $filename r]
    set contents [read $f]
    close $f
    return $contents
}

proc print { chan txt } {
    #     puts -nonewline $txt
    puts -nonewline $chan $txt
}



proc setup_package { pkgname ccfiles libdir } {

    set shlib $libdir/${pkgname}[info sharedlibextension]

    regsub -all {\.cc} $ccfiles .do files

    print $::depfile "PKG_LIBS += ${shlib}\n\n"

    print $::depfile "${shlib}:"

    foreach file $files {
	print $::depfile " \\\n\t${::objdir}/$file"
    }

    print $::depfile "\n\n"

    set code [catch {eval exec grep -l _Init $ccfiles} initfile]

    if { $code == 0 } {

	if { [llength $initfile] > 1 } {
	    error "package $pkgname has more than one Tcl_PkgInit: $initfile"
	}

	set pkgtitle [string totitle $pkgname]

	set contents [getFileContents $initfile]

	set revision [extractPkgRevision $contents]

	set pkgreqs [extractPkgReqs $contents]

	print $::indexfile "package ifneeded $pkgtitle $revision \{\n"

	foreach req $pkgreqs {
	    print $::indexfile "\tpackage require $req\n"
	}

	print $::indexfile "\tload $shlib\n\}\n"
    }
}

#
# Main code
#

set project_root [pwd]

set indexfile [open $::env(HOME)/local/$::env(ARCH)/lib/visx/pkgIndex.tcl w]

# The name of the output file is given in the command-line args to this
# script:
set depfile [open [lindex $argv 0] w]

set libdir $::env(HOME)/local/$::env(ARCH)/lib/visx

set objdir obj/$::env(ARCH)/pkgs

set pkgdir ${project_root}/src/pkgs

cd $pkgdir

print $::depfile "PKG_LIBS := \n\n"

foreach dir [glob \[a-z\]*/] {

    set dir [string trimright $dir "/"]

    if { ![string equal $dir "whitebox"] } {

	# For "normal" packages, we assume all the .cc files in the
	# directory should be merged into one shlib, and the directory name
	# becomes the package name
	set pkgname [file tail $dir]

	set ccfiles [glob ${dir}/*.cc]

	setup_package $pkgname $ccfiles $libdir
    } else {

	# For the special case of the whitebox tests, each .cc file becomes
	# its own package/shlib, and the package name is taken from the
	# stem (i.e. "rootname") of the .cc filename

	set ccfiles [glob -nocomplain ${dir}/*.cc]

	foreach ccfile $ccfiles {
	    set pkgname [file tail [file rootname $ccfile]]

	    setup_package $pkgname $ccfile $libdir
	}
    }
}

close $depfile
close $indexfile
