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
package require Objlist
package require Poslist
package require Tlist
package require Block
package require Blocklist
package require Rhlist
package require Nullrh
package require Thlist
package require Th

if { ![Togl::inited] } { Togl::init "-rgba false"; update }

if { [BlockList::count] == 0 } { Block::Block }
if { [RhList::count] == 0 } { NullRh::NullRh }
if { [ThList::count] == 0 } { Th::Th }

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
	 BlockList::reset
	 Tlist::reset
	 ObjList::reset
	 PosList::reset
	 set files {expt215302Aug1999.asw.gz expt215012Jan2000.asw.gz expt232423May2000.asw.gz}
	 set ocounts {272 181 206}
	 srand [clock clicks]
	 set i [expr int([rand 0 3])]
    Expt::load $::TEST_DIR/[lindex $files $i]
	 set dif [expr [ObjList::count] - [lindex $ocounts $i]]
	 BlockList::reset
	 Tlist::reset
	 ObjList::reset
	 PosList::reset
	 return $dif
} {^0$}
test "ExptTcl-Expt::load" "psyphy samples" {
	 set files {expt080905Oct2000.asw.gz train_2_fishes_or.asw.gz pairs_mfaces_s50.asw.gz}
	 set ocounts {20 10 20}
	 set tcounts {20 10 400}

	 set result ""

	 for {set i 0} {$i < 3} {incr i} {
		  BlockList::reset
		  Tlist::reset
		  ObjList::reset
		  PosList::reset

		  Expt::load $::TEST_DIR/[lindex $files $i]
		  set odif [expr [ObjList::count] - [lindex $ocounts $i]]
		  set tdif [expr [Tlist::count] - [lindex $tcounts $i]]
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
	 set thid [Th::Th]
	 Th::addStartEvent $thid EndTrialEvent 100

	 set rhid [NullRh::NullRh]

	 set face [Face::Face]
	 set pos [Pos::Pos]
	 set trial [Trial::Trial]
	 Trial::add $trial $face $pos

	 Trial::timingHdlr $trial $thid
	 Trial::responseHdlr $trial $rhid

	 Expt::clear

	 BlockList::reset
	 set block [Block::Block]
	 Block::addTrialIds $block $trial

	 namespace eval ::Expt {
		  proc doUponCompletion {} { set ::DONE "complete" }
	 }
	 after 200 set ::DONE "incomplete"
	 Expt::begin

	 vwait ::DONE

	 Expt::reset

	 return $::DONE
} {^complete$}
