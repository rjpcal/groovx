##############################################################################
###
### FaceTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### faceCmd ###
test "FaceTcl-Face::Face" "too many args" {
	 Face::Face junk
} {wrong \# args: should be "Face::Face"}
test "FaceTcl-Face::Face" "normal face creation" {
	 Face::Face
} {[0-9]+}
test "FaceTcl-Face::Face" "no possible error" {} $BLANK $no_test

set EPS 0.00001 ;# floating-point precision required of next four commands
set faceid [Face::Face] ;# guaranteed face id for next operations

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
} {Face::eyeHeight: an error of type InvalidIdError occurred}

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
} {Face::eyeDistance: an error of type InvalidIdError occurred}

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
} {Face::noseLength: an error of type InvalidIdError occurred}

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
} {Face::mouthHeight: an error of type InvalidIdError occurred}

unset faceid
ObjList::reset
unset EPS

### Face::loadFacesCmd ###
test "FaceTcl-Face::loadFaces" "too few args" { 
	 Face::loadFaces
} {wrong \# args: should be "Face::loadFaces\
		  filename \?num_to_read\? \?use_virtual_ctor\?"}
test "FaceTcl-Face::loadFaces" "too many args" { 
	 Face::loadFaces j u n k y
} {wrong \# args: should be "Face::loadFaces\
		  filename \?num_to_read\? \?use_virtual_ctor\?"}
test "FaceTcl-Face::loadFaces" "normal file read with no comments" {
	 ObjList::reset
	 set objids [Face::loadFaces $::TEST_DIR/faces_file_no_comments]
	 set num_read [llength $objids]
	 expr $num_read == 10 && $num_read == [ObjList::count] \
				&& [string compare [GrObj::type 0] "Face"] == 0
} {^1$}
test "FaceTcl-Face::loadFaces" "normal file read" {
	 ObjList::reset
	 set objids [Face::loadFaces $::TEST_DIR/faces_file]
	 set num_read [llength $objids]
	 expr $num_read == 10 && $num_read == [ObjList::count] \
				&& [string compare [GrObj::type 0] "Face"] == 0
} {^1$}
test "FaceTcl-Face::loadFaces" "normal file read with limit on # to read" {
	 ObjList::reset
	 set objids [Face::loadFaces $::TEST_DIR/faces_file 5]
	 set num_read [llength $objids]
	 expr $num_read == 5 && $num_read == [ObjList::count] \
				&& [string compare [GrObj::type 0] "Face"] == 0
} {^1$}
test "FaceTcl-Face::loadFaces" "file read with virtual constructor" {
} {^$} $must_implement
test "FaceTcl-Face::loadFaces" "empty file read" {
	 ObjList::reset
	 set objids [Face::loadFaces $::TEST_DIR/empty_file]
	 set num_read [llength $objids]
	 expr $num_read == 0 && [ObjList::count] == 0
} {^1$}
test "FaceTcl-Face::loadFaces" "empty file read with limit on # to read" {
	 ObjList::reset
	 set objids [Face::loadFaces $::TEST_DIR/empty_file 5]
	 set num_read [llength $objids]
	 expr $num_read == 0 && [ObjList::count] == 0
} {^1$}
# On bad input, the error may be detected by either Face or IoMgr 
# depending on which constructor got called.
test "FaceTcl-Face::loadFaces" "error on non-existent file" {
	 exec rm -rf $::TEST_DIR/nonexistent_file
	 Face::loadFaces $::TEST_DIR/nonexistent_file
} {^Face::loadFaces: unable to open file$}
test "FaceTcl-Face::loadFaces" "error from junk text file" {
	 Face::loadFaces $::TEST_DIR/junk_text_file
} {^Face::loadFaces: (InputError|unable to create object of type).*$}
test "FaceTcl-Face::loadFaces" "error from junk binary file" {
	 Face::loadFaces $::TEST_DIR/junk_bin_file
} {^Face::loadFaces: (InputError|unable to create object of type).*$}

### Face::stringifyCmd ###
test "FaceTcl-Face::stringify" "too few args" {
    Face::stringify
} {wrong \# args: should be "Face::stringify item_id"}
test "FaceTcl-Face::stringify" "too many args" {
    Face::stringify j u
} {wrong \# args: should be "Face::stringify item_id"}
test "FaceTcl-Face::stringify" "error from bad objid" {
    Face::stringify -1
} {Face::stringify: an error of type InvalidIdError occurred}
test "FaceTcl-Face::stringify" "error from wrong type" {
    set fx [Fixpt::Fixpt]
	 Face::stringify $fx
} {Face::stringify: object not of correct type}

### Face::destringifyCmd ###
test "FaceTcl-Face::destringify" "too few args" {
    Face::destringify
} {wrong \# args: should be "Face::destringify item_id string"}
test "FaceTcl-Face::destringify" "too many args" {
    Face::destringify j u n
} {wrong \# args: should be "Face::destringify item_id string"}
test "FaceTcl-Face::destringify" "error from bad objid" {
    Face::destringify -1 junk
} {Face::destringify: an error of type InvalidIdError occurred}
test "FaceTcl-Face::destringify" "error from wrong type" {
    set fx [Fixpt::Fixpt]
	 Face::destringify $fx junk
} {Face::destringify: object not of correct type}
test "FaceTcl-Face::destringify" "stringify/destringify check" {
	 set faceid [Face::Face]
	 Face::noseLength $faceid -1.2
	 Face::mouthHeight $faceid -0.6
	 Face::eyeHeight $faceid 0.4
	 Face::eyeDistance $faceid 0.6
	 set str1 [Face::stringify $faceid]

	 set faceid2 [Face::Face]
	 Face::destringify $faceid2 $str1
	 set str2 [Face::stringify $faceid2]

	 expr [string compare $str1 $str2] == 0
} {^1$}

ObjList::reset

