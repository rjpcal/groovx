##############################################################################
###
### GxTextTcl
### Rob Peters
### $Id$
###
##############################################################################

package require Gxtext
package require Toglet
package require Gl
package require Tlist

source ${::TEST_DIR}/gxshapekit_test.tcl

set ::TEXT [Obj::new GxText]
GxText::text $::TEXT "GxText"
GxShapeKit::testSubclass GxText GxText $::TEXT


set TEXT -1
set PACKAGE GxTextTcl

### Obj::new GxText ###
test "$PACKAGE-Obj::new GxText" "too many args" {
	 Obj::new GxText junk junk
} {^wrong \# args: should be}
test "$PACKAGE-Obj::new GxText" "normal use" {
	 set ::TEXT [Obj::new GxText]
	 GxText::text $::TEXT "Hello, World!"
	 return $::TEXT
} "^${INT}$"

### GxText::textCmd ###
test "$PACKAGE-GxText::text" "too few args" {
	 GxText::text
} {^wrong \# args: should be}
test "$PACKAGE-GxText::text" "too many args" {
	 GxText::text $::TEXT oops junk
} {^wrong \# args: should be}
test "$PACKAGE-GxText::text" "normal use set, get, compare" {
	 GxText::text $::TEXT "Hello, again"
	 GxText::text $::TEXT
} {^Hello, again$}
test "$PACKAGE-GxText::text" "error" {} {^$} $no_test

### GxText rendering ###
test "$PACKAGE-rendering" "normal render" {
	 clearscreen
	 see $::TEXT
	 expr {[pixelCheckSum] != 0}
} {^1$}

### cleanup
unset PACKAGE
unset TEXT
