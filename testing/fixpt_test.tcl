##############################################################################
###
### FixptTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Fixpt
package require Objlist

source ${::TEST_DIR}/grobj_test.tcl

GrObj::testSubclass Fixpt FixPt

### IO::new FixPt ###
test "FixptTcl-IO::new FixPt" "normal create" {
	 IO::new FixPt
} {^[0-9]+$}

set fix [IO::new FixPt]

### lengthCmd ###
test "FixptTcl-length" "too few args" {
    FixPt::length
} {^wrong \# args: should be "FixPt::length item_id\(s\) \?new_value\(s\)\?"$}
test "FixptTcl-length" "too many args" {
    FixPt::length j u n
} {^wrong \# args: should be "FixPt::length item_id\(s\) \?new_value\(s\)\?"$}
test "FixptTcl-length" "normal set" {
	 catch {FixPt::length $::fix 1.0}
} {^0$}
test "FixptTcl-length" "error from non-numeric input" {
    FixPt::length $::fix junk
} {^FixPt::length: expected floating-point number but got "junk"$}
test "FixptTcl-length" "error from bad objid" {
    FixPt::length -1 1.0
} {^FixPt::length: attempt to access invalid id '.*' in}
test "FixptTcl-length" "error from wrong type" {
    set f [IO::new Face]
	 FixPt::length $f 1.0
} {^FixPt::length: an error of type (std::)?bad_cast occurred:.*$}

### widthCmd ###
test "FixptTcl-width" "too few args" {
    FixPt::width
} {wrong \# args: should be "FixPt::width item_id\(s\) \?new_value\(s\)\?"}
test "FixptTcl-width" "too many args" {
    FixPt::width j u n
} {wrong \# args: should be "FixPt::width item_id\(s\) \?new_value\(s\)\?"}
test "FixptTcl-width" "normal set" {
	 catch {FixPt::width $::fix 2}
} {^0$}
test "FixptTcl-width" "error from non-numeric input" {
    FixPt::width $::fix junk
} {^FixPt::width: expected integer but got "junk"$}
test "FixptTcl-width" "error from bad objid" {
    FixPt::width -1 5
} {^FixPt::width: attempt to access invalid id '.*' in}
test "FixptTcl-width" "error from wrong type" {
    set f [IO::new Face]
	 FixPt::width $f 5
} {^FixPt::width: an error of type (std::)?bad_cast occurred:.*$}
test "FixptTcl-width" "error from non-integral number" {
    FixPt::width $::fix 1.5
} {^FixPt::width: expected integer but got "1\.5"$}

ObjList::reset
unset fix

