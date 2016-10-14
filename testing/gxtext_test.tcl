##############################################################################
###
### GxTextTcl
### Rob Peters
###
##############################################################################

package require Gxtext
package require Toglet
package require Gl
package require Tlist

source ${::TEST_DIR}/gxshapekit_test.tcl

set p [Obj::new GxText]
-> $p text "Hello"
::testGxshapekitSubclass GxText $p


set PACKAGE GxTextTcl

### Obj::new GxText ###
test "$PACKAGE-Obj::new GxText" "too many args" {
    Obj::new GxText junk junk
} {^wrong \# args: should be}
test "$PACKAGE-Obj::new GxText" "normal use" {
    set p [Obj::new GxText]
    GxText::text $p "Hello, World!"
} {^$}

### GxText::textCmd ###
test "$PACKAGE-GxText::text" "too few args" {
    GxText::text
} {^wrong \# args: should be}
test "$PACKAGE-GxText::text" "too many args" {
    GxText::text blah oops junk
} {^wrong \# args: should be}
test "$PACKAGE-GxText::text" "normal use set, get, compare" {
    set p [new GxText]
    -> $p text "Hello, again"
    -> $p text
} {^Hello, again$}

### GxText rendering ###
test "$PACKAGE-rendering" "normal render" {
    clearscreen
    set p [new GxText]
    -> $p text "Hi there."
    see $p
    expr {[-> [::cv] pixelCheckSum] != 0}
} {^1$}

### cleanup
unset PACKAGE
