##############################################################################
###
### BlockTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### Block::currentTrialCmd ###
test "BlockTcl-Block::currentTrial" "too few args" {
	 Block::currentTrial
} {wrong \# args: should be "Block::currentTrial item_id\(s\)"}
test "BlockTcl-Block::currentTrial" "too many args" {
	 Block::currentTrial 0 junk
} {wrong \# args: should be "Block::currentTrial item_id\(s\)"}

if {$test_serialize} {
test "BlockTcl-Block::currentTrial" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Block::currentTrial 0
} {^5$}
test "BlockTcl-Block::currentTrial" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Block::currentTrial 0
} {\-1}
}

test "BlockTcl-Block::currentTrial" "normal use on empty block" {
	 Block::reset 0
	 Block::currentTrial 0
} {^\-1$}
test "BlockTcl-Block::currentTrial" "no error" {} $BLANK $no_test

### Block::currentTrialTypeCmd ###
test "BlockTcl-Block::currentTrialType" "too few args" {
    Block::currentTrialType 
} {wrong \# args: should be "Block::currentTrialType item_id\(s\)"}
test "BlockTcl-Block::currentTrialType" "too many args" {
    Block::currentTrialType 0 junk
} {wrong \# args: should be "Block::currentTrialType item_id\(s\)"}

if {$test_serialize} {
test "BlockTcl-Block::currentTrialType" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Block::currentTrialType 0
} {^1$}
test "BlockTcl-Block::currentTrialType" "normal use on completed expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 # This should be a no-op
	 Block::currentTrialType 0
} {^-1$}
}

test "BlockTcl-Block::currentTrialType" "normal use on empty expt" {
	 Block::reset 0
	 Block::currentTrialType 0
} {^-1$}
test "BlockTcl-Block::currentTrialType" "error" {} $BLANK $no_test

### Block::isCompleteCmd ###
test "BlockTcl-Block::isComplete" "too few args" {
    Block::isComplete 
} {wrong \# args: should be "Block::isComplete item_id\(s\)"}
test "BlockTcl-Block::isComplete" "too many args" {
    Block::isComplete 0 junk
} {wrong \# args: should be "Block::isComplete item_id\(s\)"}

if {$test_serialize} {
test "BlockTcl-Block::isComplete" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Block::isComplete 0
} {^0$}
test "BlockTcl-Block::isComplete" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Block::isComplete 0
} {^1$}
}

test "BlockTcl-Block::isComplete" "normal use on empty expt" {
	 Block::reset 0
	 Block::isComplete 0
} {^1$}
test "BlockTcl-Block::isComplete" "error" {} $BLANK $no_test

### Block::numCompletedCmd ###
test "BlockTcl-Block::numCompleted" "too few args" {
	 Block::numCompleted
} {wrong \# args: should be "Block::numCompleted item_id\(s\)"}
test "BlockTcl-Block::numCompleted" "too many args" {
	 Block::numCompleted 0 junk
} {wrong \# args: should be "Block::numCompleted item_id\(s\)"}

if {$test_serialize} {
test "BlockTcl-Block::numCompleted" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Block::numCompleted 0
} {^4$}
test "BlockTcl-Block::numCompleted" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Block::numCompleted 0
} {^100$}
}

test "BlockTcl-Block::numCompleted" "use on empty expt" {
	 Block::reset 0
	 Block::numCompleted 0
} {^0$}
test "BlockTcl-Block::numCompleted" "no error" {} $BLANK $no_test

### Block::numTrialsCmd ###
test "BlockTcl-Block::numTrials" "too few args" {
	 Block::numTrials 
} {wrong \# args: should be "Block::numTrials item_id\(s\)"}
test "BlockTcl-Block::numTrials" "too many args" {
	 Block::numTrials 0 junk
} {wrong \# args: should be "Block::numTrials item_id\(s\)"}

if {$test_serialize} {
test "BlockTcl-Block::numTrials" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Block::numTrials	0 
} {^10$}
test "BlockTcl-Block::numTrials" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Block::numTrials	0
} {^100$}
}

test "BlockTcl-Block::numTrials" "use on empty expt" {
	 Block::reset 0
	 Block::numTrials	0 
} {^0$}
test "BlockTcl-Block::numTrials" "no error" {} $BLANK $no_test

### Block::prevResponseCmd ###
test "BlockTcl-Block::prevResponse" "too few args" {
	 Block::prevResponse 
} {wrong \# args: should be "Block::prevResponse item_id\(s\)"}
test "BlockTcl-Block::prevResponse" "too many args" {
	 Block::prevResponse 0 junk
} {wrong \# args: should be "Block::prevResponse item_id\(s\)"}

if {$test_serialize} {
test "BlockTcl-Block::prevResponse" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Block::prevResponse 0
} {^0$}
test "BlockTcl-Block::prevResponse" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Block::prevResponse 0
} {^1$}
}

test "BlockTcl-Block::prevResponse" "normal use on empty expt" {
	 Block::reset 0
	 Block::prevResponse 0
} {^\-1$}
test "BlockTcl-Block::prevResponse" "error" {} $BLANK $no_test

### Block::trialDescriptionCmd ###
test "BlockTcl-Block::trialDescription" "too few args" {
    Block::trialDescription 
} {wrong \# args: should be "Block::trialDescription item_id\(s\)"}
test "BlockTcl-Block::trialDescription" "too many args" {
    Block::trialDescription 0 junk
} {wrong \# args: should be "Block::trialDescription item_id\(s\)"}

if {$test_serialize} {
test "BlockTcl-Block::trialDescription" "normal use on incomplete expt" {
	 Expt::read $::TEST_DIR/expt_in_progress_file
	 Block::trialDescription 0
} {trial id == 5, trial type == 1, objs == 5, categories == 1,\
		  completed 4 of 10}
test "BlockTcl-Block::trialDescription" "normal use on complete expt" {
	 Expt::read $::TEST_DIR/completed_expt_file
	 Block::trialDescription 0
} {^block is complete$}
}

test "BlockTcl-Block::trialDescription" "normal use on empty expt" {
	 Block::reset 0
	 Block::trialDescription 0
} {^block is complete$}
test "BlockTcl-Block::trialDescription" "error" {} $BLANK $no_test

### Block::verboseCmd ###
test "BlockTcl-Block::verbose" "too few args" {
	 Block::verbose 
} {^wrong \# args: should be "Block::verbose item_id\(s\) \?new_value\(s\)\?"$}
test "BlockTcl-Block::verbose" "too many args" {
	 Block::verbose junk junk junk
} {^wrong \# args: should be "Block::verbose item_id\(s\) \?new_value\(s\)\?"$}
test "BlockTcl-Block::verbose" "normal use" {
	 Block::verbose 0 true
	 Block::verbose 0
} {^1$}
test "BlockTcl-Block::verbose" "normal use" {
	 Block::verbose 0 yes
	 Block::verbose 0
} {^1$}
test "BlockTcl-Block::verbose" "normal use" {
	 Block::verbose 0 1
	 Block::verbose 0
} {^1$}
test "BlockTcl-Block::verbose" "normal use" {
	 Block::verbose 0 -1.5
	 Block::verbose 0
} {^1$}
test "BlockTcl-Block::verbose" "normal use" {
	 Block::verbose 0 false
	 Block::verbose 0
} {^0$}
test "BlockTcl-Block::verbose" "normal use" {
	 Block::verbose 0 no
	 Block::verbose 0
} {^0$}
test "BlockTcl-Block::verbose" "normal use" {
	 Block::verbose 0 0
	 Block::verbose 0
} {^0$}
test "BlockTcl-Block::verbose" "error on non-boolean input" {
	 Block::verbose 0 FLASE
} {^expected boolean value but got "FLASE"$}


