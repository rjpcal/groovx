##############################################################################
#
# grshtest.tcl
# Rob Peters Apr-99
#
# This test script which is to be run on grsh. The spirit of this test file is 
# to test every nook and cranny of the C++ code that has been used to create
# various Tcl/Tk extension packages. In practice, this means making sure that
# functions exhibit expected behavior under normal conditions, as well as 
# testing all exits by TCL_ERROR.
#
# The following packages are tested:
#   ExptTcl
#   FaceTcl
#   FixptTcl
#   JitterTcl
#   MiscTcl
#   ObjlistTcl
#   ObjTogl
#   PositionTcl
#   PoslistTcl
#   SoundTcl
#   SubjectTcl
#   Tcldlist
#   TclGL
#   TlistTcl
#
##############################################################################

set START_CLICKS [clock clicks]

set VERBOSE 0
set TEST_DIR /cit/rjpeters/grsh/testing
set DATE [clock format [clock seconds] -format %H%M%d%b%Y]

set num_tests 0
set num_success 0

# Flags to pass into 'flags' argument of test.
set no_test 0
set must_implement -1
set skip_known_bug -2
set normal_test 1
set intermittent_bug 2
set test_serialize 0

# If the 'flags' argument is 0, the test is skipped.
proc test {package_name test_name script expected_result_regexp {flags 1}} {
	 if {$flags <= 0} { return }
	 if {$::VERBOSE != 0} {
		  puts "==== $package_name $test_name"
	 }
	 catch {eval $script} result
	 incr ::num_tests
	 if { [regexp $expected_result_regexp $result] == 1 } {
		  if {$::VERBOSE != 0} { 
				puts "---- $package_name $test_name succeeded\n"
		  }
		  incr ::num_success
	 } else {
		  if {$::VERBOSE == 0} {
				puts "==== $package_name $test_name"
		  }
		  puts "==== Contents of test case:"
		  puts $script
		  puts "==== Result was:"
		  puts $result
		  puts "---- Result should have been:"
		  puts $expected_result_regexp
		  puts "---- $package_name $test_name FAILED\n"
		  finish
	 }
}

proc finish {} {
	 set clicks_per_ms 1000
	 set ::END_CLICKS [clock clicks]
	 set elapsed_ms [expr ($::END_CLICKS-$::START_CLICKS) / $clicks_per_ms]
	 set per_test [expr $::num_tests ? ($elapsed_ms/$::num_tests) : 0]
	 set msg "$::num_success tests succeeded of $::num_tests tests attempted
total testing time $elapsed_ms msec, or $per_test msec per test
Testing complete."
    puts $msg
    after 10000 { event generate . <KeyPress> -keysym Return }
	 tk_messageBox -icon info -type ok -message $msg
	 exit
}

# test the test procedure
test test test { expr 2+2; } 4

# skeleton calls for new test procedures:
### commandCmd ###
test "package-command" "too few args" {} {^$}
test "package-command" "too many args" {} {^$}
test "package-command" "normal use" {} {^$}
test "package-command" "error" {} {^$}

# Helper RegExps
set SP {[ \t]+}
set BIT {[01]}
set HEX {[0-9a-f]+}
set INT {[0-9]+}
set FLT {[0-9]+\(\.[0-9]+\)}
set BLANK {^$}

##############################################################################
###
### FaceTcl
###
##############################################################################

### faceCmd ###
test "FaceTcl-face" "too many args" {
	 face junk
} {wrong \# args: should be "face"}
test "FaceTcl-face" "normal face creation" { face } {[0-9]+}
test "FaceTcl-face" "no possible error" {} $BLANK $no_test

set EPS 0.00001 ;# floating-point precision required of next four commands
set faceid [face] ;# guaranteed face id for next operations

### eyeHgtCmd ###
test "FaceTcl-eyeHgt" "too few args" {
	 Face::eyeHgt
} {^wrong \# args: should be "Face::eyeHgt faceid \[new_val\]"$}
test "FaceTcl-eyeHgt" "too many args" {
	 Face::eyeHgt j u n
} {^wrong \# args: should be "Face::eyeHgt faceid \[new_val\]"$}
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
} {^wrong \# args: should be "Face::eyeDist faceid \[new_val\]"$}
test "FaceTcl-eyeDist" "too many args" {
	 Face::eyeDist j u n
} {^wrong \# args: should be "Face::eyeDist faceid \[new_val\]"$}
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
} {^wrong \# args: should be "Face::noseLen faceid \[new_val\]"$}
test "FaceTcl-noseLen" "too many args" {
	 Face::noseLen j u n
} {^wrong \# args: should be "Face::noseLen faceid \[new_val\]"$}
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
} {^wrong \# args: should be "Face::mouthHgt faceid \[new_val\]"$}
test "FaceTcl-mouthHgt" "too many args" {
	 Face::mouthHgt j u n
} {^wrong \# args: should be "Face::mouthHgt faceid \[new_val\]"$}
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
resetObjList
unset EPS

### Face::loadFacesCmd ###
test "FaceTcl-Face::loadFaces" "too few args" { 
	 Face::loadFaces
} {wrong \# args: should be "Face::loadFaces\
		  filename \?num_to_read\? \?first_id\? \?use_virtual_ctor\?"}
test "FaceTcl-Face::loadFaces" "too many args" { 
	 Face::loadFaces j u n k y
} {wrong \# args: should be "Face::loadFaces\
		  filename \?num_to_read\? \?first_id\? \?use_virtual_ctor\?"}
test "FaceTcl-Face::loadFaces" "normal file read with no comments" {
	 resetObjList
	 set num_read [Face::loadFaces $::TEST_DIR/faces_file_no_comments]
	 expr $num_read == 10 && $num_read == [objCount] \
				&& [string compare [objType 0] "Face"] == 0
} {^1$}
test "FaceTcl-Face::loadFaces" "normal file read" {
	 resetObjList
	 set num_read [Face::loadFaces $::TEST_DIR/faces_file]
	 expr $num_read == 10 && $num_read == [objCount] \
				&& [string compare [objType 0] "Face"] == 0
} {^1$}
test "FaceTcl-Face::loadFaces" "normal file read with limit on # to read" {
	 resetObjList
	 set num_read [Face::loadFaces $::TEST_DIR/faces_file 5]
	 expr $num_read == 5 && $num_read == [objCount] \
				&& [string compare [objType 0] "Face"] == 0
} {^1$}
test "FaceTcl-Face::loadFaces" "normal file read with starting offset" {
	 resetObjList
	 set num_read [Face::loadFaces $::TEST_DIR/faces_file -1 3]
	 expr $num_read == 10 && $num_read == [objCount] \
				&& [string compare [objType 3] "Face"] == 0
} {^1$}
test "FaceTcl-Face::loadFaces" "file read with virtual constructor" {
} {^$} $must_implement
test "FaceTcl-Face::loadFaces" "empty file read" {
	 resetObjList
	 set num_read [Face::loadFaces $::TEST_DIR/empty_file]
	 expr $num_read == 0 && [objCount] == 0
} {^1$}
test "FaceTcl-Face::loadFaces" "empty file read with limit on # to read" {
	 resetObjList
	 set num_read [Face::loadFaces $::TEST_DIR/empty_file 5]
	 expr $num_read == 0 && [objCount] == 0
} {^1$}
# On bad input, the error may be detected by either Face or IoMgr 
# depending on which constructor got called.
test "FaceTcl-Face::loadFaces" "error on non-existent file" {
	 exec rm -rf $::TEST_DIR/nonexistent_file
	 Face::loadFaces $::TEST_DIR/nonexistent_file
} {^Face::loadFaces: unable to open file$}
test "FaceTcl-Face::loadFaces" "error from junk text file" {
	 Face::loadFaces $::TEST_DIR/junk_text_file
} {^Face::loadFaces: InputError: (Face|IoMgr.*)$}
test "FaceTcl-Face::loadFaces" "error from junk binary file" {
	 Face::loadFaces $::TEST_DIR/junk_bin_file
} {^Face::loadFaces: InputError: (Face|IoMgr.*)$}

### Face::stringifyCmd ###
test "FaceTcl-Face::stringify" "too few args" {
    Face::stringify
} {wrong \# args: should be "Face::stringify faceid"}
test "FaceTcl-Face::stringify" "too many args" {
    Face::stringify j u
} {wrong \# args: should be "Face::stringify faceid"}
test "FaceTcl-Face::stringify" "error from bad objid" {
    Face::stringify -1
} {Face::stringify: objid out of range}
test "FaceTcl-Face::stringify" "error from wrong type" {
    set fx [fixpt]
	 Face::stringify $fx
} {Face::stringify: object not of correct type}

### Face::destringifyCmd ###
test "FaceTcl-Face::destringify" "too few args" {
    Face::destringify
} {wrong \# args: should be "Face::destringify faceid string"}
test "FaceTcl-Face::destringify" "too many args" {
    Face::destringify j u n
} {wrong \# args: should be "Face::destringify faceid string"}
test "FaceTcl-Face::destringify" "error from bad objid" {
    Face::destringify -1 junk
} {Face::destringify: objid out of range}
test "FaceTcl-Face::destringify" "error from wrong type" {
    set fx [fixpt]; Face::destringify $fx junk
} {Face::destringify: object not of correct type}
test "FaceTcl-Face::destringify" "stringify/destringify check" {
	 set faceid [face]
	 Face::noseLen $faceid -1.2
	 Face::mouthHgt $faceid -0.6
	 Face::eyeHgt $faceid 0.4
	 Face::eyeDist $faceid 0.6
	 set str1 [Face::stringify $faceid]

	 set faceid2 [face]
	 Face::destringify $faceid2 $str1
	 set str2 [Face::stringify $faceid2]

	 expr [string compare $str1 $str2] == 0
} {^1$}

resetObjList

##############################################################################
###
### FixptTcl
###
##############################################################################

### fixptCmd ###
test "FixptTcl-fixpt" "no args limit" {} $BLANK $no_test
test "FixptTcl-fixpt" "normal create" {
	 fixpt
} {^[0-9]+$}
test "FixptTcl-fixpt" "error from bad subcommand" {
	 fixpt junk
} {^fixpt: invalid command$}

set fix [fixpt]

### lengthCmd ###
test "FixptTcl-length" "too few args" {
    fixpt length
} {^wrong \# args: should be "fixpt length objid length_val"$}
test "FixptTcl-length" "too many args" {
    fixpt length j u n
} {^wrong \# args: should be "fixpt length objid length_val"$}
test "FixptTcl-length" "normal set" {
	 catch {fixpt length $::fix 1.0}
} {^0$}
test "FixptTcl-length" "error from non-numeric input" {
    fixpt length $::fix junk
} {^expected floating-point number but got "junk"$}
test "FixptTcl-length" "error from bad objid" {
    fixpt length -1 1.0
} {^fixpt: objid out of range$}
test "FixptTcl-length" "error from wrong type" {
    set f [face]; fixpt length $f 1.0
} {^fixpt: object not of correct type$}

### widthCmd ###
test "FixptTcl-width" "too few args" {
    fixpt width
} {wrong \# args: should be "fixpt width objid width_val"}
test "FixptTcl-width" "too many args" {
    fixpt width j u n
} {wrong \# args: should be "fixpt width objid width_val"}
test "FixptTcl-width" "normal set" {
	 catch {fixpt width $::fix 2}
} {^0$}
test "FixptTcl-width" "error from non-numeric input" {
    fixpt width $::fix junk
} {expected integer but got "junk"}
test "FixptTcl-width" "error from bad obid" {
    fixpt width -1 junk
} {fixpt: objid out of range}
test "FixptTcl-width" "error from wrong type" {
    set f [face]; fixpt width $f 1.0
} {fixpt: object not of correct type}
test "FixptTcl-width" "error from non-integral number" {
    fixpt width $::fix 1.5
} {expected integer but got "1\.5"}

resetObjList
unset fix

##############################################################################
###
### JitterTcl
###
##############################################################################

### jitterCmd ###
test "JitterTcl-jitter" "too many args" {
	 jitter junk
} {wrong \# args: should be "jitter"}
test "JitterTcl-jitter" "normal create" {jitter;} {[0-9]+}
test "JitterTcl-jitter" "no error" {} $BLANK $no_test

set jit [jitter]

### setJitterCmd ###
test "JitterTcl-setJitter" "too few args" {
    setJitter
} {wrong \# args: should be "setJitter posid x_jitter y_jitter r_jitter"}
test "JitterTcl-setJitter" "too many args" {
    setJitter j u n k y
} {wrong \# args: should be "setJitter posid x_jitter y_jitter r_jitter"}
test "JitterTcl-setJitter" "normal use" {
	 catch {setJitter $::jit 1.0 1.0 1.0}
} {^0$}
test "JitterTcl-setJitter" "error from non-numeric input" {
    setJitter $::jit junk 1.0 1.0
} {expected floating-point number but got "junk"}
test "JitterTcl-setJitter" "error from wrong type" {
    set p [Pos::position]; setJitter $p 1 1 1
} {setJitter: position not of type jitter}

resetPosList
unset jit


##############################################################################
###
### MiscTcl
###
##############################################################################

### randCmd ###
test "MiscTcl-rand" "too few args" {
	 rand
} {wrong \# args: should be "rand min max"}
test "MiscTcl-rand" "too many args" {
	 rand j u n
} {wrong \# args: should be "rand min max"}
test "MiscTcl-rand" "normal use" {
	 srand [clock seconds]
	 set val [rand 0 1]
	 expr $val >= 0 && $val < 1
} {^1$}
test "MiscTcl-rand" "error from non-numeric input" {
    rand junk 0
} {expected floating-point number but got "junk"}

### srandCmd ###
test "MiscTcl-srand" "too few args" {
	 srand
} {wrong \# args: should be "srand seed"}
test "MiscTcl-srand" "too few args" {
	 srand j u
} {wrong \# args: should be "srand seed"}
test "MiscTcl-srand" "normal use" {
	 srand 10
	 expr [rand 0 1] == 0.138641357422
} {^1$}
test "MiscTcl-srand" "error from non-numeric input" {
    srand junk
} {expected integer but got "junk"}
test "MiscTcl-srand" "error from non-integral number" {
    srand 1.5
} {expected integer but got "1\.5"}

### sleepCmd ###
test "MiscTcl-sleep" "too few args" {
	 # The command should exit with error, but no message
	 set exit_code [catch {sleep} res]
	 expr $exit_code == 1 && [string compare $res ""] == 0
} {^1$}
test "MiscTcl-sleep" "too many args" {
	 # The command should exit with error, but no message
	 set exit_code [catch {sleep j u} res]
	 expr $exit_code == 1 && [string compare $res ""] == 0
} {^1$}
test "MiscTcl-sleep" "normal use" {
	 set res [time {sleep 1}]
	 set us [lindex $res 0]
	 expr $us > 900000 && $us < 1500000
} {^1$}
test "MiscTcl-sleep" "error from negative input" {
	 # The command should exit with error; thus it should be fast
	 set exit_code [catch {sleep -1}]
	 set res [time {catch {sleep -1}} 100]
	 set us [lindex $res 0]
	 expr $exit_code == 1 && $us > 0 && $us < 250
} {^1$}
	 
### usleepCmd ###
test "MiscTcl-usleep" "too few args" {
	 # The command should exit with error, but no message
	 set exit_code [catch {usleep} res]
	 expr $exit_code == 1 && [string compare $res ""] == 0
} {^1$}
test "MiscTcl-usleep" "too many args" {
	 # The command should exit with error, but no message
	 set exit_code [catch {usleep j u} res]
	 expr $exit_code == 1 && [string compare $res ""] == 0
} {^1$}
test "MiscTcl-usleep" "normal use" {
	 set res [time {usleep 10000}]
	 set us [lindex $res 0]
	 expr $us > 10000 && $us < 25000
} {^1$}
test "MiscTcl-usleep" "error from negative input" {
	 # The command should exit with error; thus it should be fast
	 set exit_code [catch {usleep -1}]
	 set res [time {catch {usleep -1}} 100]
	 set us [lindex $res 0]
	 expr $exit_code == 1 && $us > 0 && $us < 250
} {^1$}

### usleeprCmd ###
test "MiscTcl-usleepr" "too few args" {
	 # The command should exit with error, but no message
	 set exit_code [catch {usleepr} res]
	 expr $exit_code == 1 && [string compare $res ""] == 0
} {^1$}
test "MiscTcl-usleepr" "too many args" {
	 # The command should exit with error, but no message
	 set exit_code [catch {usleepr j u n} res]
	 expr $exit_code == 1 && [string compare $res ""] == 0
} {^1$}
test "MiscTcl-usleepr" "normal use" {
	 set res [time {usleepr 100 10}]
	 set us [lindex $res 0]
	 expr $us > 0 && $us < 250000
} {^1$}
test "MiscTcl-usleepr" "error from negative input" {
	 # The command should exit with error; thus it should be fast
	 set exit_code [catch {usleepr -1 -1}]
	 set res [time {catch {usleepr -1 -1}} 100]
	 set us [lindex $res 0]
	 expr $exit_code == 1 && $us > 0 && $us < 250
} {^1$}

##############################################################################
###
### ObjlistTcl
###
##############################################################################

### regexp to match ObjList in serializations
set OBJLIST {ObjList|PtrList<GrObj>}
	 
### resetObjListCmd ###
test "ObjlistTcl-resetObjList" "too many args" {
    resetObjList junk
} {wrong \# args: should be "resetObjList"}
test "ObjlistTcl-resetObjList" "check number of objects -> 0" {
	 face
	 fixpt
	 resetObjList
	 objCount
} {^0$}
test "ObjlistTcl-resetObjList" "check first vacant -> 0" {
	 face
	 fixpt
	 resetObjList
	 face
} {^0$}
test "ObjlistTcl-resetObjList" "no error" {} $BLANK $no_test

### objCountCmd ###
test "ObjlistTcl-objCount" "too many args" {
    objCount junk
} {wrong \# args: should be "objCount"}
test "ObjlistTcl-objCount" "normal use" {
	 resetObjList
	 face
	 fixpt
	 objCount
} {^2$}
test "ObjlistTcl-objCount" "no error" {} $BLANK $no_test

### objTypeCmd ###
test "ObjlistTcl-objType" "too few args" {
    objType
} {wrong \# args: should be "objType objid"}
test "ObjlistTcl-objType" "too many args" {
    objType j u
} {wrong \# args: should be "objType objid"}
test "ObjlistTcl-objType" "normal use on Face" {
	 set f [face]
	 objType $f
} {Face}
test "ObjlistTcl-objType" "normal use on FixPt" {
	 set f [fixpt]
	 objType $f
} {FixPt}
test "ObjlistTcl-objType" "error from bad objid" {
	 objType -1
} {objType: objid out of range}
test "ObjlistTcl-objType" "error from too large objid" {
	 objType 10000
} {objType: objid out of range}

### ObjList::stringifyCmd ###
test "ObjlistTcl-ObjList::stringify" "too many args" {
    ObjList::stringify junk
} {wrong \# args: should be "ObjList::stringify"}
test "ObjlistTcl-ObjList::stringify" "use on empty list" {
    resetObjList; ObjList::stringify
} {PtrList<GrObj> 100 0
0}
test "ObjlistTcl-ObjList::stringify" "use on filled list" {
	 resetObjList
	 set f [face]
	 set f [fixpt]
	 ObjList::stringify
} "$OBJLIST $INT 2
0 Face .*
1 FixPt .*
2"
test "ObjlistTcl-ObjList::stringify" "no error" {} $BLANK $no_test
	 
### ObjList::destringifyCmd ###
test "ObjlistTcl-ObjList::destringify" "args" {
    ObjList::destringify
} {wrong \# args: should be "ObjList::destringify string"}
test "ObjlistTcl-ObjList::destringify" "use with filled list" {
	 resetObjList
	 face
	 fixpt
	 set a [ObjList::stringify]
	 ObjList::destringify $a
	 expr [objCount]==2 \
				&& ([string compare [objType 0] Face] == 0) \
				&& ([string compare [objType 1] FixPt] == 0)
} {^1$}
test "ObjlistTcl-ObjList::destringify" "stringify, destringify, restringify" {
	 resetObjList
	 face
	 fixpt
	 set a [ObjList::stringify]
	 ObjList::destringify $a
	 set b [ObjList::stringify]
	 expr [string compare $a $b] == 0
} {^1$}
test "ObjlistTcl-ObjList::destringify" "use with empty list" {
	 resetObjList
	 set a [ObjList::stringify]
	 ObjList::destringify $a
	 expr [objCount]==0
} {^1$}
test "ObjlistTcl-ObjList::destringify" "error on incomplete input" {
    ObjList::destringify "ObjList"
} {ObjList::destringify: InputError: ObjList}
test "ObjlistTcl-ObjList::destringify" "error on bad input" {
    ObjList::destringify "this is a bunch of bs"
} {ObjList::destringify: InputError: ObjList}


##############################################################################
###
### ObjTogl
###
##############################################################################

### toglCreateCallback ###
test "ObjTogl-toglCreateCallback" "test widget create" {
	 togl .togl -rgba false
	 set ::TOGL_WIDTH [lindex [.togl configure -width] 4 ]
	 set ::TOGL_HEIGHT [lindex [.togl configure -height] 4 ]
	 pack .togl
	 update
} {^$}

### dumpCmapCmd ###
test "ObjTogl-dumpCmap" "too many args" {
    .togl dumpCmap junk junk junk
} {wrong \# args: should be "pathname dumpCmap \?start_index=0\? \?end_index=255\?"}
test "ObjTogl-dumpCmap" "normal use" {
    .togl dumpCmap 0 3
} "0$SP$HEX$SP$HEX$SP$HEX
1$SP$HEX$SP$HEX$SP$HEX
2$SP$HEX$SP$HEX$SP$HEX
3$SP$HEX$SP$HEX$SP$HEX"
test "ObjTogl-dumpCmap" "error from non-integral number arg1" {
    .togl dumpCmap 0.5 1
} {expected integer but got "0\.5"}
test "ObjTogl-dumpCmap" "error from non-integral number arg2" {
    .togl dumpCmap 0 1.5
} {expected integer but got "1\.5"}
test "ObjTogl-dumpCmap" "error from too small index" {
    .togl dumpCmap -3 -1
} {dumpCmap: colormap index out of range}
test "ObjTogl-dumpCmap" "error from too large index" {
    .togl dumpCmap 100 900
} {dumpCmap: colormap index out of range}

### dumpEpsCmd ###
test "ObjTogl-dumpEps" "too few args" {
    .togl dumpEps
} {wrong \# args: should be "pathname dumpEps filename"}
test "ObjTogl-dumpEps" "too few args" {
    .togl dumpEps j u
} {wrong \# args: should be "pathname dumpEps filename"}
test "ObjTogl-dumpEps" "normal use" {} $BLANK $no_test
test "ObjTogl-dumpEps" "no error" {} $BLANK $no_test

### scaleRectCmd ###
test "ObjTogl-scaleRect" "too few args" {
    .togl scaleRect
} {wrong \# args: should be "pathname scaleRect scale"}
test "ObjTogl-scaleRect" "too few args" {
    .togl scaleRect j u
} {wrong \# args: should be "pathname scaleRect scale"}
test "ObjTogl-scaleRect" "normal use on integer" {
	 catch {.togl scaleRect 1}
} {^0$}
test "ObjTogl-scaleRect" "normal use on floating point" {
	 catch {.togl scaleRect 1.5}
} {^0$}
test "ObjTogl-scaleRect" "error on scale by zero" {
    .togl scaleRect 0
} {scaleRect: invalid scaling factor}
test "ObjTogl-scaleRect" "error on negative scale factor" {
    .togl scaleRect -1.2
} {scaleRect: invalid scaling factor}

### setColorCmd ###
test "ObjTogl-setColor" "too few args" {
    .togl setColor
} {wrong \# args: should be "pathname setColor index r g b"}
test "ObjTogl-setColor" "too few args" {
    .togl setColor j u n k y
} {wrong \# args: should be "pathname setColor index r g b"}
test "ObjTogl-setColor" "normal use" {
	 # can only try this command if we have a private colormap
	 set pc [lindex [.togl configure -privatecmap] 5]
	 if {$pc == 1} { 
		  .togl setColor 0 0.5 0.5 0.5
	 }
} {^$}
test "ObjTogl-setColor" "error" {
	 catch {.togl setColor -1 0.5 0.5 0.5;} str
	 string compare $str "setColor: colormap index out of range"
} {^0$}

### setFixedScaleCmd ###
test "ObjTogl-setFixedScale" "args" {
    .togl setFixedScale
} {wrong \# args: should be "pathname setFixedScale scale"}
test "ObjTogl-setFixedScale" "norm1" {
	 catch {.togl setFixedScale 1}
} {^0$}
test "ObjTogl-setFixedScale" "norm2" {
	 catch {.togl setFixedScale 1.5}
} {^0$}
test "ObjTogl-setFixedScale" "err1" {
    .togl setFixedScale 0
} {setFixedScale: invalid scaling factor}
test "ObjTogl-setFixedScale" "err2" {
    .togl setFixedScale -1.2
} {setFixedScale: invalid scaling factor}

### setMinRectCmd ###
test "ObjTogl-setMinRect" "args" {
    .togl setMinRect
} {wrong \# args: should be "pathname setMinRect left top right bottom"}
test "ObjTogl-setMinRect" "normal use" {
	 catch {.togl setMinRect -1.3 15.2 2.6 1.3}
} {^0$}
test "ObjTogl-setMinRect" "err1" {
    .togl setMinRect -1 0 1 0
} {setMinRect: invalid rect}
test "ObjTogl-setMinRect" "err2" {
    .togl setMinRect 0 1 0 -1
} {setMinRect: invalid rect}


### setUnitAngleCmd ###
test "ObjTogl-setUnitAngle" "args" {
    .togl setUnitAngle
} {wrong \# args: should be "pathname setUnitAngle angle_in_degrees"}
test "ObjTogl-setUnitAngle" "normal use" {
	 catch {.togl setUnitAngle 2.3}
} {^0$}
test "ObjTogl-setUnitAngle" "err1" {
    .togl setUnitAngle 0
} {setUnitAngle: invalid value of unit angle}
test "ObjTogl-setUnitAngle" "err2" {
    .togl setUnitAngle -1.5
} {setUnitAngle: invalid value of unit angle}

### setViewingDistanceCmd ###
test "ObjTogl-setViewingDistance" "args" {
    .togl setViewingDistance
} {wrong \# args: should be "pathname setViewingDistance distance_in_inches"}
test "ObjTogl-setViewingDistance" "normal use" {
	 catch {.togl setViewingDistance 60}
} {^0$}
test "ObjTogl-setViewingDistance" "err1" {
    .togl setViewingDistance 0
} {setViewingDistance: look out! you'll hurt your nose trying\
		  to view the screen from that distance! try another value}
test "ObjTogl-setViewingDistance" "err2" {
    .togl setViewingDistance -1
} {setViewingDistance: look out! you'll hurt your nose trying\
		  to view the screen from that distance! try another value}
test "ObjTogl-setViewingDistance" "err3" {
    .togl setViewingDistance 1001
} {setViewingDistance: if you really insist on trying to view the\
		  screen from so far away, you should really invest in a good telescope!\
		  try another value}

### toglDestroyCallback ###
test "ObjTogl-toglDestroyCallback" "test with destroy widget" {
	 togl .togl2 -rgba false
	 catch {destroy .togl2}
} {^0$}

##############################################################################
###
### PositionTcl
###
##############################################################################

### Pos::positionCmd ###
test "PositionTcl-Pos::position" "too many args" {
    Pos::position junk
} {wrong \# args: should be "Pos::position"}
test "PositionTcl-Pos::position" "normal create" {
	 Pos::position
} {^[0-9]+$}
test "PositionTcl-Pos::position" "no error" {} $BLANK $no_test

set pos [Pos::position]

### Pos::rotateCmd ###
test "PositionTcl-Pos::rotate" "too few args" {
    Pos::rotate
} {wrong \# args: should be "Pos::rotate posid angle \?rot_axis_x rot_axis_y rot_axis_z\?"}
test "PositionTcl-Pos::rotate" "illegal arg count" {
    Pos::rotate posid angle junk
} {wrong \# args: should be "Pos::rotate posid angle \?rot_axis_x rot_axis_y rot_axis_z\?"}
test "PositionTcl-Pos::rotate" "too many" {
    Pos::rotate posid angle j u n k
} {wrong \# args: should be "Pos::rotate posid angle \?rot_axis_x rot_axis_y rot_axis_z\?"}
test "PositionTcl-Pos::rotate" "normal use with angle only" {
	 catch {Pos::rotate $::pos 3.0}
} {^0$}
test "PositionTcl-Pos::rotate" "normal use with angle and axis of rotation" {
	 catch {Pos::rotate $::pos 3.0 1.5 0.0 -2.2}
} {^0$}
test "PositionTcl-Pos::rotate" "error on non-numeric input" {
    Pos::rotate $::pos junk
} {expected floating-point number but got "junk"}
test "PositionTcl-Pos::rotate" "error on bad posid" {
    Pos::rotate -1 1 1 1 1
} {Pos::rotate: posid out of range}

### scaleCmd ###
test "PositionTcl-Pos::scale" "too few args" {
    Pos::scale posid
} {wrong \# args: should be "Pos::scale posid scale_x \?scale_y\? \?scale_z\?"}
test "PositionTcl-Pos::scale" "too many args" {
    Pos::scale posid j u n k
} {wrong \# args: should be "Pos::scale posid scale_x \?scale_y\? \?scale_z\?"}
test "PositionTcl-Pos::scale" "normal use" {
	 catch {Pos::scale $::pos 1.5 3.0 4.5}
} {^0$}
test "PositionTcl-Pos::scale" "error on non-numeric input" {
    Pos::scale $::pos junk
} {expected floating-point number but got "junk"}
test "PositionTcl-Pos::scale" "error on bad posid" {
    Pos::scale -1 -1.2 1.0 1.0
} {Pos::scale: posid out of range}

### Pos::translateCmd ###
test "PositionTcl-Pos::translate" "too few args" {
    Pos::translate posid x
} {wrong \# args: should be "Pos::translate posid new_x new_y \?new_z\?"}
test "PositionTcl-Pos::translate" "too many args" {
    Pos::translate posid x y z junk
} {wrong \# args: should be "Pos::translate posid new_x new_y \?new_z\?"}
test "PositionTcl-Pos::translate" "normal use" {
	 catch {Pos::translate $::pos 10.0 -3.6 9.4}
} {^0$}
test "PositionTcl-Pos::translate" "error on non-numeric input" {
    Pos::translate $::pos junk junk junk
} {expected floating-point number but got "junk"}
test "PositionTcl-Pos::translate" "error on bad posid" {
    Pos::translate -1 1 1 1
} {Pos::translate: posid out of range}

### Pos::stringifyCmd ###
test "PositionTcl-Pos::stringify" "too few args" {
    Pos::stringify
} {wrong \# args: should be "Pos::stringify posid"}
test "PositionTcl-Pos::stringify" "too many args" {
    Pos::stringify posid junk
} {wrong \# args: should be "Pos::stringify posid"}
test "PositionTcl-Pos::stringify" "normal use" {
	 Pos::rotate $::pos 3.0 1.5 0.0 -2.2
	 Pos::scale $::pos 1.0 2.5 4.0
	 Pos::translate $::pos 5.3 10.6 15.9
	 Pos::stringify $::pos
} {Position 5\.3 10\.6 15\.9 1 2\.5 4 1\.5 0 -2\.2 3}
test "PositionTcl-Pos::stringify" "error on bad posid" {
    Pos::stringify -1
} {Pos::stringify: posid out of range}
test "PositionTcl-Pos::stringify" "error on non-integral posid" {
    Pos::stringify 1.5
} {expected integer but got "1\.5"}
	 
resetPosList
unset pos


##############################################################################
###
### PoslistTcl
###
##############################################################################

### regexp to match PosList in serializations
set POSLIST {PosList|PtrList<Position>}

### resetPosListCmd ###
test "PoslistTcl-resetPosList" "too many args" {
    resetPosList junk
} {wrong \# args: should be "resetPosList"}
test "PoslistTcl-resetPosList" "check number of positions -> 0" {
	 Pos::position
	 jitter
	 resetPosList
	 posCount
} {^0$}
test "PoslistTcl-resetPosList" "check first vacant -> 0" {
	 Pos::position
	 jitter
	 resetPosList
	 Pos::position
} {^0$}
test "PoslistTcl-resetPosList" "no error" {} $BLANK $no_test

### posCountCmd ###
test "PoslistTcl-posCount" "args" {
    posCount junk
} {wrong \# args: should be "posCount"}
test "PoslistTcl-posCount" "normal use" { 
	 resetPosList
	 Pos::position
	 jitter
	 posCount
} {^2$}
test "PoslistTcl-posCount" "no error" {} $BLANK $no_test

### posTypeCmd ###
test "PoslistTcl-posType" "args" {
    posType
} {wrong \# args: should be "posType posid"}
test "PoslistTcl-posType" "normal use on Position" { 
	 set f [Pos::position]
	 posType $f
} {Position}
test "PoslistTcl-posType" "normal use on Jitter" {
	 set f [jitter]
	 posType $f
} {Jitter}
test "PoslistTcl-posType" "error on too small posid" {
	 posType -1
} {posType: posid out of range}
test "PoslistTcl-posType" "error on too large" {
	 posType 10000
} {posType: posid out of range}

### PosList::stringifyCmd ###
test "PoslistTcl-PosList::stringify" "args" {
    PosList::stringify junk
} {wrong \# args: should be "PosList::stringify"}
test "PoslistTcl-PosList::stringify" "stringify a non-empty PosList" {
	 resetPosList
	 set p [Pos::position]
	 Pos::rotate $p 3.0 1.5 0.0 -2.2
	 Pos::scale $p 1.0 2.5 4.0
	 Pos::translate $p 5.3 10.6 15.9
	 set j [jitter]
	 Pos::rotate $j 3.0 1.5 0.0 -2.2
	 Pos::scale $j 1.0 2.5 4.0
	 Pos::translate $j 5.3 10.6 15.9
	 setJitter $j 1.5 3.0 4.5
	 PosList::stringify
} "$POSLIST $INT 2
0 Position 5\.3 10\.6 15\.9 1 2\.5 4 1\.5 0 -2\.2 3
1 Jitter 1\.5 3 4\.5
Position 5\.3 10\.6 15\.9 1 2\.5 4 1\.5 0 -2\.2 3
2"

test "PoslistTcl-PosList::stringify" "stringify an empty PosList" {
	 resetPosList
	 PosList::stringify
} "$POSLIST $INT 0
0"
test "PoslistTcl-PosList::stringify" "no error" {} $BLANK $no_test

### PosList::destringifyCmd ###
test "PoslistTcl-PosList::destringify" "args" {
    PosList::destringify
} {wrong \# args: should be "PosList::destringify string"}
test "PoslistTcl-PosList::destringify" "normal use" {
	 resetPosList
	 set p [Pos::position]
	 Pos::rotate $p 3.0 1.5 0.0 -2.2
	 Pos::scale $p 1.0 2.5 4.0
	 Pos::translate $p 5.3 10.6 15.9
	 set j [jitter]
	 Pos::rotate $j 3.0 1.5 0.0 -2.2
	 Pos::scale $j 1.0 2.5 4.0
	 Pos::translate $j 5.3 10.6 15.9
	 setJitter $j 1.5 3.0 4.5
	 set str [PosList::stringify]
	 resetPosList
	 PosList::destringify $str
	 expr [posCount]==2 \
				&& ([string compare [posType 0] Position] == 0) \
				&& ([string compare [posType 1] Jitter] == 0)
} {^1$}
test "PoslistTcl-PosList::destringify" "error" {
    PosList::destringify "this is all just a bunch of bunk"
} {PosList::destringify: InputError: PosList}


##############################################################################
###
### SubjectTcl
###
##############################################################################

### subjectCmd ###
test "SubjectTcl-subject" "args" {
    subject
} {wrong \# args: should be "subject subcommand \?args\.\.\.\?"}
test "SubjectTcl-subject" "normal use" {} $BLANK $no_test
test "SubjectTcl-subject" "error" {
    subject junk
} {subject: invalid command} 

### newSubjectCmd ###
test "SubjectTcl-newSubject" "args" {
    subject new
} {wrong \# args: should be "subject new subjectKey \?subjectName\? \?subjectDir\?"}
test "SubjectTcl-newSubject" "norm1" {
	 catch {subject new rjp1}
} {^0$}
test "SubjectTcl-newSubject" "norm2" {
	 catch {subject new rjp2 Rob}
} {^0$}
test "SubjectTcl-newSubject" "norm3" {
	 catch {subject new rjp3 Rob ~/mydir/}
} {^0$}
test "SubjectTcl-newSubject" "error" {
    subject new rep; subject new rep
} {subject: subject already exists}

### subjectCmd ###
test "SubjectTcl-clear" "args" {
    subject
} {wrong \# args: should be "subject subcommand \?args\.\.\.\?"}
test "SubjectTcl-clear" "normal use" { 
	 subject new s1
	 subject new s2
	 subject clear
	 subject list
} {^$}
test "SubjectTcl-clear" "no error" {} $BLANK $no_test

### subjectNameCmd ###
subject new nt name_test
test "SubjectTcl-subjectName" "args" {
    subject name
} {wrong \# args: should be "subject name subjectKey \?subjectName\?"}
test "SubjectTcl-subjectName" "norm1" {subject name nt;} {name_test}
test "SubjectTcl-subjectName" "norm2" {
	 subject name nt new_name
	 subject name nt
} {new_name}
test "SubjectTcl-subjectName" "error" {
    subject name junk
} {subject: no such subject exists}

### subjectDirCmd ###
subject new dt dt_name /dir_test/
test "SubjectTcl-subjectDir" "args" {
    subject dir
} {wrong \# args: should be "subject dir subjectKey \?subjectDir\?"}
test "SubjectTcl-subjectDir" "norm1" {subject dir dt;} {/dir_test/}
test "SubjectTcl-subjectDir" "norm2" {
	 subject dir dt /new_dir/
	 subject dir dt
} {/new_dir/}
test "SubjectTcl-subjectDir" "error" {
	 subject dir junk
} {subject: no such subject exists}

### listSubjectsCmd ###
test "SubjectTcl-listSubjects" "args" {
    subject list junk
} {wrong \# args: should be "subject list"}
test "SubjectTcl-listSubjects" "normal use" {
	 subject clear
	 subject new s1 subj1 /dir1/
	 subject new s2 subj2 /dir2/
	 subject list
} {s1[ \t]+subj1[ \t]+/dir1/
s2[ \t]+subj2[ \t]+/dir2/}
test "SubjectTcl-listSubjects" "no error" {} $BLANK $no_test
	 

##############################################################################
###
### Tcldlist
###
##############################################################################

### dlist_chooseCmd ###
test "Tcldlist-dlist_choose" "args" {
	 dlist_choose dlist_choose
} {wrong \# args: should be "dlist_choose source_list index_list"}
test "Tcldlist-dlist_choose" "norm1" { 
	 catch {dlist_choose {} {}}
} {^0$}
test "Tcldlist-dlist_choose" "norm2" { 
	 dlist_choose { 3.1 5 7 } { 2 0 1 }
} {7 3\.1 5}
test "Tcldlist-dlist_choose" "err1" {
     dlist_choose {1 2} {-1 0} 
} {dlist_choose: index out of range}
test "Tcldlist-dlist_choose" "err2" {
     dlist_choose {1 2} {1.5 0} 
} {expected integer but got "1\.5"}

### dlist_notCmd ###
test "Tcldlist-dlist_not" "args" {
    dlist_not
} {wrong \# args: should be "dlist_not source_list"}
test "Tcldlist-dlist_not" "norm1" { 
	 catch {dlist_not {}}
} {^0$}
test "Tcldlist-dlist_not" "norm2" { 
	 dlist_not { -1 0 1 349 }
} {0 1 0 0}
test "Tcldlist-dlist_not" "error" {
     dlist_not { -1.5 0.3 }
} {expected integer but got "-1\.5"}

### dlist_onesCmd ###
test "Tcldlist-dlist_ones" "args" {
    dlist_ones
} {wrong \# args: should be "dlist_ones num_ones"}
test "Tcldlist-dlist_ones" "norm1" { 
	 catch {dlist_ones 0}
} {^0$}
test "Tcldlist-dlist_ones" "norm2" { 
	 dlist_ones 5
} {1 1 1 1 1}
test "Tcldlist-dlist_ones" "err1" {
     dlist_ones 4.5
} {expected integer but got "4\.5"}
test "Tcldlist-dlist_ones" "err2" { dlist_ones -1; } {^$}

### dlist_pickoneCmd ###
test "Tcldlist-dlist_pickone" "args" {
    dlist_pickone
} {wrong \# args: should be "dlist_pickone source_list"}
test "Tcldlist-dlist_pickone" "norm1" { 
	 dlist_pickone {1 3 5 7 9}
} {[13579]}
test "Tcldlist-dlist_pickone" "norm2" {
     dlist_pickone {0 3.4 -2.6 str {list list}}
} {0|3\.4|-2\.6|str|\{list list\}}
test "Tcldlist-dlist_pickone" "error" {
     dlist_pickone {}
} {dlist_pickone: source_list is empty}

### dlist_rangeCmd ###
test "Tcldlist-dlist_range" "args" {
    dlist_range
} {wrong \# args: should be "dlist_range begin end"}
test "Tcldlist-dlist_range" "norm1" { 
	 dlist_range 0 0
} {^0$}
test "Tcldlist-dlist_range" "norm2" { 
	 dlist_range 0 5
} {0 1 2 3 4 5}
test "Tcldlist-dlist_range" "norm3" { dlist_range 0 -1} {^$}
test "Tcldlist-dlist_range" "err1" {
    dlist_range 0.5 6
} {expected integer but got "0\.5"}

### dlist_repeatCmd ###
test "Tcldlist-dlist_repeat" "args" {
    dlist_repeat
} {wrong \# args: should be "dlist_repeat source_list times_list"}
test "Tcldlist-dlist_repeat" "norm1" {
	 catch {dlist_repeat {} {}}
} {^0$}
test "Tcldlist-dlist_repeat" "norm2" { 
	 catch {dlist_repeat { 4 5 6 } { 0 0 0 }}
} {^0$}
test "Tcldlist-dlist_repeat" "norm3" { 
	 dlist_repeat { 4 5 6 } { 1 2 3 }
} {4 5 5 6 6 6}
test "Tcldlist-dlist_repeat" "err4" { 
	 dlist_repeat { 4 5 6 } { -1 2 3 }
} {5 5 6 6 6}
test "Tcldlist-dlist_repeat" "err2" {
     dlist_repeat { 4 5 6 } { 1.5 2 3 }
} {expected integer but got "1\.5"}

### dlist_selectCmd ###
test "Tcldlist-dlist_select" "args" {
    dlist_select
} {wrong \# args: should be "dlist_select source_list flags_list"}
test "Tcldlist-dlist_select" "norm1" { 
	 dlist_select {1 2 3 4 5} {1 0 0 1 1}
} {1 4 5}
test "Tcldlist-dlist_select" "norm2" { 
	 dlist_select {1 2.5 str {L L}} {0 1 1 1}
} {2\.5 str \{L L\}}
test "Tcldlist-dlist_select" "error" {
     dlist_select {1 2 3} {1.5 0.2 0.8}
} {expected integer but got "1\.5"}

### dlist_sumCmd ###
test "Tcldlist-dlist_sum" "args" {
    dlist_sum
} {wrong \# args: should be "dlist_sum source_list"}
test "Tcldlist-dlist_sum" "norm int" { dlist_sum { -3 6 0 9 17}; } 29
test "Tcldlist-dlist_sum" "norm float" {
     dlist_sum { 1.6 0.0 -0.2 12.9 }
} {14\.3}
test "Tcldlist-dlist_sum" "error" {
     dlist_sum { 1 junk }
} {expected floating-point number but got "junk"}

### dlist_zerosCmd ###
test "Tcldlist-dlist_zeros" "args" {
	 dlist_zeros
} {wrong \# args: should be "dlist_zeros num_zeros"}
test "Tcldlist-dlist_zeros" "norm1" {
	 catch {dlist_zeros 0}
} {^0$}
test "Tcldlist-dlist_zeros" "norm2" {
	 dlist_zeros 5
} {0 0 0 0 0}
test "Tcldlist-dlist_zeros" "norm3" {
	 catch {dlist_zeros -1}
} {^0$}
test "Tcldlist-dlist_zeros" "err1" { 
	 dlist_zeros 3.5
} {expected integer but got "3\.5"}


##############################################################################
###
### TclGL
###
##############################################################################

### glBeginCmd ###
test "TclGL-glBegin" "args" {glBegin} {wrong \# args: should be "glBegin mode"}
test "TclGL-glBegin" "normal use" {} {^$}
test "TclGL-glBegin" "error" {} {^$}

### glEndCmd ###
test "TclGL-glEnd" "args" {glEnd junk} {wrong \# args: should be "glEnd"}
test "TclGL-glEnd" "normal use" {} {^$}
test "TclGL-glEnd" "error" {} {^$}

### glVertex3fCmd ###
test "TclGL-glVertex3f" "args" {
	 glVertex3f
} {wrong \# args: should be "glVertex3f x y z"}
test "TclGL-glVertex3f" "normal use" {} {^$}
test "TclGL-glVertex3f" "error" {} {^$}

### glIndexiCmd ###
test "TclGL-glIndexi" "args" {
	 glIndexi
} {wrong \# args: should be "glIndexi index"}
test "TclGL-glIndexi" "normal use" {} {^$}
test "TclGL-glIndexi" "error" {} {^$}

### glLineWidthCmd ###
test "TclGL-glLineWidth" "args" {
	 glLineWidth
} {wrong \# args: should be "glLineWidth width"}
test "TclGL-glLineWidth" "normal use" {} {^$}
test "TclGL-glLineWidth" "error" {} {^$}

### glFlushCmd ###
test "TclGL-glFlush" "args" {glFlush junk} {wrong \# args: should be "glFlush"}
test "TclGL-glFlush" "normal use" {} {^$}
test "TclGL-glFlush" "error" {} {^$}

### setBackgroundCmd ###
test "TclGL-setBackground" "args" {
	 setBackground
} {wrong \# args: should be "setBackground { \[index\] or \[R G B\] }"}
test "TclGL-setBackground" "normal use" {} {^$}
test "TclGL-setBackground" "error" {} {^$}

### setForegroundCmd ###
test "TclGL-setForeground" "args" {
	 setForeground
} {wrong \# args: should be "setForeground { \[index\] or \[R G B\] }"}
test "TclGL-setForeground" "normal use" {} {^$}
test "TclGL-setForeground" "error" {} {^$}

### drawOneLineCmd ###
test "TclGL-drawOneLine" "args" {
	 drawOneLine
} {wrong \# args: should be "drawOneLine x1 y1 x2 y2"}
test "TclGL-drawOneLine" "normal use" {} {^$}
test "TclGL-drawOneLine" "error" {} {^$}

### drawThickLineCmd ###
test "TclGL-drawThickLine" "args" {
	 drawThickLine
} {wrong \# args: should be "drawThickLine x1 y1 x2 y2 thickness"}
test "TclGL-drawThickLine" "normal use" {} {^$}
test "TclGL-drawThickLine" "error" {} {^$}

### antialiasCmd ###
test "TclGL-antialias" "args" {
	 antialias
} {wrong \# args: should be "antialias on_off"}
test "TclGL-antialias" "normal use" {} {^$}
test "TclGL-antialias" "error" {} {^$}

### lineInfoCmd ###
test "TclGL-lineInfo" "args" {
	 lineInfo junk
} {wrong \# args: should be "lineInfo"}
test "TclGL-lineInfo" "normal use" {} {^$}
test "TclGL-lineInfo" "error" {} {^$}


##############################################################################
###
### SoundTcl
###
##############################################################################

# Check if audio is connected; if not, we skip the sound tests.
#set AUDIO [Sound::haveAudio]
set AUDIO 0							  ;# disable sound tests

### Sound::setCmd ###
test "SoundTcl-Sound::set" "too few args" {
	 Sound::set
} {wrong \# args: should be "Sound::set sound_name \?new_filename\?"} $AUDIO
test "SoundTcl-Sound::set" "too many args" {
	 Sound::set j u n
} {wrong \# args: should be "Sound::set sound_name \?new_filename\?"} $AUDIO
test "SoundTcl-Sound::set" "normal filename set" {
	 catch {Sound::set test $::TEST_DIR/sound1.au}
} {^0$} $AUDIO
test "SoundTcl-Sound::set" "normal filename query" {
	 Sound::set test $::TEST_DIR/sound1.au
	 set f [Sound::set test]
	 expr [string compare $f "$::TEST_DIR/sound1.au"] == 0
} {^1$} $AUDIO
test "SoundTcl-Sound::set" "error on bad filename" {
	 exec rm -rf $::TEST_DIR/nonexistent_file
	 Sound::set test $::TEST_DIR/nonexistent_file
} "Sound::set: SoundError: bad or nonexistent file\
		  '$::TEST_DIR/nonexistent_file'" $AUDIO

### Sound::playCmd ###
test "SoundTcl-Sound::play" "too few args" {
	 Sound::play
} {wrong \# args: should be "Sound::play sound_name"} $AUDIO
test "SoundTcl-Sound::play" "too many args" {
	 Sound::play j u
} {wrong \# args: should be "Sound::play sound_name"} $AUDIO
test "SoundTcl-Sound::play" "normal play test sound" {
	 Sound::set test $::TEST_DIR/sound1.au
	 catch {Sound::play test}
} {^0$} $AUDIO
test "SoundTcl-Sound::play" "normal play ok sound" {
	 Sound::set ok $::TEST_DIR/sound1.au
	 catch {Sound::play ok}
} {^0$} $AUDIO
test "SoundTcl-Sound::play" "normal play err sound" {
	 Sound::set err $::TEST_DIR/sound2.au
	 catch {Sound::play err}
} {^0$} $AUDIO
test "SoundTcl-Sound::play" "error on bad sound name" {
	 Sound::play junk
} {Sound::play: sound does not exist} $AUDIO

unset AUDIO


##############################################################################
###
### TlistTcl
###
##############################################################################

### undrawCmd ###
test "TlistTcl-undraw" "too many args" {
	 undraw junk
} {wrong \# args: should be "undraw"}
test "TlistTcl-undraw" "normal use" {
	 Tlist::clearAllTrials
	 set f [face]
	 set p [Pos::position]
	 Tlist::addObject 0 $f $p
	 setForeground 1
	 setBackground 0
	 show 0
	 undraw
	 pixelCheckSum 0 0 $::TOGL_WIDTH $::TOGL_HEIGHT
} {^0$}
test "TlistTcl-undraw" "no error" {} $BLANK $no_test

### redrawCmd ###
test "TlistTcl-redraw" "too many args" {
	 redraw junk
} {wrong \# args: should be "redraw"}
test "TlistTcl-redraw" "normal use" {
	 Tlist::clearAllTrials
	 set f [face]
	 set p [Pos::position]
	 Tlist::addObject 0 $f $p
	 Tlist::setCurTrial 0
	 setForeground 1
	 setBackground 0
	 redraw
	 # check to see if some pixels actually got drawn
	 expr [pixelCheckSum 0 0 $::TOGL_WIDTH $::TOGL_HEIGHT] > 500
} {^1$}
test "TlistTcl-redraw" "no error" {} $BLANK $no_test

### clearscreenCmd ###
test "TlistTcl-clearscreen" "too many args" {
    clearscreen junk
} {wrong \# args: should be "clearscreen"}
test "TlistTcl-clearscreen" "normal use" {
	 setBackground 0
	 clearscreen
	 pixelCheckSum 0 0 $::TOGL_WIDTH $::TOGL_HEIGHT
} {^0$}
test "TlistTcl-clearscreen" "no error" {} $BLANK $no_test

### showCmd ###
test "TlistTcl-show" "too few args" {
	 show
} {wrong \# args: should be "show trial_id"}
test "TlistTcl-show" "too many args" {
	 show j u
} {wrong \# args: should be "show trial_id"}
test "TlistTcl-show" "normal use on valid trial id" {
	 catch {show 10}
} {^0$}
test "TlistTcl-show" "normal use on invalid trial id" { 
	 catch {show -1}
} {^0$}
test "TlistTcl-show" "no error" {} $BLANK $no_test

### Tlist::numTrialsCmd ###
test "TlistTcl-Tlist::numTrials" "too many args" {
	 Tlist::numTrials junk
} {wrong \# args: should be "Tlist::numTrials"}
test "TlistTcl-Tlist::numTrials" "normal use on empty Tlist" {
	 Tlist::clearAllTrials
	 Tlist::numTrials
} {^0$}
test "TlistTcl-Tlist::numTrials" "normal use on non-empty Tlist" {
	 Tlist::clearAllTrials
	 set f [face]
	 set p [Pos::position]
	 Tlist::addObject 0 $f $p
	 Tlist::addObject 3 $f $p
	 Tlist::numTrials
} {^2$}
test "TlistTcl-Tlist::numTrials" "no error" {} $BLANK $no_test

### Tlist::setVisibleCmd ###
test "TlistTcl-Tlist::setVisible" "too few args" {
    Tlist::setVisible
} {wrong \# args: should be "Tlist::setVisible visibility"}
test "TlistTcl-Tlist::setVisible" "too many args" {
    Tlist::setVisible j u
} {wrong \# args: should be "Tlist::setVisible visibility"}
test "TlistTcl-Tlist::setVisible" "normal set off" {
	 catch {Tlist::setVisible 0}
} {^0$}
test "TlistTcl-Tlist::setVisible" "normal set on" {
	 catch {Tlist::setVisible 1}
} {^0$}
test "TlistTcl-Tlist::setVisible" "unusual set on" {
	 catch {Tlist::setVisible -1}
} {^0$}
test "TlistTcl-Tlist::setVisible" "error on non-numeric input" {
    Tlist::setVisible junk
} {expected boolean value but got "junk"}

### Tlist::addObjectCmd ###
test "TlistTcl-Tlist::addObject" "too few args" {
    Tlist::addObject
} {wrong \# args: should be "Tlist::addObject trial objid posid"}
test "TlistTcl-Tlist::addObject" "too many args" {
    Tlist::addObject j u n k
} {wrong \# args: should be "Tlist::addObject trial objid posid"}
test "TlistTcl-Tlist::addObject" "normal use" {
	 set f [face]
	 set p [Pos::position]
	 catch {Tlist::addObject 5 $f $p}
} {^0$}
test "TlistTcl-Tlist::addObject" "error on bad objid" {
	 set p [Pos::position]
    Tlist::addObject 5 -1 $p
} {Tlist::addObject: objid out of range}
test "TlistTcl-Tlist::addObject" "error on bad posid" {
	 set f [face]
    Tlist::addObject 5 $f -1
} {Tlist::addObject: posid out of range}
test "TlistTcl-Tlist::addObject" "error on bad trial id" {
	 set f [face]
	 set p [Pos::position]
    Tlist::addObject -1 $f $p
} {Tlist::addObject: invalid trial id}

### Tlist::setCurTrialCmd ###
test "TlistTcl-Tlist::setCurTrial" "too few args" {
    Tlist::setCurTrial
} {wrong \# args: should be "Tlist::setCurTrial trial_id"}
test "TlistTcl-Tlist::setCurTrial" "too many args" {
    Tlist::setCurTrial j u
} {wrong \# args: should be "Tlist::setCurTrial trial_id"}
test "TlistTcl-Tlist::setCurTrial" "normal use" {
	 set f [face]
	 set p [Pos::position]
	 Tlist::addObject 0 0 0
	 catch {Tlist::setCurTrial 0}
} {^0$}
test "TlistTcl-Tlist::setCurTrial" "error on too low trial id" {
    Tlist::setCurTrial -1
} {Tlist::setCurTrial: invalid trial id}
test "TlistTcl-Tlist::setCurTrial" "error on too large trial id" {
	 Tlist::clearAllTrials
    Tlist::setCurTrial 10000
} {Tlist::setCurTrial: invalid trial id}

### Tlist::clearAllTrialsCmd ###
test "TlistTcl-Tlist::clearAllTrials" "too many args" {
	 Tlist::clearAllTrials junk
} {wrong \# args: should be "Tlist::clearAllTrials"}
test "TlistTcl-Tlist::clearAllTrials" "normal use" {
	 resetObjList
	 resetPosList
	 set f [face]
	 set p [Pos::position]
	 Tlist::addObject 0 $f $p
	 Tlist::addObject 1 $f $p
	 Tlist::clearAllTrials
	 Tlist::stringify
} "Tlist $OBJLIST $INT $INT
0 Face .*
1
$POSLIST $INT $INT
0 Position .*
1
$INT 0
0 $BIT"
test "TlistTcl-Tlist::clearAllTrials" "no error" {} $BLANK $no_test

### Tlist::loadObjidFileCmd ###
test "TlistTcl-Tlist::loadObjidFile" "too few args" {
    Tlist::loadObjidFile
} {wrong \# args: should be "Tlist::loadObjidFile\
		  objid_file num_lines \?offset\?"}
test "TlistTcl-Tlist::loadObjidFile" "too many args" {
    Tlist::loadObjidFile j u n k
} {wrong \# args: should be "Tlist::loadObjidFile\
		  objid_file num_lines \?offset\?"}
test "TlistTcl-Tlist::loadObjidFile" "normal read with no offset" {
	 resetObjList
	 resetPosList
	 Tlist::loadObjidFile $::TEST_DIR/objid_file -1 0
	 Tlist::stringify
} "Tlist $OBJLIST $INT 0
0
$POSLIST $INT 0
0
3 3
0 Trial 3 1 0  1 1  2 2  0  -1.*
1 Trial 2 3 0  4 1  0  -1.*
2 Trial 4 6 0  3 1  9 2  11 3  0  -1.*
0 $BIT"
test "TlistTcl-Tlist::loadObjidFile" "read with fixed # lines, and offset" {
	 resetObjList
	 resetPosList
	 Tlist::loadObjidFile $::TEST_DIR/objid_file 2 1
	 Tlist::stringify
} "Tlist $OBJLIST $INT 0
0
$POSLIST $INT 0
0
2 2
0 Trial 3 2 0  2 1  3 2  0  -1.*
1 Trial 2 4 0  5 1  0  -1.*
0 \[01\]"
test "TlistTcl-Tlist::loadObjidFile" "read empty file" {
	 resetObjList
	 resetPosList
	 Tlist::loadObjidFile $::TEST_DIR/empty_file -1 0
	 Tlist::stringify
} "Tlist $OBJLIST $INT 0
0
$POSLIST $INT 0
0
0 0
0 $BIT"
test "TlistTcl-Tlist::loadObjidFile" "error on junk text file" {
	 Tlist::loadObjidFile $::TEST_DIR/junk_text_file -1 0
} {Tlist::loadObjidFile: InputError: Trial}
test "TlistTcl-Tlist::loadObjidFile" "error on junk binary file" {
	 Tlist::loadObjidFile $::TEST_DIR/junk_bin_file -1 0
} {Tlist::loadObjidFile: InputError: Trial}

### Tlist::makeSummaryTrialCmd ###
test "TlistTcl-Tlist::makeSummaryTrial" "too few args" {
	 Tlist::makeSummaryTrial
} {^wrong \# args: should be "Tlist::makeSummaryTrial trialid num_cols scale \?xstep\? \?ystep\?"$}
test "TlistTcl-Tlist::makeSummaryTrial" "too many args" {
	 Tlist::makeSummaryTrial 2 3 4 5 6 7
} {^wrong \# args: should be "Tlist::makeSummaryTrial trialid num_cols scale \?xstep\? \?ystep\?"$}
test "TlistTcl-Tlist::makeSummaryTrial" "normal use" {
	 resetObjList
	 resetPosList
	 Face::loadFaces $::TEST_DIR/faces_file
	 Tlist::makeSummaryTrial 0 5 1.0 2.0 3.0
} {^0$}
test "TlistTcl-Tlist::makeSummaryTrial" "error on bad trialid" {
	 resetObjList
	 resetPosList
	 Face::loadFaces $::TEST_DIR/faces_file
	 Tlist::makeSummaryTrial -1 5 1.0 2.0 3.0
} {^Tlist::makeSummaryTrial: invalid trial id$}
test "TlistTcl-Tlist::makeSummaryTrial" "error on bad num_cols" {
	 resetObjList
	 resetPosList
	 Face::loadFaces $::TEST_DIR/faces_file
	 Tlist::makeSummaryTrial 0 -1 1.0 2.0 3.0
} {^Tlist::makeSummaryTrial: num_cols must be a positive integer$}

### Tlist::makeSinglesCmd ###
test "TlistTcl-Tlist::makeSingles" "too few args" {
    Tlist::makeSingles
} {wrong \# args: should be "Tlist::makeSingles posid"}
test "TlistTcl-Tlist::makeSingles" "too many args" {
    Tlist::makeSingles j u
} {wrong \# args: should be "Tlist::makeSingles posid"}
test "TlistTcl-Tlist::makeSingles" "normal use with several GrObj's" {
	 resetObjList
	 resetPosList
	 face; face; face;
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Tlist::stringify
} "Tlist $OBJLIST $INT 3
0 Face .*
1 Face .*
2 Face .*
3
$POSLIST $INT 1
0 Position .*
1
$INT 3
0 Trial 1 0 0  0  0.*
1 Trial 1 1 0  0  0.*
2 Trial 1 2 0  0  0.*
0 $BIT"
test "TlistTcl-Tlist::makeSingles" "normal use with empty ObjList" {
	 resetObjList
	 resetPosList
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Tlist::stringify
} "Tlist $OBJLIST $INT 0
0
$POSLIST $INT 1
0 Position .*
1
$INT 0
0 $BIT"
test "TlistTcl-Tlist::makeSingles" "error on bad posid" {
	 resetObjList
	 resetPosList
	 Tlist::makeSingles 0
} {Tlist::makeSingles: posid out of range}

### Tlist::makePairsCmd ###
test "TlistTcl-Tlist::makePairs" "too few args" {
    Tlist::makePairs
} {wrong \# args: should be "Tlist::makePairs posid1 posid2"}
test "TlistTcl-Tlist::makePairs" "too many args" {
    Tlist::makePairs j u n
} {wrong \# args: should be "Tlist::makePairs posid1 posid2"}
test "TlistTcl-Tlist::makePairs" "normal use on two GrObj's" {
	 resetObjList
	 resetPosList
	 face; face;
	 set p1 [Pos::position]
	 set p2 [Pos::position]
	 Tlist::makePairs $p1 $p2
	 Tlist::stringify
} "Tlist $OBJLIST $INT 2
0 Face .*
1 Face .*
2
$POSLIST $INT 2
0 Position .*
1 Position .*
2
$INT 4
0 Trial 2 0 0  0 1  0  1.*
1 Trial 2 0 0  1 1  0  0.*
2 Trial 2 1 0  0 1  0  0.*
3 Trial 2 1 0  1 1  0  1.*
0 $BIT"
test "TlistTcl-Tlist::makePairs" "normal use with empty ObjList" {
	 resetObjList
	 resetPosList
	 set p1 [Pos::position]
	 set p2 [Pos::position]
	 Tlist::makePairs $p1 $p2
	 Tlist::stringify
} "Tlist $OBJLIST $INT 0
0
$POSLIST $INT 2
0 Position .*
1 Position .*
2
$INT 0
0 $BIT"
test "TlistTcl-Tlist::makePairs" "error on bad posid" {
	 resetObjList
	 resetPosList
	 Tlist::makePairs 0 1
} {Tlist::makePairs: posid out of range}

### Tlist::makeTriadsCmd ###
test "TlistTcl-Tlist::makeTriads" "too few args" {
    Tlist::makeTriads
} {wrong \# args: should be "Tlist::makeTriads posid1 posid2 posid3"}
test "TlistTcl-Tlist::makeTriads" "too many args" {
    Tlist::makeTriads j u n k
} {wrong \# args: should be "Tlist::makeTriads posid1 posid2 posid3"}
test "TlistTcl-Tlist::makeTriads" "normal use on three GrObj's" {
	 resetObjList
	 resetPosList
	 face; face; face;
	 set p1 [Pos::position]
	 set p2 [Pos::position]
	 set p3 [Pos::position]
	 Tlist::makeTriads $p1 $p2 $p3
	 Tlist::numTrials
} {^18$}
test "TlistTcl-Tlist::makeTriads" "normal use on two GrObj's" {
	 resetObjList
	 resetPosList
	 face; face;
	 set p1 [Pos::position]
	 set p2 [Pos::position]
	 set p3 [Pos::position]
	 Tlist::makeTriads $p1 $p2 $p3
	 Tlist::numTrials
} {^0$}
test "TlistTcl-Tlist::makeTriads" "normal use on empty ObjList" {
	 resetObjList
	 resetPosList
	 set p1 [Pos::position]
	 Tlist::makeTriads $p1 $p1 $p1
	 Tlist::numTrials
} {^0$}
test "TlistTcl-Tlist::makeTriads" "check that Tlist is cleared first" {
	 # Put some random stuff in the Tlist first
	 resetObjList
	 resetPosList
	 set f [face]
	 set p [Pos::position]
	 Tlist::addObject 0 $f $p
	 Tlist::addObject 3 $f $p
	 # Now, this should clear the Tlist before it does anything else, and since
	 # there aren't enough GrObj's for triads, it should do nothing else.
	 Tlist::makeTriads $p $p $p
	 Tlist::numTrials
} {^0$}
test "TlistTcl-Tlist::makeTriads" "error on bad posid" {
	 resetObjList
	 resetPosList
	 Tlist::makeTriads 0 1 2
} {Tlist::makeTriads: posid out of range}

### Tlist::write_responsesCmd ###
test "TlistTcl-Tlist::write_responses" "too few args" {
    Tlist::write_responses
} {wrong \# args: should be "Tlist::write_responses filename"}
test "TlistTcl-Tlist::write_responses" "too many args" {
    Tlist::write_responses j u
} {wrong \# args: should be "Tlist::write_responses filename"}
test "TlistTcl-Tlist::write_responses" "normal use" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 set temp_file $::TEST_DIR/temp[pid]_$::DATE
	 Tlist::write_responses $temp_file
	 set val [catch {exec diff $::TEST_DIR/response_file $temp_file} res]
	 exec rm $temp_file
	 expr $val == 0 && [string compare $res ""] == 0
} {^1$} $test_serialize

### Tlist::stringifyCmd ###
test "TlistTcl-Tlist::stringify" "too many args" {
    Tlist::stringify junk
} {wrong \# args: should be "Tlist::stringify"}
test "TlistTcl-Tlist::stringify" "normal use with everything empty" {
	 resetObjList
	 resetPosList
	 Tlist::clearAllTrials
	 Tlist::stringify
} "Tlist $OBJLIST $INT 0
0
$POSLIST $INT 0
0
$INT 0
0 $BIT"
test "TlistTcl-Tlist::stringify" "no error" {} $BLANK $no_test

### Tlist::destringifyCmd ###
test "TlistTcl-Tlist::destringify" "too few args" {
    Tlist::destringify
} {wrong \# args: should be "Tlist::destringify string"}
test "TlistTcl-Tlist::destringify" "too many args" {
    Tlist::destringify j u
} {wrong \# args: should be "Tlist::destringify string"}
test "TlistTcl-Tlist::destringify" "write, read, write and compare" {
	 resetObjList
	 resetPosList
	 face; face;
	 set p1 [Pos::position]
	 set p2 [Pos::position]
	 Tlist::makePairs $p1 $p2
	 set str [Tlist::stringify]
	 resetObjList
	 resetPosList
	 Tlist::clearAllTrials
	 Tlist::destringify $str
	 set str2 [Tlist::stringify]
	 expr [objCount] == 2 && [posCount] == 2 \
				&& [string compare $str $str2] == 0
} {^1$}
test "TlistTcl-Tlist::destringify" "error on junk input" {
	 Tlist::destringify "what a bunch of hogwash this is"
} {Tlist::destringify: InputError: Tlist}

##############################################################################
###
### ExptTcl
###
##############################################################################

# need to add checks on appropriate returns from all functions that use
# isComplete()
# i.e. try to begin a trial after the expt is done, try to abort a trial on 
# an expt that hasn't started, etc.
# make sure to get trialDescription on empty + completed expt's
# try all different possibilities on init

### Expt::abortTrialCmd ###
test "ExptTcl-Expt::abortTrial" "too many args" {
    Expt::abortTrial junk
} {wrong \# args: should be "Expt::abortTrial"} $Expt::haveTest

if {$test_serialize} {
test "ExptTcl-Expt::abortTrial" "normal use on incomplete expt" {
	 # Check that the value of the previous response is the same 
	 # before and after we abort a trial.
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Expt::beginTrial
	 Expt::recordResponse 5
	 set val [Expt::prevResponse]
	 Expt::abortTrial
	 Expt::beginTrial
	 expr $val == 5 && [Expt::prevResponse] == $val
} {^1$} $Expt::haveTest
test "ExptTcl-Expt::abortTrial" "normal use on completed expt" {
	 # abortTrial on completed Expt is a no-op
	 Expt::read $::TEST_DIR/completed_expt_file
	 catch {Expt::abortTrial}
} {^0$} $Expt::haveTest
test "ExptTcl-Expt::abortTrial" "normal use on empty expt" {
	 # abortTrial on empty Expt is a no-op
	 resetObjList
	 resetPosList
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Expt::init 1 0
	 catch {Expt::abortTrial}
} {^0$} $Expt::haveTest
test "ExptTcl-Expt::abortTrial" "error" {} $BLANK $no_test
}

### Expt::beginCmd ###
test "ExptTcl-Expt::begin" "too many args" {
	 Expt::begin junk
} {^wrong \# args: should be "Expt::begin"$}
test "ExptTcl-Expt::begin" "normal use" {
	 catch {
		  Expt::begin
		  Expt::stop
	 }
} {^0$}
test "ExptTcl-Expt::begin" "error" {} $BLANK $no_test

### Expt::beginTrialCmd ###
test "ExptTcl-Expt::beginTrial" "too many args" {
    Expt::beginTrial junk
} {wrong \# args: should be "Expt::beginTrial"} $Expt::haveTest

if {$test_serialize} {
test "ExptTcl-Expt::beginTrial" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 catch {Expt::beginTrial}
} {^0$} $Expt::haveTest
test "ExptTcl-Expt::beginTrial" "normal use on completed expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 # This should be a no-op
	 catch {Expt::beginTrial}
} {^0$} $Expt::haveTest
test "ExptTcl-Expt::beginTrial" "normal use on empty expt" {
	 resetObjList
	 resetPosList
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Expt::init 1 0
	 # This should be a no-op
	 catch {Expt::beginTrial}
} {^0$} $Expt::haveTest
}

test "ExptTcl-Expt::beginTrial" "error" {} $BLANK $no_test

### Expt::currentTrialTypeCmd ###
test "ExptTcl-Expt::currentTrialType" "args" {
    Expt::currentTrialType junk
} {wrong \# args: should be "Expt::currentTrialType"}

if {$test_serialize} {
test "ExptTcl-Expt::currentTrialType" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Expt::currentTrialType
} {^1$}
test "ExptTcl-Expt::currentTrialType" "normal use on completed expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 # This should be a no-op
	 Expt::currentTrialType
} {^-1$}
}

test "ExptTcl-Expt::currentTrialType" "normal use on empty expt" {
	 resetObjList
	 resetPosList
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Expt::init 1 0
	 # This should be a no-op
	 Expt::currentTrialType
} {^-1$}
test "ExptTcl-Expt::currentTrialType" "error" {} $BLANK $no_test

### currentTrialCmd ###
test "ExptTcl-Expt::currentTrial" "too many args" {
	 Expt::currentTrial junk
} {wrong \# args: should be "Expt::currentTrial"}

if {$test_serialize} {
test "ExptTcl-Expt::currentTrial" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Expt::currentTrial
} {^5$}
test "ExptTcl-Expt::currentTrial" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Expt::currentTrial
} {\-1}
}

test "ExptTcl-Expt::currentTrial" "normal use on empty expt" {
	 # Make an empty expt by clearing ObjList, then Tlist::makeSingles
	 resetObjList
	 resetPosList
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Expt::init 1 0
	 Expt::currentTrial
} {^\-1$}
test "ExptTcl-Expt::currentTrial" "no error" {} $BLANK $no_test

### Expt::initCmd ###
test "ExptTcl-Expt::init" "too few args" {
    Expt::init
} {wrong \# args: should be "Expt::init repeat rand_seed"}
test "ExptTcl-Expt::init" "too many args" {
    Expt::init j u n
} {wrong \# args: should be "Expt::init repeat rand_seed"}
test "ExptTcl-Expt::init" "normal use" {} {^$}
test "ExptTcl-Expt::init" "error" {} {^$}

### Expt::isCompleteCmd ###
test "ExptTcl-Expt::isComplete" "args" {
    Expt::isComplete junk
} {wrong \# args: should be "Expt::isComplete"}

if {$test_serialize} {
test "ExptTcl-Expt::isComplete" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Expt::isComplete
} {^0$}
test "ExptTcl-Expt::isComplete" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Expt::isComplete
} {^1$}
}

test "ExptTcl-Expt::isComplete" "normal use on empty expt" {
	 # An empty expt is complete (all of its 0 trials are finished)
	 resetObjList
	 resetPosList
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Expt::init 1 0
	 Expt::isComplete
} {^1$}
test "ExptTcl-Expt::isComplete" "error" {} $BLANK $no_test

### numCompletedCmd ###
test "ExptTcl-Expt::numCompleted" "too many args" {
	 Expt::numCompleted junk
} {wrong \# args: should be "Expt::numCompleted"}

if {$test_serialize} {
test "ExptTcl-Expt::numCompleted" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Expt::numCompleted
} {^4$}
test "ExptTcl-Expt::numCompleted" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Expt::numCompleted
} {^100$}
}

test "ExptTcl-Expt::numCompleted" "use on empty expt" {
	 # Make an empty expt by clearing ObjList, then Tlist::makeSingles
	 resetObjList
	 resetPosList
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Expt::init 1 0
	 Expt::numCompleted
} {^0$}
test "ExptTcl-Expt::numCompleted" "no error" {} $BLANK $no_test

### numTrialsCmd ###
test "ExptTcl-Expt::numTrials" "too many args" {
	 Expt::numTrials junk
} {wrong \# args: should be "Expt::numTrials"}

if {$test_serialize} {
test "ExptTcl-Expt::numTrials" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Expt::numTrials	 
} {^10$}
test "ExptTcl-Expt::numTrials" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Expt::numTrials	 
} {^100$}
}

test "ExptTcl-Expt::numTrials" "use on empty expt" {
	 # Make an empty expt by clearing ObjList, then Tlist::makeSingles
	 resetObjList
	 resetPosList
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Expt::init 1 0
	 Expt::numTrials	 
} {^0$}
test "ExptTcl-Expt::numTrials" "no error" {} $BLANK $no_test

### Expt::pauseCmd ###
test "ExptTcl-Expt::pause" "too many args" {
	 Expt::pause junk
} {^wrong \# args: should be "Expt::pause"$}
test "ExptTcl-Expt::pause" "normal use" {} {^$}
test "ExptTcl-Expt::pause" "error" {} $BLANK $no_test

### prevResponseCmd ###
test "ExptTcl-Expt::prevResponse" "too many args" {
	 Expt::prevResponse junk
} {wrong \# args: should be "Expt::prevResponse"}

if {$test_serialize} {
test "ExptTcl-Expt::prevResponse" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Expt::prevResponse
} {^0$}
test "ExptTcl-Expt::prevResponse" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Expt::prevResponse
} {^1$}
}

test "ExptTcl-Expt::prevResponse" "normal use on empty expt" {
	 # Make an empty expt by clearing ObjList, then Tlist::makeSingles
	 resetObjList
	 resetPosList
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Expt::init 1 0
	 Expt::prevResponse
} {^\-1$}
test "ExptTcl-Expt::prevResponse" "error" {} $BLANK $no_test

### Expt::readCmd ###
test "ExptTcl-Expt::read" "too few args" {
    Expt::read
} {wrong \# args: should be "Expt::read filename"}
test "ExptTcl-Expt::read" "too many args" {
    Expt::read j u
} {wrong \# args: should be "Expt::read filename"}

if {$test_serialize} {
test "ExptTcl-Expt::read" "normal read of completed expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 expr [Expt::isComplete] == 1 && \
				[objCount] == 10 && \
				[posCount] == 1 && \
				[Expt::numTrials] == 100 && \
				[Expt::numCompleted] == 100 && \
				[Expt::prevResponse] == 1
} {^1$}
test "ExptTcl-Expt::read" "error on junk text file" {
	 Expt::read $::TEST_DIR/junk_text_file
} {Expt::read: InputError: Expt}
test "ExptTcl-Expt::read" "error on junk binary file" {
	 Expt::read $::TEST_DIR/junk_bin_file
} {Expt::read: InputError: Expt}
test "ExptTcl-Expt::read" "error on non-existent file" {
	 exec rm -rf $::TEST_DIR/nonexistent_file
	 Expt::read $::TEST_DIR/nonexistent_file
} "Expt::read: IoFilenameError: $::TEST_DIR/nonexistent_file"
}

### Expt::recordResponseCmd ###
test "ExptTcl-Expt::recordResponse" "too few args" {
    Expt::recordResponse
} {wrong \# args: should be "Expt::recordResponse response"} $Expt::haveTest
test "ExptTcl-Expt::recordResponse" "too many args" {
    Expt::recordResponse j u
} {wrong \# args: should be "Expt::recordResponse response"} $Expt::haveTest

if {$test_serialize} {
test "ExptTcl-Expt::recordResponse" "normal record response" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Expt::recordResponse 5
	 Expt::prevResponse
} {^5$} $Expt::haveTest
test "ExptTcl-Expt::recordResponse" "record response on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 catch {Expt::recordResponse 5}
} {^0$} $Expt::haveTest
}

test "ExptTcl-Expt::recordResponse" "record response on empty expt" {
	 resetObjList
	 resetPosList
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Expt::init 1 0
	 catch {Expt::recordResponse 5}
} {^0$} $Expt::haveTest
test "ExptTcl-Expt::recordResponse" "error" {} $BLANK $no_test

### Expt::stopCmd ###
test "ExptTcl-Expt::stop" "too many args" {
	 Expt::stop junk
} {^wrong \# args: should be "Expt::stop"$}
test "ExptTcl-Expt::stop" "normal use" {} {^$}
test "ExptTcl-Expt::stop" "error" {} $BLANK $no_test

### Expt::trialDescriptionCmd ###
test "ExptTcl-Expt::trialDescription" "too many args" {
    Expt::trialDescription junk
} {wrong \# args: should be "Expt::trialDescription"}

if {$test_serialize} {
test "ExptTcl-Expt::trialDescription" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Expt::trialDescription
} {trial id == 5, trial type == 1, objs == 5, categories == 1,\
		  completed 4 of 10}
test "ExptTcl-Expt::trialDescription" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Expt::trialDescription
} {^expt is complete$}
}

test "ExptTcl-Expt::trialDescription" "normal use on empty expt" {
	 resetObjList
	 resetPosList
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Expt::init 1 0
	 Expt::trialDescription
} {^expt is complete$}
test "ExptTcl-Expt::trialDescription" "error" {} $BLANK $no_test

### Expt::undrawTrialCmd ###
test "ExptTcl-Expt::undrawTrial" "too many args" {
	 Expt::undrawTrial junk
} {^wrong \# args: should be "Expt::undrawTrial"$} $Expt::haveTest
test "ExptTcl-Expt::undrawTrial" "normal use" {} {^$} $Expt::haveTest
test "ExptTcl-Expt::undrawTrial" "error" {} $BLANK $no_test

### Expt::useFeedbackCmd ###
test "ExptTcl-Expt::useFeedback" "too many args" {
	 Expt::useFeedback junk junk
} {^wrong \# args: should be "Expt::useFeedback \?new_value\?"$}
test "ExptTcl-Expt::useFeedback" "normal use" {
	 Expt::useFeedback true
	 Expt::useFeedback
} {^1$}
test "ExptTcl-Expt::useFeedback" "normal use" {
	 Expt::useFeedback yes
	 Expt::useFeedback
} {^1$}
test "ExptTcl-Expt::useFeedback" "normal use" {
	 Expt::useFeedback 1
	 Expt::useFeedback
} {^1$}
test "ExptTcl-Expt::useFeedback" "normal use" {
	 Expt::useFeedback -1.5
	 Expt::useFeedback
} {^1$}
test "ExptTcl-Expt::useFeedback" "normal use" {
	 Expt::useFeedback false
	 Expt::useFeedback
} {^0$}
test "ExptTcl-Expt::useFeedback" "normal use" {
	 Expt::useFeedback no
	 Expt::useFeedback
} {^0$}
test "ExptTcl-Expt::useFeedback" "normal use" {
	 Expt::useFeedback 0
	 Expt::useFeedback
} {^0$}
test "ExptTcl-Expt::useFeedback" "error on non-boolean input" {
	 Expt::useFeedback FLASE
} {^expected boolean value but got "FLASE"$}

### Expt::verboseCmd ###
test "ExptTcl-Expt::verbose" "too many args" {
	 Expt::verbose junk junk
} {^wrong \# args: should be "Expt::verbose \?new_value\?"$}
test "ExptTcl-Expt::verbose" "normal use" {
	 Expt::verbose true
	 Expt::verbose
} {^1$}
test "ExptTcl-Expt::verbose" "normal use" {
	 Expt::verbose yes
	 Expt::verbose
} {^1$}
test "ExptTcl-Expt::verbose" "normal use" {
	 Expt::verbose 1
	 Expt::verbose
} {^1$}
test "ExptTcl-Expt::verbose" "normal use" {
	 Expt::verbose -1.5
	 Expt::verbose
} {^1$}
test "ExptTcl-Expt::verbose" "normal use" {
	 Expt::verbose false
	 Expt::verbose
} {^0$}
test "ExptTcl-Expt::verbose" "normal use" {
	 Expt::verbose no
	 Expt::verbose
} {^0$}
test "ExptTcl-Expt::verbose" "normal use" {
	 Expt::verbose 0
	 Expt::verbose
} {^0$}
test "ExptTcl-Expt::verbose" "error on non-boolean input" {
	 Expt::verbose FLASE
} {^expected boolean value but got "FLASE"$}

### Expt::writeCmd ###
test "ExptTcl-Expt::write" "too few args" {
    Expt::write
} {wrong \# args: should be "Expt::write filename"}
test "ExptTcl-Expt::write" "too many args" {
    Expt::write j u
} {wrong \# args: should be "Expt::write filename"}

if {$test_serialize} {
test "ExptTcl-Expt::write" "read, write, and compare completed expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 set temp_file $::TEST_DIR/temp[pid]_$::DATE
	 Expt::write ${temp_file}_1
	 Expt::read ${temp_file}_1
	 Expt::write ${temp_file}_2 
	 set val [catch {exec diff ${temp_file}_1 ${temp_file}_2} res]
	 exec rm -f ${temp_file}_1 ${temp_file}_2
	 expr $val == 0 && [string compare $res ""] == 0
} {^1$}
test "ExptTcl-Expt::write" "read, write, and compare incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 set temp_file $::TEST_DIR/temp[pid]_$::DATE
	 Expt::write ${temp_file}_1
	 Expt::read ${temp_file}_1
	 Expt::write ${temp_file}_2 
	 set val [catch {exec diff ${temp_file}_1 ${temp_file}_2} res]
	 exec rm -f ${temp_file}_1 ${temp_file}_2
	 expr $val == 0 && [string compare $res ""] == 0
} {^1$}
test "ExptTcl-Expt::write" "error on bad pathname" {
	 exec rm -rf $::TEST_DIR/nonexistent_dir/
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Expt::write $::TEST_DIR/nonexistent_dir/no_file
} "ExptDriver::write: IoFilenameError: $::TEST_DIR/nonexistent_dir/no_file"
}


### Compute test statistics and exit
finish
