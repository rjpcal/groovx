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

### Expt::stopCmd ###
test "ExptTcl-Expt::stop" "too many args" {
	 Expt::stop junk
} {^wrong \# args: should be "Expt::stop"$}
test "ExptTcl-Expt::stop" "normal use" {} {^$}
test "ExptTcl-Expt::stop" "error" {} $BLANK $no_test

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

