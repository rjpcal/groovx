##############################################################################
###
### GtextTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Objlist
package require Gtext
package require Objtogl
package require Tclgl
package require Tlist
package require Pos

if { ![Togl::inited] } { Togl::init "-rgba false"; update }

set GTEXT -1
set PACKAGE GtextTcl

### Gtext::GtextCmd ###
test "$PACKAGE-Gtext::Gtext" "too many args" {
	 Gtext::Gtext junk junk
} {^wrong \# args: should be "Gtext::Gtext \?text\?"$}
test "$PACKAGE-Gtext::Gtext" "normal use" {
	 set ::GTEXT [Gtext::Gtext "Hello, World!"]
} "^${INT}$"
test "$PACKAGE-Gtext::Gtext" "error" {} {^$} $no_test

### Gtext::loadFontCmd ###
test "$PACKAGE-Gtext::loadFont" "too many args" {
	 Gtext::loadFont fixed junk
} {^wrong \# args: should be "Gtext::loadFont \?fontname\?"$}
test "$PACKAGE-Gtext::loadFont" "normal use" {
	 catch {Gtext::loadFont}
} {^0$}
test "$PACKAGE-Gtext::loadFont" "error" {
	 Gtext::loadFont junk
} {^Gtext::loadFont: unable to load font junk$}

### Gtext::loadFontiCmd ###
test "$PACKAGE-Gtext::loadFonti" "too many args" {
	 Gtext::loadFonti 3 junk
} {^wrong \# args: should be "Gtext::loadFonti \?fontnumber\?"$}
test "$PACKAGE-Gtext::loadFonti" "normal use" {
	 catch {Gtext::loadFonti 3}
} {^0$}
test "$PACKAGE-Gtext::loadFonti" "error" {
	 Gtext::loadFonti 20
} {^Gtext::loadFonti: unable to load font$}

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
	 set str1 [Gtext::stringify $::GTEXT]
	 ObjList::reset
	 set ::GTEXT [Gtext::Gtext]
	 Gtext::destringify $::GTEXT $str1
	 set str2 [Gtext::stringify $::GTEXT]
	 string equal $str1 $str2
} {^1$}

set POS [Pos::position]

### Gtext rendering ###
test "$PACKAGE-rendering" "normal render" {
	 clearscreen
	 Tlist::makeSingles $::POS
	 show $::GTEXT
	 expr {[pixelCheckSum] != 0}
} {^1$}

test "$PACKAGE-rendering" "compare two fonts" {
	 clearscreen
	 Gtext::loadFonti 1
	 show $::GTEXT
	 set sum1 [pixelCheckSum]
	 clearscreen
	 Gtext::loadFonti 2
	 show $::GTEXT
	 set sum2 [pixelCheckSum]
	 clearscreen
	 expr {$sum1 != $sum2}
} {^1$}

### cleanup
unset PACKAGE
unset GTEXT
unset POS
