##############################################################################
###
### TlistTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### Tlist::undrawCmd ###
test "TlistTcl-Tlist::undraw" "too many args" {
	 Tlist::undraw junk
} {wrong \# args: should be "Tlist::undraw"}
test "TlistTcl-Tlist::undraw" "normal use" {
	 Tlist::reset
	 set f [Face::face]
	 set p [Pos::position]
	 Tlist::addObject 0 $f $p
	 setForeground 1
	 setBackground 0
	 clearscreen
	 Tlist::show 0
	 Tlist::undraw
	 pixelCheckSum 0 0 $::TOGL_WIDTH $::TOGL_HEIGHT
} {^0$}
test "TlistTcl-Tlist::undraw" "no error" {} $BLANK $no_test

### Tlist::redrawCmd ###
test "TlistTcl-Tlist::redraw" "too many args" {
	 Tlist::redraw junk
} {wrong \# args: should be "Tlist::redraw"}
test "TlistTcl-Tlist::redraw" "normal use" {
	 Tlist::reset
	 set f [Face::face]
	 set p [Pos::position]
	 Tlist::addObject 0 $f $p
	 Tlist::setCurTrial 0
	 setForeground 1
	 setBackground 0
	 Tlist::redraw
	 # check to see if some pixels actually got drawn
	 expr [pixelCheckSum 0 0 $::TOGL_WIDTH $::TOGL_HEIGHT] > 500
} {^1$}
test "TlistTcl-Tlist::redraw" "no error" {} $BLANK $no_test

### Tlist::clearscreenCmd ###
test "TlistTcl-Tlist::clearscreen" "too many args" {
    Tlist::clearscreen junk
} {wrong \# args: should be "Tlist::clearscreen"}
test "TlistTcl-Tlist::clearscreen" "normal use" {
	 setBackground 0
	 Tlist::clearscreen
	 pixelCheckSum 0 0 $::TOGL_WIDTH $::TOGL_HEIGHT
} {^0$}
test "TlistTcl-Tlist::clearscreen" "no error" {} $BLANK $no_test

### Tlist::showCmd ###
test "TlistTcl-Tlist::show" "too few args" {
	 Tlist::show
} {wrong \# args: should be "Tlist::show trial_id"}
test "TlistTcl-Tlist::show" "too many args" {
	 Tlist::show j u
} {wrong \# args: should be "Tlist::show trial_id"}
test "TlistTcl-Tlist::show" "normal use on valid trial id" {
	 catch {Tlist::show 10}
} {^0$}
test "TlistTcl-Tlist::show" "normal use on invalid trial id" { 
	 catch {Tlist::show -1}
} {^0$}
test "TlistTcl-Tlist::show" "no error" {} $BLANK $no_test

### Tlist::countCmd ###
test "TlistTcl-Tlist::count" "too many args" {
	 Tlist::count junk
} {wrong \# args: should be "Tlist::count"}
test "TlistTcl-Tlist::count" "normal use on empty Tlist" {
	 Tlist::reset
	 Tlist::count
} {^0$}
test "TlistTcl-Tlist::count" "normal use on non-empty Tlist" {
	 Tlist::reset
	 set f [Face::face]
	 set p [Pos::position]
	 Tlist::addObject 0 $f $p
	 Tlist::addObject 3 $f $p
	 Tlist::count
} {^2$}
test "TlistTcl-Tlist::count" "no error" {} $BLANK $no_test

### Tlist::setVisibleCmd ###
test "TlistTcl-Tlist::setVisible" "too few args" {
    Tlist::setVisible
} {wrong \# args: should be "Tlist::setVisible new_value"}
test "TlistTcl-Tlist::setVisible" "too many args" {
    Tlist::setVisible j u
} {wrong \# args: should be "Tlist::setVisible new_value"}
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
	 set f [Face::face]
	 set p [Pos::position]
	 catch {Tlist::addObject 5 $f $p}
} {^0$}
test "TlistTcl-Tlist::addObject" "error on bad objid" {
	 set p [Pos::position]
    Tlist::addObject 5 -1 $p
} {Tlist::addObject: objid out of range}
test "TlistTcl-Tlist::addObject" "error on bad posid" {
	 set f [Face::face]
    Tlist::addObject 5 $f -1
} {Tlist::addObject: posid out of range}
test "TlistTcl-Tlist::addObject" "error on bad trial id" {
	 set f [Face::face]
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
	 set f [Face::face]
	 set p [Pos::position]
	 Tlist::addObject 0 0 0
	 catch {Tlist::setCurTrial 0}
} {^0$}
test "TlistTcl-Tlist::setCurTrial" "error on too low trial id" {
    Tlist::setCurTrial -1
} {Tlist::setCurTrial: invalid trial id}
test "TlistTcl-Tlist::setCurTrial" "error on too large trial id" {
	 Tlist::reset
    Tlist::setCurTrial 10000
} {Tlist::setCurTrial: invalid trial id}

### Tlist::resetCmd ###
test "TlistTcl-Tlist::reset" "too many args" {
	 Tlist::reset junk
} {wrong \# args: should be "Tlist::reset"}
test "TlistTcl-Tlist::reset" "normal use" {
	 ObjList::reset
	 PosList::reset
	 set f [Face::face]
	 set p [Pos::position]
	 Tlist::addObject 0 $f $p
	 Tlist::addObject 1 $f $p
	 Tlist::reset
	 Tlist::count
} {^0$}
test "TlistTcl-Tlist::reset" "no error" {} $BLANK $no_test

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
	 Tlist::stringify
} "Tlist 3 3
0 Trial 3 1 0  1 1  2 2  0  -1.*
1 Trial 2 3 0  4 1  0  -1.*
2 Trial 4 6 0  3 1  9 2  11 3  0  -1.*
0 $BIT"
test "TlistTcl-Tlist::loadObjidFile" "read with fixed # lines, and offset" {
	 ObjList::reset
	 PosList::reset
	 Tlist::loadObjidFile $::TEST_DIR/objid_file 2 1
	 Tlist::stringify
} "Tlist 2 2
0 Trial 3 2 0  2 1  3 2  0  -1.*
1 Trial 2 4 0  5 1  0  -1.*
0 \[01\]"
test "TlistTcl-Tlist::loadObjidFile" "read empty file" {
	 ObjList::reset
	 PosList::reset
	 Tlist::loadObjidFile $::TEST_DIR/empty_file -1 0
	 Tlist::stringify
} "Tlist 0 0
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
	 ObjList::reset
	 PosList::reset
	 Face::loadFaces $::TEST_DIR/faces_file
	 Tlist::makeSummaryTrial 0 5 1.0 2.0 3.0
} {^0$}
test "TlistTcl-Tlist::makeSummaryTrial" "error on bad trialid" {
	 ObjList::reset
	 PosList::reset
	 Face::loadFaces $::TEST_DIR/faces_file
	 Tlist::makeSummaryTrial -1 5 1.0 2.0 3.0
} {^Tlist::makeSummaryTrial: invalid trial id$}
test "TlistTcl-Tlist::makeSummaryTrial" "error on bad num_cols" {
	 ObjList::reset
	 PosList::reset
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
	 ObjList::reset
	 PosList::reset
	 Face::face
	 Face::face
	 Face::face
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Tlist::stringify
} "Tlist $INT 3
0 Trial 1 0 0  0  0.*
1 Trial 1 1 0  0  0.*
2 Trial 1 2 0  0  0.*
0 $BIT"
test "TlistTcl-Tlist::makeSingles" "normal use with empty ObjList" {
	 ObjList::reset
	 PosList::reset
	 set p [Pos::position]
	 Tlist::makeSingles $p
	 Tlist::stringify
} "Tlist $INT 0
0 $BIT"
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
	 ObjList::reset
	 PosList::reset
	 Face::face
	 Face::face
	 set p1 [Pos::position]
	 set p2 [Pos::position]
	 Tlist::makePairs $p1 $p2
	 Tlist::stringify
} "Tlist $INT 4
0 Trial 2 0 0  0 1  0  1.*
1 Trial 2 0 0  1 1  0  0.*
2 Trial 2 1 0  0 1  0  0.*
3 Trial 2 1 0  1 1  0  1.*
0 $BIT"
test "TlistTcl-Tlist::makePairs" "normal use with empty ObjList" {
	 ObjList::reset
	 PosList::reset
	 set p1 [Pos::position]
	 set p2 [Pos::position]
	 Tlist::makePairs $p1 $p2
	 Tlist::stringify
} "Tlist $INT 0
0 $BIT"
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
	 Face::face
	 Face::face
	 Face::face
	 set p1 [Pos::position]
	 set p2 [Pos::position]
	 set p3 [Pos::position]
	 Tlist::makeTriads $p1 $p2 $p3
	 Tlist::count
} {^18$}
test "TlistTcl-Tlist::makeTriads" "normal use on two GrObj's" {
	 ObjList::reset
	 PosList::reset
	 Face::face
	 Face::face
	 set p1 [Pos::position]
	 set p2 [Pos::position]
	 set p3 [Pos::position]
	 Tlist::makeTriads $p1 $p2 $p3
	 Tlist::count
} {^0$}
test "TlistTcl-Tlist::makeTriads" "normal use on empty ObjList" {
	 ObjList::reset
	 PosList::reset
	 set p1 [Pos::position]
	 Tlist::makeTriads $p1 $p1 $p1
	 Tlist::count
} {^0$}
test "TlistTcl-Tlist::makeTriads" "check that Tlist is cleared first" {
	 # Put some random stuff in the Tlist first
	 ObjList::reset
	 PosList::reset
	 set f [Face::face]
	 set p [Pos::position]
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
test "TlistTcl-Tlist::stringify" "too many args" {
    Tlist::stringify junk
} {wrong \# args: should be "Tlist::stringify"}
test "TlistTcl-Tlist::stringify" "normal use with everything empty" {
	 ObjList::reset
	 PosList::reset
	 Tlist::reset
	 Tlist::stringify
} "Tlist $INT 0
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
	 ObjList::reset
	 PosList::reset
	 Face::face
	 Face::face
	 set p1 [Pos::position]
	 set p2 [Pos::position]
	 Tlist::makePairs $p1 $p2
	 set str [Tlist::stringify]
	 Tlist::reset
	 Tlist::destringify $str
	 set str2 [Tlist::stringify]
	 expr [string compare $str $str2] == 0
} {^1$}
test "TlistTcl-Tlist::destringify" "error on junk input" {
	 Tlist::destringify "what a bunch of hogwash this is"
} {Tlist::destringify: InputError: Tlist}

