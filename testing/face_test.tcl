##############################################################################
###
### FaceTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Face

source ${::TEST_DIR}/gxshapekit_test.tcl

GxShapeKit::testSubclass Face Face

### faceCmd ###
test "FaceTcl-Obj::new Face" "too many args" {
	 Obj::new Face junk junk
} {wrong \# args: should be}
test "FaceTcl-Obj::new Face" "normal face creation" {
	 Obj::new Face
} {[0-9]+}

set EPS 0.00001 ;# floating-point precision required of next four commands
set faceid [Obj::new Face] ;# guaranteed face id for next operations

### eyeHeightCmd ###
test "FaceTcl-eyeHeight" "too few args" {
	 Face::eyeHeight
} {^wrong \# args: should be}
test "FaceTcl-eyeHeight" "too many args" {
	 Face::eyeHeight j u n
} {^wrong \# args: should be}
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
} {Face::eyeHeight: .*}

### eyeDistanceCmd ###
test "FaceTcl-eyeDistance" "too few args" {
	 Face::eyeDistance
} {^wrong \# args: should be}
test "FaceTcl-eyeDistance" "too many args" {
	 Face::eyeDistance j u n
} {^wrong \# args: should be}
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
} {Face::eyeDistance: .*}

### noseLengthCmd ###
test "FaceTcl-noseLength" "too few args" {
	 Face::noseLength
} {^wrong \# args: should be}
test "FaceTcl-noseLength" "too many args" {
	 Face::noseLength j u n
} {^wrong \# args: should be}
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
} {Face::noseLength: .*}

### mouthHeightCmd ###
test "FaceTcl-mouthHeight" "too few args" {
	 Face::mouthHeight
} {^wrong \# args: should be}
test "FaceTcl-mouthHeight" "too many args" {
	 Face::mouthHeight j u n
} {^wrong \# args: should be}
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
} {Face::mouthHeight: .*}

unset faceid
unset EPS

### IO::destringifyCmd ###
test "FaceTcl-IO::destringify" "error from wrong type" {
    set fx [Obj::new FixPt]
	 IO::destringify $fx junk
} {^IO::destringify: .*$}
test "FaceTcl-IO::destringify" "stringify/destringify check" {
	 set faceid [Obj::new Face]
	 Face::noseLength $faceid -1.2
	 Face::mouthHeight $faceid -0.6
	 Face::eyeHeight $faceid 0.4
	 Face::eyeDistance $faceid 0.6
	 set str1 [IO::stringify $faceid]

	 set faceid2 [Obj::new Face]
	 IO::destringify $faceid2 $str1
	 set str2 [IO::stringify $faceid2]

	 expr [string compare $str1 $str2] == 0
} {^1$}

