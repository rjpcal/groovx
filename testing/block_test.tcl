##############################################################################
###
### BlockTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

BlockList::reset
set ::BLOCK [Block::Block]

### Block::currentTrialCmd ###
test "BlockTcl-Block::currentTrial" "too few args" {
	 Block::currentTrial
} {wrong \# args: should be "Block::currentTrial item_id\(s\)"}
test "BlockTcl-Block::currentTrial" "too many args" {
	 Block::currentTrial $::BLOCK junk
} {wrong \# args: should be "Block::currentTrial item_id\(s\)"}

test "BlockTcl-Block::currentTrial" "normal use on empty block" {
	 Block::removeAllTrials $::BLOCK
	 Block::currentTrial $::BLOCK
} {^\-1$}
test "BlockTcl-Block::currentTrial" "no error" {} $BLANK $no_test

### Block::currentTrialTypeCmd ###
test "BlockTcl-Block::currentTrialType" "too few args" {
    Block::currentTrialType 
} {wrong \# args: should be "Block::currentTrialType item_id\(s\)"}
test "BlockTcl-Block::currentTrialType" "too many args" {
    Block::currentTrialType $::BLOCK junk
} {wrong \# args: should be "Block::currentTrialType item_id\(s\)"}

test "BlockTcl-Block::currentTrialType" "normal use on empty expt" {
	 Block::removeAllTrials $::BLOCK
	 Block::currentTrialType $::BLOCK
} {^-1$}
test "BlockTcl-Block::currentTrialType" "error" {} $BLANK $no_test

### Block::isCompleteCmd ###
test "BlockTcl-Block::isComplete" "too few args" {
    Block::isComplete 
} {wrong \# args: should be "Block::isComplete item_id\(s\)"}
test "BlockTcl-Block::isComplete" "too many args" {
    Block::isComplete $::BLOCK junk
} {wrong \# args: should be "Block::isComplete item_id\(s\)"}

test "BlockTcl-Block::isComplete" "normal use on empty expt" {
	 Block::removeAllTrials $::BLOCK
	 Block::isComplete $::BLOCK
} {^1$}
test "BlockTcl-Block::isComplete" "error" {} $BLANK $no_test

### Block::numCompletedCmd ###
test "BlockTcl-Block::numCompleted" "too few args" {
	 Block::numCompleted
} {wrong \# args: should be "Block::numCompleted item_id\(s\)"}
test "BlockTcl-Block::numCompleted" "too many args" {
	 Block::numCompleted $::BLOCK junk
} {wrong \# args: should be "Block::numCompleted item_id\(s\)"}

test "BlockTcl-Block::numCompleted" "use on empty expt" {
	 Block::removeAllTrials $::BLOCK
	 Block::numCompleted $::BLOCK
} {^0$}
test "BlockTcl-Block::numCompleted" "no error" {} $BLANK $no_test

### Block::numTrialsCmd ###
test "BlockTcl-Block::numTrials" "too few args" {
	 Block::numTrials 
} {wrong \# args: should be "Block::numTrials item_id\(s\)"}
test "BlockTcl-Block::numTrials" "too many args" {
	 Block::numTrials $::BLOCK junk
} {wrong \# args: should be "Block::numTrials item_id\(s\)"}

test "BlockTcl-Block::numTrials" "use on empty expt" {
	 Block::removeAllTrials $::BLOCK
	 Block::numTrials	$::BLOCK 
} {^0$}
test "BlockTcl-Block::numTrials" "no error" {} $BLANK $no_test

### Block::prevResponseCmd ###
test "BlockTcl-Block::prevResponse" "too few args" {
	 Block::prevResponse 
} {wrong \# args: should be "Block::prevResponse item_id\(s\)"}
test "BlockTcl-Block::prevResponse" "too many args" {
	 Block::prevResponse $::BLOCK junk
} {wrong \# args: should be "Block::prevResponse item_id\(s\)"}

test "BlockTcl-Block::prevResponse" "normal use on empty expt" {
	 Block::removeAllTrials $::BLOCK
	 Block::prevResponse $::BLOCK
} {^\-1$}
test "BlockTcl-Block::prevResponse" "error" {} $BLANK $no_test

### Block::trialDescriptionCmd ###
test "BlockTcl-Block::trialDescription" "too few args" {
    Block::trialDescription 
} {wrong \# args: should be "Block::trialDescription item_id\(s\)"}
test "BlockTcl-Block::trialDescription" "too many args" {
    Block::trialDescription $::BLOCK junk
} {wrong \# args: should be "Block::trialDescription item_id\(s\)"}

test "BlockTcl-Block::trialDescription" "normal use on empty expt" {
	 Block::removeAllTrials $::BLOCK
	 Block::trialDescription $::BLOCK
} {^block is complete$}
test "BlockTcl-Block::trialDescription" "error" {} $BLANK $no_test

### Block::verboseCmd ###
test "BlockTcl-Block::verbose" "too few args" {
	 Block::verbose 
} {^wrong \# args: should be "Block::verbose item_id\(s\) \?new_value\(s\)\?"$}
test "BlockTcl-Block::verbose" "too many args" {
	 Block::verbose junk junk junk
} {^wrong \# args: should be "Block::verbose item_id\(s\) \?new_value\(s\)\?"$}
test "BlockTcl-Block::verbose" "normal use 1" {
	 Block::verbose $::BLOCK true
	 Block::verbose $::BLOCK
} {^1$}
test "BlockTcl-Block::verbose" "normal use 2" {
	 Block::verbose $::BLOCK yes
	 Block::verbose $::BLOCK
} {^1$}
test "BlockTcl-Block::verbose" "normal use 3" {
	 Block::verbose $::BLOCK 1
	 Block::verbose $::BLOCK
} {^1$}
test "BlockTcl-Block::verbose" "normal use 4" {
	 Block::verbose $::BLOCK -1.5
	 Block::verbose $::BLOCK
} {^1$}
test "BlockTcl-Block::verbose" "normal use 5" {
	 Block::verbose $::BLOCK false
	 Block::verbose $::BLOCK
} {^0$}
test "BlockTcl-Block::verbose" "normal use 6" {
	 Block::verbose $::BLOCK no
	 Block::verbose $::BLOCK
} {^0$}
test "BlockTcl-Block::verbose" "normal use 7" {
	 Block::verbose $::BLOCK 0
	 Block::verbose $::BLOCK
} {^0$}
test "BlockTcl-Block::verbose" "error on non-boolean input" {
	 Block::verbose $::BLOCK FLASE
} {expected boolean value but got "FLASE"}
