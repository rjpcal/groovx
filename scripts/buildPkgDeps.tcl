#!/usr/bin/env tclsh
#
# Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
#
# $Id$


proc getPkgNameFromPath { path } {

    set code [regexp {pkgs/([a-z]*)/} $path fullmatch pkgname]

    if { $code == 0 } {
	puts "couldn't extract package name from path '$path'"
	exit 1
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

    if { [regexp {Revision: ([0-9\.]*)} $contents fullmatch revision] } {
	return $revision
    } elseif { [regexp {Id: ([^ ]+) ([0-9]+)} $contents - fname revision] } {
	return $revision
    }

    error "couldn't determine package version"
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
	    puts "package $pkgname has more than one Tcl_PkgInit: $initfile"
	    exit 1
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

source [file dirname [info script]]/parse_cmdline.tcl

# We are depending on the following command-line args:
#   --depfile, --objdir, --objext, --pkgsrc, --libdir, and --pkgdirs
parse_cmdline $argv

set pkgdirs [split ${::PKGDIRS} ","]

set indexfile [open ${::LIBDIR}/pkgIndex.tcl w]

set depfile [open $::DEPFILE w]

cd $PKGSRC

print $::depfile "PKG_LIBS := \n\n"

foreach pkgdir $pkgdirs {

    set dir [string trimright $pkgdir "/"]

    if { ![file isdirectory $dir] } {
	puts "while computing package dependencies:"
	puts "no such directory '$dir'"
	exit 1
    }

    if { ![string equal $pkgdir "whitebox"] } {

	# For "normal" packages, we assume all the .cc files in the
	# directory should be merged into one shlib, and the directory name
	# becomes the package name
	set pkgname $pkgdir

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
