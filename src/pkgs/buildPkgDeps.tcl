#!/usr/bin/env tclsh
#
# Copyright (c) 2002-2003 Rob Peters rjpeters@klab.caltech.edu
#
# $Revision$


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

    if { [llength $ccfiles] == 0 } { return }

    set shlib $libdir/${pkgname}[info sharedlibextension]

    regsub -all {\.cc} $ccfiles .${::OBJEXT} files

    print $::depfile "PKG_LIBS += ${shlib}\n\n"

    print $::depfile "${shlib}:"

    foreach file $files {
	print $::depfile " \\\n\t${::OBJDIR}/$file"
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

source [file dirname [info script]]/../../scripts/parse_cmdline.tcl

# We are depending on the following command-line args:
#   --depfile, --objdir, --objext, --pkgdir, and --libdir
parse_cmdline $argv

set indexfile [open ${::LIBDIR}/pkgIndex.tcl w]

set depfile [open $::DEPFILE w]

cd $PKGDIR

print $::depfile "PKG_LIBS := \n\n"

foreach dir [glob \[a-z\]*/] {

    set dir [string trimright $dir "/"]

    if { ![string equal $dir "whitebox"] } {

	# For "normal" packages, we assume all the .cc files in the
	# directory should be merged into one shlib, and the directory name
	# becomes the package name
	set pkgname [file tail $dir]

	set ccfiles [glob -nocomplain ${dir}/*.cc]

	setup_package $pkgname $ccfiles $LIBDIR
    } else {

	# For the special case of the whitebox tests, each .cc file becomes
	# its own package/shlib, and the package name is taken from the
	# stem (i.e. "rootname") of the .cc filename

	set ccfiles [glob -nocomplain ${dir}/*.cc]

	foreach ccfile $ccfiles {
	    set pkgname [file tail [file rootname $ccfile]]

	    setup_package $pkgname $ccfile $LIBDIR
	}
    }
}

close $depfile
close $indexfile
