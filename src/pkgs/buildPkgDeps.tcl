#!/usr/bin/env tclsh

set indexfile [open $::env(HOME)/local/$::env(ARCH)/lib/visx/pkgIndex.tcl w]

set depfile [open $::env(HOME)/sorcery/grsh/dep/$::env(ARCH)/pkgdepends w]

proc print { chan txt } {
#	 puts -nonewline $txt
	 puts -nonewline $chan $txt
}

set libdir $::env(HOME)/local/$::env(ARCH)/lib/visx

set objdir $::env(HOME)/sorcery/grsh/obj/$::env(ARCH)/pkgs

cd $::env(HOME)/sorcery/grsh/src/pkgs

foreach dir [glob \[a-z\]*/] {
	 set ccfiles [glob ${dir}*.cc]

	 set pkg [string trimright $dir /]

	 set shlib $libdir/${pkg}.so

	 regsub -all {\.cc} $ccfiles .do files

	 set objfiles [list]

	 print $::depfile "${shlib}:"

	 foreach file $files {
		  lappend objfiles ${::objdir}/$file

		  print $::depfile " \\\n\t${::objdir}/$file"
	 }

	 print $::depfile "\n\n"

	 set ccfiles [glob ${dir}*.cc]
	 set code [catch {eval exec grep -l _Init $ccfiles} initfiles]

	 if { $code == 0 } {

		  if { [llength $initfiles] > 1 } {
				error "package $pkg has more than one Tcl_PkgInit function"
		  }

		  set initfile [open $initfiles r]

		  set contents [read $initfile]

		  close $initfile

		  set code [regexp {Revision: ([0-9\.]*)} $contents fullmatch revision]

		  set pkgname [string totitle $pkg]

		  print $::indexfile \
					 "package ifneeded $pkgname $revision { load $shlib }\n"
	 }
}

close $depfile
close $indexfile