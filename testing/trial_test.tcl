##############################################################################
###
### TrialTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Trial

set TRIAL [IO::new Trial]

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd TrialTcl IO 1 $::TRIAL
IO::testDestringifyCmd TrialTcl IO 1 $::TRIAL
IO::testWriteCmd TrialTcl IO 1 $::TRIAL
IO::testReadCmd TrialTcl IO 1 $::TRIAL

### Trial::description ###
test "TrialTcl-Trial::description" "use when empty" {
    set tr [IO::new Trial]
    set str [Trial::description $tr]
    regexp {objs ==([0-9 ]*),} $str fullmatch objs
    return "[llength $objs]"
} {^0$}

test "TrialTcl-Trial::description" "use with one object" {
    set face [IO::new Face]
    set pos [IO::new Position]
    set tr [IO::new Trial]
    Trial::add $tr $face $pos
    set str [Trial::description $tr]
    regexp {objs == ([0-9 ]*),} $str fullmatch objs
    return "[expr $objs == $face] [llength $objs]"
} {^1 1$}

test "TrialTcl-Trial::description" "use with several objects" {
    set face [IO::new Face]
    set pos [IO::new Position]
    set fixpt [IO::new FixPt]
    set house [IO::new House]
    set tr [IO::new Trial]
    Trial::add $tr $face $pos
    Trial::add $tr $fixpt $pos
    Trial::add $tr $house $pos
    set str [Trial::description $tr]
    regexp {objs == ([0-9 ]*),} $str fullmatch objs
    set idx0 [lsearch -exact $objs $face]
    set idx1 [lsearch -exact $objs $fixpt]
    set idx2 [lsearch -exact $objs $house]
    return "[llength $objs] $idx0 $idx1 $idx2"
} {^3 0 1 2$}

test "TrialTcl-Trial::description" "use with nested objects" {
    set face [IO::new Face]
    set pos [IO::new Position]

    set sep [IO::new GxSeparator]
    set fixpt [IO::new FixPt]
    set house [IO::new House]
    GxSeparator::addChild $sep $fixpt
    GxSeparator::addChild $sep $house

    set tr [IO::new Trial]
    Trial::add $tr $sep $pos
    Trial::add $tr $face $pos

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
} {^wrong \# args: should be "Trial::responseHdlr item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::responseHdlr" "too many args" {
    Trial::responseHdlr $::TRIAL 0 junk
} {^wrong \# args: should be "Trial::responseHdlr item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::responseHdlr" "normal use" {
    Trial::responseHdlr $::TRIAL 0
    Trial::responseHdlr $::TRIAL
} {^0$}
test "TrialTcl-Trial::responseHdlr" "error" {
    set result [catch {Trial::responseHdlr $::TRIAL -1} msg]
    return "$result $msg"
} {^1 Trial::responseHdlr}

### Trial::timingHdlrCmd ###
test "TrialTcl-Trial::timingHdlr" "too few args" {
    Trial::timingHdlr
} {^wrong \# args: should be "Trial::timingHdlr item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::timingHdlr" "too many args" {
    Trial::timingHdlr $::TRIAL 0 junk
} {^wrong \# args: should be "Trial::timingHdlr item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::timingHdlr" "normal use" {
    Trial::timingHdlr $::TRIAL 0
    Trial::timingHdlr $::TRIAL
} {^0$}
test "TrialTcl-Trial::timingHdlr" "error" {
    set result [catch {Trial::timingHdlr $::TRIAL -1} msg]
    return "$result $msg"
} {^1 Trial::timingHdlr}

### Trial::typeCmd ###
test "TrialTcl-Trial::type" "too few args" {
    Trial::type
} {^wrong \# args: should be "Trial::type item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::type" "too many args" {
    Trial::type $::TRIAL 0 junk 
} {^wrong \# args: should be "Trial::type item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::type" "normal use" {
    Trial::type $::TRIAL 0
    Trial::type $::TRIAL
} {^0$}

