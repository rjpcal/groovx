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

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd ExptTcl IO 1 [ExptDriver::current]
IO::testDestringifyCmd ExptTcl IO 1 [ExptDriver::current]
IO::testWriteCmd ExptTcl IO 1 [ExptDriver::current]
IO::testReadCmd ExptTcl IO 1 [ExptDriver::current]

# need to add checks on appropriate returns from all functions that use
# isComplete()
# i.e. try to begin a trial after the expt is done, try to abort a trial on 
# an expt that hasn't started, etc.
# make sure to get trialDescription on empty + completed expt's
# try all different possibilities on init

### Expt::beginCmd ###
test "ExptTcl-Expt::begin" "too many args" {
    Expt::begin junk
} {^wrong \# args: should be}
test "ExptTcl-Expt::begin" "normal use" {
    Expt::clear
    set block [Obj::new Block]
    Expt::addBlock $block
    Expt::begin
    Expt::stop
} {^$}
test "ExptTcl-Expt::begin" "error" {} $BLANK $no_test

### Expt::pauseCmd ###
test "ExptTcl-Expt::pause" "too many args" {
    Expt::pause junk
} {^wrong \# args: should be}
test "ExptTcl-Expt::pause" "normal use" {} {^$}
test "ExptTcl-Expt::pause" "error" {} $BLANK $no_test

### Expt::loadCmd ###
test "ExptTcl-Expt::load" "too few args" {
    Expt::load
} {wrong \# args: should be}
test "ExptTcl-Expt::load" "too many args" {
    Expt::load j u
} {wrong \# args: should be}
test "ExptTcl-Expt::load" "fMRI sample" {
    Togl::setVisible false
    Expt::clear
    ObjDb::clear
    set files {expt215302Aug1999.asw.gz expt215012Jan2000.asw.gz expt232423May2000.asw.gz}
    set ocounts {113 166 76}
    set i [expr int(rand()*3)]
    set filename [lindex $files $i]
    puts "filename $filename"
    Expt::load $::TEST_DIR/$filename
    set dif [expr [GxShapeKit::countAll] - [lindex $ocounts $i]]
    return $dif
} {^0$}
test "ExptTcl-Expt::load" "psyphy samples" {
    set files {expt080905Oct2000.asw.gz train_2_fishes_or.asw.gz pairs_mfaces_s50.asw.gz}
    set ocounts {20 10 20}
    set tcounts {20 10 400}

    set result ""

    for {set i 0} {$i < 3} {incr i} {
        Expt::clear
        ObjDb::clear

        Expt::load $::TEST_DIR/[lindex $files $i]
        set odif [expr [GxShapeKit::countAll] - [lindex $ocounts $i]]
        set tdif [expr [Trial::countAll] - [lindex $tcounts $i]]
        append result "$odif $tdif "
    }
    return $result
} {^0 0 0 0 0 0 $}

### Expt::saveCmd ###
test "ExptTcl-Expt::save" "too few args" {
    Expt::save
} {wrong \# args: should be}
test "ExptTcl-Expt::save" "too many args" {
    Expt::save j u
} {wrong \# args: should be}

### Expt::stopCmd ###
test "ExptTcl-Expt::stop" "too many args" {
    Expt::stop junk
} {^wrong \# args: should be}
test "ExptTcl-Expt::stop" "normal use" {} {^$}
test "ExptTcl-Expt::stop" "error" {} $BLANK $no_test

### General experiment tests ###
test "ExptTcl-Expt::begin" "general sanity test" {
    set thid [Obj::new TimingHdlr]
    Th::addStartEvent $thid EndTrialEvent 100

    set rhid [Obj::new NullResponseHdlr]

    set face [Obj::new Face]
    set trial [Obj::new Trial]
    Trial::addNode $trial $face

    Trial::timingHdlr $trial $thid
    Trial::responseHdlr $trial $rhid

    set block [Obj::new Block]
    Block::addTrialIds $block $trial

    Expt::clear
    Expt::addBlock $block

    set ::DONE 0
    Expt::doWhenComplete { set ::DONE 1; set ::STOP 1 }
    after 2000 set ::STOP 1
    Expt::begin

    vwait ::STOP

    Expt::reset

    return $::DONE
} {^1$}
