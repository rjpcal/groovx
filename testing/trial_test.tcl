##############################################################################
###
### TrialTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Trial

set TRIAL [Obj::new Trial]

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd TrialTcl IO 1 $::TRIAL
IO::testDestringifyCmd TrialTcl IO 1 $::TRIAL
IO::testWriteCmd TrialTcl IO 1 $::TRIAL
IO::testReadCmd TrialTcl IO 1 $::TRIAL

### Trial::description ###
test "TrialTcl-Trial::description" "use when empty" {
    set tr [Obj::new Trial]
    set str [Trial::description $tr]
    regexp {objs ==([0-9 ]*),} $str fullmatch objs
    return "[llength $objs]"
} {^0$}

test "TrialTcl-Trial::description" "use with one object" {
    set face [Obj::new Face]
    set tr [Obj::new Trial]
    Trial::addNode $tr $face
    set str [Trial::description $tr]
    regexp {objs == ([0-9 ]*),} $str fullmatch objs
    return "[expr $objs == $face] [llength $objs]"
} {^1 1$}

test "TrialTcl-Trial::description" "use with several objects" {
    set face [Obj::new Face]
    set fixpt [Obj::new FixPt]
    set house [Obj::new House]
    set tr [Obj::new Trial]
    Trial::addNode $tr $face
    Trial::addNode $tr $fixpt
    Trial::addNode $tr $house
    set str [Trial::description $tr]
    regexp {objs == ([0-9 ]*),} $str fullmatch objs
    set idx0 [lsearch -exact $objs $face]
    set idx1 [lsearch -exact $objs $fixpt]
    set idx2 [lsearch -exact $objs $house]
    return "[llength $objs] $idx0 $idx1 $idx2"
} {^3 0 1 2$}

test "TrialTcl-Trial::description" "use with nested objects" {
    set face [Obj::new Face]
    set pos [Obj::new GxTransform]

    set sep [Obj::new GxSeparator]
    set fixpt [Obj::new FixPt]
    set house [Obj::new House]
    GxSeparator::addChild $sep $fixpt
    GxSeparator::addChild $sep $house

    set tr [Obj::new Trial]
    Trial::addNode $tr $sep
    Trial::addNode $tr $face

    set str [Trial::description $tr]
    regexp {objs == ([0-9 ]*),} $str fullmatch objs

    set idx0 [lsearch -exact $objs $fixpt]
    set idx1 [lsearch -exact $objs $house]
    set idx2 [lsearch -exact $objs $face]
    return "[llength $objs] $idx0 $idx1 $idx2"
} {^3 0 1 2$}

### Trial::responseHdlrCmd ###
test "TrialTcl-Trial::responseHdlr" "too few args" {
    Trial::responseHdlr
} {^wrong \# args: should be}
test "TrialTcl-Trial::responseHdlr" "too many args" {
    Trial::responseHdlr $::TRIAL 0 junk
} {^wrong \# args: should be}
test "TrialTcl-Trial::responseHdlr" "normal use" {
	 set rh [new NullResponseHdlr]
    Trial::responseHdlr $::TRIAL $rh
    expr $rh == [Trial::responseHdlr $::TRIAL]
} {^1$}
test "TrialTcl-Trial::responseHdlr" "error" {
    set result [catch {Trial::responseHdlr $::TRIAL -1} msg]
    return "$result $msg"
} {^1 Trial::responseHdlr}

### Trial::timingHdlrCmd ###
test "TrialTcl-Trial::timingHdlr" "too few args" {
    Trial::timingHdlr
} {^wrong \# args: should be}
test "TrialTcl-Trial::timingHdlr" "too many args" {
    Trial::timingHdlr $::TRIAL 0 junk
} {^wrong \# args: should be}
test "TrialTcl-Trial::timingHdlr" "normal use" {
	 set th [new TimingHdlr]
    Trial::timingHdlr $::TRIAL $th
    expr $th == [Trial::timingHdlr $::TRIAL]
} {^1$}
test "TrialTcl-Trial::timingHdlr" "error" {
    set result [catch {Trial::timingHdlr $::TRIAL -1} msg]
    return "$result $msg"
} {^1 Trial::timingHdlr}

### Trial::typeCmd ###
test "TrialTcl-Trial::type" "too few args" {
    Trial::type
} {^wrong \# args: should be}
test "TrialTcl-Trial::type" "too many args" {
    Trial::type $::TRIAL 0 junk 
} {^wrong \# args: should be}
test "TrialTcl-Trial::type" "normal use" {
    Trial::type $::TRIAL 0
    Trial::type $::TRIAL
} {^0$}

