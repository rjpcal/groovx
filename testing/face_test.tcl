##############################################################################
###
### FaceTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Face

source ${::TEST_DIR}/grobj_test.tcl

GrObj::testSubclass Face Face

### faceCmd ###
test "FaceTcl-IO::new Face" "too many args" {
	 IO::new Face junk
} {wrong \# args: should be "IO::new typename"}
test "FaceTcl-IO::new Face" "normal face creation" {
	 IO::new Face
} {[0-9]+}

set EPS 0.00001 ;# floating-point precision required of next four commands
set faceid [IO::new Face] ;# guaranteed face id for next operations

### eyeHeightCmd ###
test "FaceTcl-eyeHeight" "too few args" {
	 Face::eyeHeight
} {^wrong \# args: should be "Face::eyeHeight item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-eyeHeight" "too many args" {
	 Face::eyeHeight j u n
} {^wrong \# args: should be "Face::eyeHeight item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-eyeHeight" "normal use set" {
	 Face::eyeHeight $::faceid 2.0 
} {^$}
test "FaceTcl-eyeHeight" "normal use query" {
	 set val1 1.234
	 Face::eyeHeight $::faceid $val1
	 set val2 [Face::eyeHeight $::faceid]
	 expr abs($val2-$val1) < $::EPS
} {^1$}
test "FaceTcl-eyeHeight" "error from nun-numeric input" {
    Face::eyeHeight $::faceid junk
} {expected floating-point number but got "junk"}
test "FaceTcl-eyeHeight" "error from bad objid" {
    Face::eyeHeight -1
} {Face::eyeHeight: attempt to access invalid id '.*' in}

### eyeDistanceCmd ###
test "FaceTcl-eyeDistance" "too few args" {
	 Face::eyeDistance
} {^wrong \# args: should be "Face::eyeDistance item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-eyeDistance" "too many args" {
	 Face::eyeDistance j u n
} {^wrong \# args: should be "Face::eyeDistance item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-eyeDistance" "normal use set" {
	 Face::eyeDistance $::faceid 2.0 
} {^$}
test "FaceTcl-eyeDistance" "normal use query" {
	 set val1 1.357
	 Face::eyeDistance $::faceid $val1
	 set val2 [Face::eyeDistance $::faceid]
	 expr abs($val2-$val1) < $::EPS
} {^1$}
test "FaceTcl-eyeDistance" "error from nun-numeric input" {
    Face::eyeDistance $::faceid junk
} {expected floating-point number but got "junk"}
test "FaceTcl-eyeDistance" "error from bad objid" {
    Face::eyeDistance -1
} {Face::eyeDistance: attempt to access invalid id '.*' in}

### noseLengthCmd ###
test "FaceTcl-noseLength" "too few args" {
	 Face::noseLength
} {^wrong \# args: should be "Face::noseLength item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-noseLength" "too many args" {
	 Face::noseLength j u n
} {^wrong \# args: should be "Face::noseLength item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-noseLength" "normal use set" {
	 Face::noseLength $::faceid 2.0 
} {^$}
test "FaceTcl-noseLength" "normal use query" {
	 set val1 0.246
	 Face::noseLength $::faceid $val1
	 set val2 [Face::noseLength $::faceid]
	 expr abs($val2-$val1) < $::EPS
} {^1$}
test "FaceTcl-noseLength" "error from nun-numeric input" {
    Face::noseLength $::faceid junk
} {expected floating-point number but got "junk"}
test "FaceTcl-noseLength" "error from bad objid" {
    Face::noseLength -1
} {Face::noseLength: attempt to access invalid id '.*' in}

### mouthHeightCmd ###
test "FaceTcl-mouthHeight" "too few args" {
	 Face::mouthHeight
} {^wrong \# args: should be "Face::mouthHeight item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-mouthHeight" "too many args" {
	 Face::mouthHeight j u n
} {^wrong \# args: should be "Face::mouthHeight item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-mouthHeight" "normal use set" {
	 Face::mouthHeight $::faceid 2.0 
} {^$}
test "FaceTcl-mouthHeight" "normal use query" {
	 set val1 9.876
	 Face::mouthHeight $::faceid $val1
	 set val2 [Face::mouthHeight $::faceid]
	 expr abs($val2-$val1) < $::EPS
} {^1$}
test "FaceTcl-mouthHeight" "error from nun-numeric input" {
    Face::mouthHeight $::faceid junk
} {expected floating-point number but got "junk"}
test "FaceTcl-mouthHeight" "error from bad objid" {
    Face::mouthHeight -1
} {Face::mouthHeight: attempt to access invalid id '.*' in}

unset faceid
ObjList::reset
unset EPS

### Face::stringifyCmd ###
test "FaceTcl-Face::stringify" "error from wrong type" {
    set fx [IO::new FixPt]
	 Face::stringify $fx
} {^Face::stringify: an error of type (std::)?bad_cast occurred:.*$}

### Face::destringifyCmd ###
test "FaceTcl-Face::destringify" "error from wrong type" {
    set fx [IO::new FixPt]
	 Face::destringify $fx junk
} {^Face::destringify: an error of type (std::)?bad_cast occurred:.*$}
test "FaceTcl-Face::destringify" "stringify/destringify check" {
	 set faceid [IO::new Face]
	 Face::noseLength $faceid -1.2
	 Face::mouthHeight $faceid -0.6
	 Face::eyeHeight $faceid 0.4
	 Face::eyeDistance $faceid 0.6
	 set str1 [Face::stringify $faceid]

	 set faceid2 [IO::new Face]
	 Face::destringify $faceid2 $str1
	 set str2 [Face::stringify $faceid2]

	 expr [string compare $str1 $str2] == 0
} {^1$}

ObjList::reset

