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

if { ![Togl::inited] } { Togl::init "-rgba false"; update }

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
	 puts "val == $val"
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
	 ObjList::reset
	 PosList::reset
	 set p [Pos::Pos]
	 Tlist::makeSingles $p
	 Block::init 0 1 0
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
	 ObjList::reset
	 PosList::reset
	 set p [Pos::Pos]
	 Tlist::makeSingles $p
	 Block::init 0 1 0
	 # This should be a no-op
	 catch {Expt::beginTrial}
} {^0$} $Expt::haveTest
}

test "ExptTcl-Expt::beginTrial" "error" {} $BLANK $no_test

### Expt::pauseCmd ###
test "ExptTcl-Expt::pause" "too many args" {
	 Expt::pause junk
} {^wrong \# args: should be "Expt::pause"$}
test "ExptTcl-Expt::pause" "normal use" {} {^$}
test "ExptTcl-Expt::pause" "error" {} $BLANK $no_test

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
				[ObjList::count] == 10 && \
				[PosList::count] == 1 && \
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
	 ObjList::reset
	 PosList::reset
	 set p [Pos::Pos]
	 Tlist::makeSingles $p
	 Block::init 0 1 0
	 catch {Expt::recordResponse 5}
} {^0$} $Expt::haveTest
test "ExptTcl-Expt::recordResponse" "error" {} $BLANK $no_test

### Expt::stopCmd ###
test "ExptTcl-Expt::stop" "too many args" {
	 Expt::stop junk
} {^wrong \# args: should be "Expt::stop"$}
test "ExptTcl-Expt::stop" "normal use" {} {^$}
test "ExptTcl-Expt::stop" "error" {} $BLANK $no_test

### Expt::undrawTrialCmd ###
test "ExptTcl-Expt::undrawTrial" "too many args" {
	 Expt::undrawTrial junk
} {^wrong \# args: should be "Expt::undrawTrial"$} $Expt::haveTest
test "ExptTcl-Expt::undrawTrial" "normal use" {} {^$} $Expt::haveTest
test "ExptTcl-Expt::undrawTrial" "error" {} $BLANK $no_test

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

