##############################################################################
###
### ExptTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Expt
package require Objtogl
package require Tlist
package require Block
package require Nullrh
package require Th

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd ExptTcl Expt -1
IO::testDestringifyCmd ExptTcl Expt -1
IO::testWriteCmd ExptTcl Expt -1
IO::testReadCmd ExptTcl Expt -1

if { ![Togl::inited] } { Togl::init; update }

# need to add checks on appropriate returns from all functions that use
# isComplete()
# i.e. try to begin a trial after the expt is done, try to abort a trial on 
# an expt that hasn't started, etc.
# make sure to get trialDescription on empty + completed expt's
# try all different possibilities on init

### Expt::beginCmd ###
test "ExptTcl-Expt::begin" "too many args" {
	 Expt::begin junk
} {^wrong \# args: should be "Expt::begin"$}
test "ExptTcl-Expt::begin" "normal use" {
	 Expt::clear
	 set block [IO::new Block]
	 Expt::addBlock $block
	 Expt::begin
	 Expt::stop
} {^$}
test "ExptTcl-Expt::begin" "error" {} $BLANK $no_test

### Expt::pauseCmd ###
test "ExptTcl-Expt::pause" "too many args" {
	 Expt::pause junk
} {^wrong \# args: should be "Expt::pause"$}
test "ExptTcl-Expt::pause" "normal use" {} {^$}
test "ExptTcl-Expt::pause" "error" {} $BLANK $no_test

### Expt::loadCmd ###
test "ExptTcl-Expt::load" "too few args" {
    Expt::load
} {wrong \# args: should be "Expt::load filename"}
test "ExptTcl-Expt::load" "too many args" {
    Expt::load j u
} {wrong \# args: should be "Expt::load filename"}
test "ExptTcl-Expt::load" "fMRI sample" {
	 Togl::setVisible false
	 Expt::clear
	 IoDb::clear
	 set files {expt215302Aug1999.asw.gz expt215012Jan2000.asw.gz expt232423May2000.asw.gz}
	 set ocounts {113 166 76}
	 set i [expr int(rand()*3)]
	 set filename [lindex $files $i]
	 puts "filename $filename"
    Expt::load $::TEST_DIR/$filename
	 set dif [expr [GrObj::countAll] - [lindex $ocounts $i]]
	 return $dif
} {^0$}
test "ExptTcl-Expt::load" "psyphy samples" {
	 set files {expt080905Oct2000.asw.gz train_2_fishes_or.asw.gz pairs_mfaces_s50.asw.gz}
	 set ocounts {20 10 20}
	 set tcounts {20 10 400}

	 set result ""

	 for {set i 0} {$i < 3} {incr i} {
		  Expt::clear
		  IoDb::clear

		  Expt::load $::TEST_DIR/[lindex $files $i]
		  set odif [expr [GrObj::countAll] - [lindex $ocounts $i]]
		  set tdif [expr [Trial::countAll] - [lindex $tcounts $i]]
		  append result "$odif $tdif "
	 }
	 return $result
} {^0 0 0 0 0 0 $}

### Expt::saveCmd ###
test "ExptTcl-Expt::save" "too few args" {
    Expt::save
} {wrong \# args: should be "Expt::save filename"}
test "ExptTcl-Expt::save" "too many args" {
    Expt::save j u
} {wrong \# args: should be "Expt::save filename"}

### Expt::stopCmd ###
test "ExptTcl-Expt::stop" "too many args" {
	 Expt::stop junk
} {^wrong \# args: should be "Expt::stop"$}
test "ExptTcl-Expt::stop" "normal use" {} {^$}
test "ExptTcl-Expt::stop" "error" {} $BLANK $no_test

### General experiment tests ###
test "ExptTcl-Expt::begin" "general sanity test" {
	 set thid [IO::new TimingHdlr]
	 Th::addStartEvent $thid EndTrialEvent 100

	 set rhid [IO::new NullResponseHdlr]

	 set face [IO::new Face]
	 set pos [IO::new Position]
	 set trial [IO::new Trial]
	 Trial::add $trial $face $pos

	 Trial::timingHdlr $trial $thid
	 Trial::responseHdlr $trial $rhid

	 set block [IO::new Block]
	 Block::addTrialIds $block $trial

	 Expt::clear
	 Expt::addBlock $block

	 namespace eval ::Expt {
		  proc doUponCompletion {} { set ::DONE "complete" }
	 }
	 after 200 set ::DONE "incomplete"
	 Expt::begin

	 vwait ::DONE

	 Expt::reset

	 return $::DONE
} {^complete$}
