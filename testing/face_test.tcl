##############################################################################
###
### FaceTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### faceCmd ###
test "FaceTcl-Face::face" "too many args" {
	 Face::face junk
} {wrong \# args: should be "Face::face"}
test "FaceTcl-Face::face" "normal face creation" {
	 Face::face
} {[0-9]+}
test "FaceTcl-Face::face" "no possible error" {} $BLANK $no_test

set EPS 0.00001 ;# floating-point precision required of next four commands
set faceid [Face::face] ;# guaranteed face id for next operations

### eyeHgtCmd ###
test "FaceTcl-eyeHgt" "too few args" {
	 Face::eyeHgt
} {^wrong \# args: should be "Face::eyeHgt item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-eyeHgt" "too many args" {
	 Face::eyeHgt j u n
} {^wrong \# args: should be "Face::eyeHgt item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-eyeHgt" "normal use set" {
	 Face::eyeHgt $::faceid 2.0 
} {^$}
test "FaceTcl-eyeHgt" "normal use query" {
	 set val1 1.234
	 Face::eyeHgt $::faceid $val1
	 set val2 [Face::eyeHgt $::faceid]
	 expr abs($val2-$val1) < $::EPS
} {^1$}
test "FaceTcl-eyeHgt" "error from nun-numeric input" {
    Face::eyeHgt $::faceid junk
} {expected floating-point number but got "junk"}
test "FaceTcl-eyeHgt" "error from bad objid" {
    Face::eyeHgt -1
} {Face::eyeHgt: objid out of range}

### eyeDistCmd ###
test "FaceTcl-eyeDist" "too few args" {
	 Face::eyeDist
} {^wrong \# args: should be "Face::eyeDist item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-eyeDist" "too many args" {
	 Face::eyeDist j u n
} {^wrong \# args: should be "Face::eyeDist item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-eyeDist" "normal use set" {
	 Face::eyeDist $::faceid 2.0 
} {^$}
test "FaceTcl-eyeDist" "normal use query" {
	 set val1 1.357
	 Face::eyeDist $::faceid $val1
	 set val2 [Face::eyeDist $::faceid]
	 expr abs($val2-$val1) < $::EPS
} {^1$}
test "FaceTcl-eyeDist" "error from nun-numeric input" {
    Face::eyeDist $::faceid junk
} {expected floating-point number but got "junk"}
test "FaceTcl-eyeDist" "error from bad objid" {
    Face::eyeDist -1
} {Face::eyeDist: objid out of range}

### noseLenCmd ###
test "FaceTcl-noseLen" "too few args" {
	 Face::noseLen
} {^wrong \# args: should be "Face::noseLen item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-noseLen" "too many args" {
	 Face::noseLen j u n
} {^wrong \# args: should be "Face::noseLen item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-noseLen" "normal use set" {
	 Face::noseLen $::faceid 2.0 
} {^$}
test "FaceTcl-noseLen" "normal use query" {
	 set val1 0.246
	 Face::noseLen $::faceid $val1
	 set val2 [Face::noseLen $::faceid]
	 expr abs($val2-$val1) < $::EPS
} {^1$}
test "FaceTcl-noseLen" "error from nun-numeric input" {
    Face::noseLen $::faceid junk
} {expected floating-point number but got "junk"}
test "FaceTcl-noseLen" "error from bad objid" {
    Face::noseLen -1
} {Face::noseLen: objid out of range}

### mouthHgtCmd ###
test "FaceTcl-mouthHgt" "too few args" {
	 Face::mouthHgt
} {^wrong \# args: should be "Face::mouthHgt item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-mouthHgt" "too many args" {
	 Face::mouthHgt j u n
} {^wrong \# args: should be "Face::mouthHgt item_id\(s\) \?new_value\(s\)\?"$}
test "FaceTcl-mouthHgt" "normal use set" {
	 Face::mouthHgt $::faceid 2.0 
} {^$}
test "FaceTcl-mouthHgt" "normal use query" {
	 set val1 9.876
	 Face::mouthHgt $::faceid $val1
	 set val2 [Face::mouthHgt $::faceid]
	 expr abs($val2-$val1) < $::EPS
} {^1$}
test "FaceTcl-mouthHgt" "error from nun-numeric input" {
    Face::mouthHgt $::faceid junk
} {expected floating-point number but got "junk"}
test "FaceTcl-mouthHgt" "error from bad objid" {
    Face::mouthHgt -1
} {Face::mouthHgt: objid out of range}

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
} {Face::stringify: objid out of range}
test "FaceTcl-Face::stringify" "error from wrong type" {
    set fx [Fixpt::fixpt]
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
} {Face::destringify: objid out of range}
test "FaceTcl-Face::destringify" "error from wrong type" {
    set fx [Fixpt::fixpt]
	 Face::destringify $fx junk
} {Face::destringify: object not of correct type}
test "FaceTcl-Face::destringify" "stringify/destringify check" {
	 set faceid [Face::face]
	 Face::noseLen $faceid -1.2
	 Face::mouthHgt $faceid -0.6
	 Face::eyeHgt $faceid 0.4
	 Face::eyeDist $faceid 0.6
	 set str1 [Face::stringify $faceid]

	 set faceid2 [Face::face]
	 Face::destringify $faceid2 $str1
	 set str2 [Face::stringify $faceid2]

	 expr [string compare $str1 $str2] == 0
} {^1$}

ObjList::reset

