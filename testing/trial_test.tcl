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

::testReadWrite TrialTcl $::TRIAL

### Trial::stdInfo ###
test "TrialTcl-Trial::stdInfo" "use when empty" {
    set tr [new Trial]
    set str [Trial::stdInfo $tr]
    regexp {objs ==([^,]*),} $str fullmatch objs
    return "[llength $objs] $objs"
} {^0 }

test "TrialTcl-Trial::stdInfo" "use with one object" {
    set face [new Face]
    set tr [new Trial]
    Trial::addNode $tr $face
    set str [Trial::stdInfo $tr]
    regexp {objs == ([^,]*),} $str fullmatch objs
    return "[expr [string equal $objs Face($face)]] $objs"
} {^1 }

test "TrialTcl-Trial::stdInfo" "use with several objects" {
    set face [new Face]
    set fixpt [new FixPt]
    set house [new House]
    set tr [new Trial]
    Trial::addNode $tr $face
    Trial::addNode $tr $fixpt
    Trial::addNode $tr $house
    set str [Trial::stdInfo $tr]
    regexp {objs == ([^,]*),} $str fullmatch objs
    set idx0 [lsearch -exact $objs "Face($face)"]
    set idx1 [lsearch -exact $objs "FixPt($fixpt)"]
    set idx2 [lsearch -exact $objs "House($house)"]
    return "[llength $objs] $idx0 $idx1 $idx2 $objs"
} {^3 0 1 2 }

test "TrialTcl-Trial::stdInfo" "use with nested objects" {
    set face [new Face]
    set pos [new GxTransform]

    set sep [new GxSeparator]
    set fixpt [new FixPt]
    set house [new House]
    GxSeparator::addChild $sep $fixpt
    GxSeparator::addChild $sep $house

    set tr [new Trial]
    Trial::addNode $tr $sep
    Trial::addNode $tr $face

    set str [Trial::stdInfo $tr]
    regexp {objs == ([^,]*),} $str fullmatch objs

    set idx0 [lsearch -exact $objs "FixPt($fixpt)"]
    set idx1 [lsearch -exact $objs "House($house)"]
    set idx2 [lsearch -exact $objs "Face($face)"]
    return "[llength $objs] $idx0 $idx1 $idx2 $objs"
} {^3 0 1 2 }

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
} {^1.*expected.*but got}

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
} {^1.*expected.*but got}

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
