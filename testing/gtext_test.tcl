##############################################################################
###
### GtextTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Gtext
package require Toglet
package require Tclgl
package require Tlist

source ${::TEST_DIR}/gxshapekit_test.tcl

if { ![Togl::inited] } { Togl::init; update }

set ::GTEXT [Obj::new Gtext]
Gtext::text $::GTEXT "Gtext"
GxShapeKit::testSubclass Gtext Gtext $::GTEXT


set GTEXT -1
set PACKAGE GtextTcl

### Obj::new Gtext ###
test "$PACKAGE-Obj::new Gtext" "too many args" {
	 Obj::new Gtext junk junk
} {^wrong \# args: should be}
test "$PACKAGE-Obj::new Gtext" "normal use" {
	 set ::GTEXT [Obj::new Gtext]
	 Gtext::text $::GTEXT "Hello, World!"
	 return $::GTEXT
} "^${INT}$"

### Gtext::textCmd ###
test "$PACKAGE-Gtext::text" "too few args" {
	 Gtext::text
} {^wrong \# args: should be}
test "$PACKAGE-Gtext::text" "too many args" {
	 Gtext::text $::GTEXT oops junk
} {^wrong \# args: should be}
test "$PACKAGE-Gtext::text" "normal use set, get, compare" {
	 Gtext::text $::GTEXT "Hello, again"
	 Gtext::text $::GTEXT
} {^Hello, again$}
test "$PACKAGE-Gtext::text" "error" {} {^$} $no_test

### Gtext rendering ###
test "$PACKAGE-rendering" "normal render" {
	 clearscreen
	 see $::GTEXT
	 expr {[pixelCheckSum] != 0}
} {^1$}

### cleanup
unset PACKAGE
unset GTEXT
