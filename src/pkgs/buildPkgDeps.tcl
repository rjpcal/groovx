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
    set ccfiles [glob ${dir}*.cc]

    set pkg [string trimright $dir /]

    set shlib $libdir/${pkg}.so

    regsub -all {\.cc} $ccfiles .do files

    set objfiles [list]

    print $::depfile "PKG_LIBS += ${shlib}\n\n"

    print $::depfile "${shlib}:"

    foreach file $files {
	lappend objfiles ${::objdir}/$file

	print $::depfile " \\\n\t${::objdir}/$file"
    }

    print $::depfile "\n\n"

    set ccfiles [glob ${dir}*.cc]
    set code [catch {eval exec grep -l _Init $ccfiles} initfile]

    if { $code == 0 } {

	if { [llength $initfile] > 1 } {
	    error "package $pkg has more than one Tcl_PkgInit: $initfile"
	}

	set pkgname [string totitle $pkg]

	set contents [getFileContents $initfile]

	set revision [extractPkgRevision $contents]

	set pkgreqs [extractPkgReqs $contents]

	print $::indexfile "package ifneeded $pkgname $revision \{\n" 

	foreach req $pkgreqs {
	    print $::indexfile "\tpackage require $req\n"
	} 
	
	print $::indexfile "\tload $shlib\n\}\n"
    }
}

close $depfile
close $indexfile
