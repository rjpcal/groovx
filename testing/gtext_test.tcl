##############################################################################
###
### GtextTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Gtext
package require Objtogl
package require Tclgl
package require Tlist
package require Pos

source ${::TEST_DIR}/grobj_test.tcl

if { ![Togl::inited] } { Togl::init; update }

set ::GTEXT [IO::new Gtext]
Gtext::text $::GTEXT "Gtext"
GrObj::testSubclass Gtext Gtext $::GTEXT


set GTEXT -1
set PACKAGE GtextTcl

### IO::new Gtext ###
test "$PACKAGE-IO::new Gtext" "too many args" {
	 IO::new Gtext junk
} {^wrong \# args: should be "IO::new typename"$}
test "$PACKAGE-IO::new Gtext" "normal use" {
	 set ::GTEXT [IO::new Gtext]
	 Gtext::text $::GTEXT "Hello, World!"
	 return $::GTEXT
} "^${INT}$"

### Gtext::textCmd ###
test "$PACKAGE-Gtext::text" "too few args" {
	 Gtext::text
} {^wrong \# args: should be "Gtext::text item_id\(s\) \?new_value\(s\)\?"$}
test "$PACKAGE-Gtext::text" "too many args" {
	 Gtext::text $::GTEXT oops junk
} {^wrong \# args: should be "Gtext::text item_id\(s\) \?new_value\(s\)\?"$}
test "$PACKAGE-Gtext::text" "normal use set, get, compare" {
	 Gtext::text $::GTEXT "Hello, again"
	 Gtext::text $::GTEXT
} {^Hello, again$}
test "$PACKAGE-Gtext::text" "error" {} {^$} $no_test

### Gtext::stringifyCmd ###
### Gtext::destringifyCmd ###
test "$PACKAGE-Gtext::stringify" "stringify, destringify, and compare" {
	 set str1 [IO::stringify $::GTEXT]
	 set ::GTEXT [IO::new Gtext]
	 IO::destringify $::GTEXT $str1
	 set str2 [IO::stringify $::GTEXT]
	 string equal $str1 $str2
} {^1$}

set POS [IO::new Position]

### Gtext rendering ###
test "$PACKAGE-rendering" "normal render" {
	 clearscreen
	 set trial [Tlist::dealSingles $::GTEXT $::POS]
	 Togl::loadFont
	 show $trial
	 expr {[pixelCheckSum] != 0}
} {^1$}

### cleanup
unset PACKAGE
unset GTEXT
unset POS
