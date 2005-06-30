##############################################################################
###
### ExptTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Exptdriver
package require Tlist
package require Toglet
package require Block
package require Nullresponsehdlr
package require Timinghdlr

set EXPT [new ExptDriver]

source ${::TEST_DIR}/io_test.tcl

::testReadWrite ExptTcl $::EXPT

delete $EXPT

# need to add checks on appropriate returns from all functions that use
# isComplete()
# i.e. try to begin a trial after the expt is done, try to abort a trial on 
# an expt that hasn't started, etc.
# make sure to get trialDescription on empty + completed expt's
# try all different possibilities on init

### ExptDriver::begin ###
test "ExptDriver::begin" "too many args" {
    ExptDriver::begin a b
} {^wrong \# args: should be}
test "ExptDriver::begin" "normal use" {
    set expt [new ExptDriver]
    set block [new Block]
    log::copy_to_stdout 0
    -> $expt widget [Toglet::current]
    -> $expt addElement $block
    -> $expt begin
    -> $expt halt
    delete $expt
    log::copy_to_stdout 1
} {^$}
test "ExptDriver::begin" "error" {} $BLANK $no_test

### ExptDriver::pause ###
test "ExptDriver::pause" "too many args" {
    ExptDriver::pause a b
} {^wrong \# args: should be}

### ExptDriver::loadASW ###
test "ExptDriver::loadASW" "too few args" {
    ExptDriver::loadASW
} {wrong \# args: should be}
test "ExptDriver::loadASW" "too many args" {
    ExptDriver::loadASW a b c
} {wrong \# args: should be}
test "ExptDriver::loadASW" "fMRI sample" {
    -> [Toglet::current] setVisible false
    objectdb::clear
    set expt [new ExptDriver]
    set files {expt215302Aug1999.asw.gz expt215012Jan2000.asw.gz expt232423May2000.asw.gz}
    set ocounts {113 166 76}
    set i [expr int(rand()*3)]
    set filename [lindex $files $i]
    puts "filename $filename"
    -> $expt loadASW $::TEST_DIR/$filename
    set dif [expr [GxShapeKit::countAll] - [lindex $ocounts $i]]
    delete $expt
    return $dif
} {^0$}
test "ExptDriver::loadASW" "psyphy samples" {
    set files {expt080905Oct2000.asw.gz train_2_fishes_or.asw.gz pairs_mfaces_s50.asw.gz}
    set ocounts {20 10 20}
    set tcounts {20 10 400}

    set result ""

    for {set i 0} {$i < 3} {incr i} {
        objectdb::clear
        set expt [new ExptDriver]

        -> $expt loadASW $::TEST_DIR/[lindex $files $i]
        set odif [expr [GxShapeKit::countAll] - [lindex $ocounts $i]]
        set tdif [expr [Trial::countAll] - [lindex $tcounts $i]]
        append result "$odif $tdif "
	delete $expt
    }
    return $result
} {^0 0 0 0 0 0 $}

### ExptDriver::saveASW ###
test "ExptDriver::saveASW" "too few args" {
    ExptDriver::saveASW
} {wrong \# args: should be}
test "ExptDriver::saveASW" "too many args" {
    ExptDriver::saveASW a b c
} {wrong \# args: should be}

### ExptDriver::halt ###
test "ExptDriver::halt" "too many args" {
    ExptDriver::halt a b
} {^wrong \# args: should be}

### General experiment tests ###
test "ExptDriver::begin" "general sanity test" {
    set thid [new TimingHdlr]
    -> $thid addStartEvent EndTrialEvent 100

    set face [new Face]
    set trial [new Trial]
    -> $trial addNode $face

    -> $trial timingHdlr $thid
    -> $trial responseHdlr [new NullResponseHdlr]

    set block [new Block]
    -> $block addElements $trial

    set expt [new ExptDriver]
    -> $expt widget [Toglet::current]
    -> $expt addElement $block

    set ::DONE 0
    -> $expt doWhenComplete { set ::DONE 1; set ::STOP 1 }
    set id [after 2000 set ::STOP 1]
    log::copy_to_stdout 0
    -> $expt begin

    vwait ::STOP

    after cancel $id

    -> $expt reset
    log::copy_to_stdout 1

    return $::DONE
} {^1$}
