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
test "Face::eyeHeight" "too few args" {
    Face::eyeHeight
} {^wrong \# args: should be}
test "Face::eyeHeight" "too many args" {
    Face::eyeHeight j u n
} {^wrong \# args: should be}
test "Face::eyeHeight" "normal use set" {
    Face::eyeHeight $::faceid 2.0
} {^$}
test "Face::eyeHeight" "normal use query" {
    set val1 1.234
    Face::eyeHeight $::faceid $val1
    set val2 [Face::eyeHeight $::faceid]
    expr abs($val2-$val1) < $::EPS
} {^1$}
test "Face::eyeHeight" "error from nun-numeric input" {
    Face::eyeHeight $::faceid junk
} {expected.*but got}
test "Face::eyeHeight" "error from bad objid" {
    Face::eyeHeight -1
} {Face::eyeHeight: .*}

### eyeDistanceCmd ###
test "Face::eyeDistance" "too few args" {
    Face::eyeDistance
} {^wrong \# args: should be}
test "Face::eyeDistance" "too many args" {
    Face::eyeDistance j u n
} {^wrong \# args: should be}
test "Face::eyeDistance" "normal use set" {
    Face::eyeDistance $::faceid 2.0
} {^$}
test "Face::eyeDistance" "normal use query" {
    set val1 1.357
    Face::eyeDistance $::faceid $val1
    set val2 [Face::eyeDistance $::faceid]
    expr abs($val2-$val1) < $::EPS
} {^1$}
test "Face::eyeDistance" "error from nun-numeric input" {
    Face::eyeDistance $::faceid junk
} {expected.*but got}
test "Face::eyeDistance" "error from bad objid" {
    Face::eyeDistance -1
} {Face::eyeDistance: .*}

### noseLengthCmd ###
test "Face::noseLength" "too few args" {
    Face::noseLength
} {^wrong \# args: should be}
test "Face::noseLength" "too many args" {
    Face::noseLength j u n
} {^wrong \# args: should be}
test "Face::noseLength" "normal use set" {
    Face::noseLength $::faceid 2.0
} {^$}
test "Face::noseLength" "normal use query" {
    set val1 0.246
    Face::noseLength $::faceid $val1
    set val2 [Face::noseLength $::faceid]
    expr abs($val2-$val1) < $::EPS
} {^1$}
test "Face::noseLength" "error from nun-numeric input" {
    Face::noseLength $::faceid junk
} {expected.*but got}
test "Face::noseLength" "error from bad objid" {
    Face::noseLength -1
} {Face::noseLength: .*}

### mouthHeightCmd ###
test "Face::mouthHeight" "too few args" {
    Face::mouthHeight
} {^wrong \# args: should be}
test "Face::mouthHeight" "too many args" {
    Face::mouthHeight j u n
} {^wrong \# args: should be}
test "Face::mouthHeight" "normal use set" {
    Face::mouthHeight $::faceid 2.0
} {^$}
test "Face::mouthHeight" "normal use query" {
    set val1 9.876
    Face::mouthHeight $::faceid $val1
    set val2 [Face::mouthHeight $::faceid]
    expr abs($val2-$val1) < $::EPS
} {^1$}
test "Face::mouthHeight" "error from nun-numeric input" {
    Face::mouthHeight $::faceid junk
} {expected.*but got}
test "Face::mouthHeight" "error from bad objid" {
    Face::mouthHeight -1
} {Face::mouthHeight: .*}

unset faceid
unset EPS

### readLGX ###
test "Face::readLGX" "error from wrong type" {
    set fx [Obj::new Face]
    -> $fx readLGX junk
} {readLGX: }
test "Face::readLGX" "writeLGX/readLGX check" {
    set f [Obj::new Face]
    -> $f noseLength -1.2
    -> $f mouthHeight -0.6
    -> $f eyeHeight 0.4
    -> $f eyeDistance 0.6
    set str1 [-> $f writeLGX]

    set f2 [Obj::new Face]
    -> $f2 readLGX $str1
    set str2 [-> $f2 writeLGX]

    expr [string compare $str1 $str2] == 0
} {^1$}

### CloneFace ###
test "CloneFace" "check fields" {
    set cf [new CloneFace]
    -> $cf eyeAspect 0.5
    -> $cf vertOffset 0.0
} {^$}
