##############################################################################
###
### FixptTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### Fixpt::fixptCmd ###
test "FixptTcl-Fixpt::fixpt" "no args limit" {} $BLANK $no_test
test "FixptTcl-Fixpt::fixpt" "normal create" {
	 Fixpt::fixpt
} {^[0-9]+$}

set fix [Fixpt::fixpt]

### lengthCmd ###
test "FixptTcl-length" "too few args" {
    Fixpt::length
} {^wrong \# args: should be "Fixpt::length item_id\(s\) \?new_value\(s\)\?"$}
test "FixptTcl-length" "too many args" {
    Fixpt::length j u n
} {^wrong \# args: should be "Fixpt::length item_id\(s\) \?new_value\(s\)\?"$}
test "FixptTcl-length" "normal set" {
	 catch {Fixpt::length $::fix 1.0}
} {^0$}
test "FixptTcl-length" "error from non-numeric input" {
    Fixpt::length $::fix junk
} {^expected floating-point number but got "junk"$}
test "FixptTcl-length" "error from bad objid" {
    Fixpt::length -1 1.0
} {^Fixpt::length: objid out of range$}
test "FixptTcl-length" "error from wrong type" {
    set f [Face::face]
	 Fixpt::length $f 1.0
} {^Fixpt::length: object not of correct type$}

### widthCmd ###
test "FixptTcl-width" "too few args" {
    Fixpt::width
} {wrong \# args: should be "Fixpt::width item_id\(s\) \?new_value\(s\)\?"}
test "FixptTcl-width" "too many args" {
    Fixpt::width j u n
} {wrong \# args: should be "Fixpt::width item_id\(s\) \?new_value\(s\)\?"}
test "FixptTcl-width" "normal set" {
	 catch {Fixpt::width $::fix 2}
} {^0$}
test "FixptTcl-width" "error from non-numeric input" {
    Fixpt::width $::fix junk
} {expected integer but got "junk"}
test "FixptTcl-width" "error from bad objid" {
    Fixpt::width -1 5
} {Fixpt::width: objid out of range}
test "FixptTcl-width" "error from wrong type" {
    set f [Face::face]
	 Fixpt::width $f 5
} {Fixpt::width: object not of correct type}
test "FixptTcl-width" "error from non-integral number" {
    Fixpt::width $::fix 1.5
} {expected integer but got "1\.5"}

ObjList::reset
unset fix

