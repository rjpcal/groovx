##############################################################################
###
### TlistTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Face
package require Pos
package require Tlist
package require Objtogl

source ${::TEST_DIR}/list_test.tcl
List::testList TlistTcl Tlist Trial Trial Trial

if { ![Togl::inited] } { Togl::init "-rgba false"; update }

### Tlist::addObjectCmd ###
test "TlistTcl-Tlist::addObject" "too few args" {
    Tlist::addObject
} {wrong \# args: should be "Tlist::addObject trial objid posid"}
test "TlistTcl-Tlist::addObject" "too many args" {
    Tlist::addObject j u n k
} {wrong \# args: should be "Tlist::addObject trial objid posid"}
test "TlistTcl-Tlist::addObject" "normal use" {
	 set f [Face::Face]
	 set p [Pos::Pos]
	 catch {Tlist::addObject 5 $f $p}
} {^0$}
test "TlistTcl-Tlist::addObject" "error on bad objid" {
	 set p [Pos::Pos]
    Tlist::addObject 5 -1 $p
} {Tlist::addObject: objid out of range}
test "TlistTcl-Tlist::addObject" "error on bad posid" {
	 set f [Face::Face]
    Tlist::addObject 5 $f -1
} {Tlist::addObject: posid out of range}
test "TlistTcl-Tlist::addObject" "error on bad trial id" {
	 set f [Face::Face]
	 set p [Pos::Pos]
    Tlist::addObject -1 $f $p
} {Tlist::addObject: invalid trial id}

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
	 ObjList::reset
	 PosList::reset
	 Tlist::loadObjidFile $::TEST_DIR/objid_file -1 0
	 Tlist::count
} {^3$}
test "TlistTcl-Tlist::loadObjidFile" "read with fixed # lines, and offset" {
	 ObjList::reset
	 PosList::reset
	 Tlist::loadObjidFile $::TEST_DIR/objid_file 2 1
	 Tlist::count
} {^2$}
test "TlistTcl-Tlist::loadObjidFile" "read empty file" {
	 ObjList::reset
	 PosList::reset
	 Tlist::loadObjidFile $::TEST_DIR/empty_file -1 0
	 Tlist::stringify
	 Tlist::count
} {^0$}
test "TlistTcl-Tlist::loadObjidFile" "error on junk text file" {
	 Tlist::loadObjidFile $::TEST_DIR/junk_text_file -1 0
} {Tlist::loadObjidFile: IO::InputError: Trial}
test "TlistTcl-Tlist::loadObjidFile" "error on junk binary file" {
	 Tlist::loadObjidFile $::TEST_DIR/junk_bin_file -1 0
} {Tlist::loadObjidFile: IO::InputError: Trial} \
  [ expr [string equal $env(ARCH) "irix6"] ? $skip_known_bug : $normal_test]


### Tlist::makeSummaryTrialCmd ###
test "TlistTcl-Tlist::makeSummaryTrial" "too few args" {
	 Tlist::makeSummaryTrial
} {^wrong \# args: should be "Tlist::makeSummaryTrial trialid num_cols scale \?xstep\? \?ystep\?"$}
test "TlistTcl-Tlist::makeSummaryTrial" "too many args" {
	 Tlist::makeSummaryTrial 2 3 4 5 6 7
} {^wrong \# args: should be "Tlist::makeSummaryTrial trialid num_cols scale \?xstep\? \?ystep\?"$}
test "TlistTcl-Tlist::makeSummaryTrial" "normal use" {
	 ObjList::reset
	 PosList::reset
	 ObjList::loadObjects $::TEST_DIR/faces_file
	 Tlist::makeSummaryTrial 0 5 1.0 2.0 3.0
} {^0$}
test "TlistTcl-Tlist::makeSummaryTrial" "error on bad trialid" {
	 ObjList::reset
	 PosList::reset
	 ObjList::loadObjects $::TEST_DIR/faces_file
	 Tlist::makeSummaryTrial -1 5 1.0 2.0 3.0
} {^Tlist::makeSummaryTrial: invalid trial id$}
test "TlistTcl-Tlist::makeSummaryTrial" "error on bad num_cols" {
	 ObjList::reset
	 PosList::reset
	 ObjList::loadObjects $::TEST_DIR/faces_file
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
	 Tlist::reset
	 ObjList::reset
	 PosList::reset
	 Face::Face
	 Face::Face
	 Face::Face
	 set p [Pos::Pos]
	 Tlist::makeSingles $p
	 Tlist::count
} {^3$}
test "TlistTcl-Tlist::makeSingles" "normal use with empty ObjList" {
	 Tlist::reset
	 ObjList::reset
	 PosList::reset
	 set p [Pos::Pos]
	 Tlist::makeSingles $p
	 Tlist::count
} {^0$}
test "TlistTcl-Tlist::makeSingles" "error on bad posid" {
	 ObjList::reset
	 PosList::reset
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
	 Tlist::reset
	 ObjList::reset
	 PosList::reset
	 Face::Face
	 Face::Face
	 set p1 [Pos::Pos]
	 set p2 [Pos::Pos]
	 Tlist::makePairs $p1 $p2
	 Tlist::count
} {^4$}
test "TlistTcl-Tlist::makePairs" "normal use with empty ObjList" {
	 ObjList::reset
	 PosList::reset
	 set p1 [Pos::Pos]
	 set p2 [Pos::Pos]
	 Tlist::makePairs $p1 $p2
	 Tlist::count
} {^0$}
test "TlistTcl-Tlist::makePairs" "error on bad posid" {
	 ObjList::reset
	 PosList::reset
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
	 ObjList::reset
	 PosList::reset
	 Face::Face
	 Face::Face
	 Face::Face
	 set p1 [Pos::Pos]
	 set p2 [Pos::Pos]
	 set p3 [Pos::Pos]
	 Tlist::makeTriads $p1 $p2 $p3
	 Tlist::count
} {^18$}
test "TlistTcl-Tlist::makeTriads" "normal use on two GrObj's" {
	 ObjList::reset
	 PosList::reset
	 Face::Face
	 Face::Face
	 set p1 [Pos::Pos]
	 set p2 [Pos::Pos]
	 set p3 [Pos::Pos]
	 Tlist::makeTriads $p1 $p2 $p3
	 Tlist::count
} {^0$}
test "TlistTcl-Tlist::makeTriads" "normal use on empty ObjList" {
	 ObjList::reset
	 PosList::reset
	 set p1 [Pos::Pos]
	 Tlist::makeTriads $p1 $p1 $p1
	 Tlist::count
} {^0$}
test "TlistTcl-Tlist::makeTriads" "check that Tlist is cleared first" {
	 # Put some random stuff in the Tlist first
	 ObjList::reset
	 PosList::reset
	 set f [Face::Face]
	 set p [Pos::Pos]
	 Tlist::addObject 0 $f $p
	 Tlist::addObject 3 $f $p
	 # Now, this should clear the Tlist before it does anything else, and since
	 # there aren't enough GrObj's for triads, it should do nothing else.
	 Tlist::makeTriads $p $p $p
	 Tlist::count
} {^0$}
test "TlistTcl-Tlist::makeTriads" "error on bad posid" {
	 ObjList::reset
	 PosList::reset
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
### Tlist::destringifyCmd ###
test "TlistTcl-Tlist::destringify" "write, read, write and compare" {
	 ObjList::reset
	 PosList::reset
	 Face::Face
	 Face::Face
	 set p1 [Pos::Pos]
	 set p2 [Pos::Pos]
	 Tlist::makePairs $p1 $p2
	 set str [Tlist::stringify]
	 Tlist::reset
	 Tlist::destringify $str
	 set str2 [Tlist::stringify]
	 expr [string compare $str $str2] == 0
} {^1$}

