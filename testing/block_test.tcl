##############################################################################
###
### BlockTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

set ::BLOCK [Obj::new Block]

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd BlockTcl IO 1 $::BLOCK
IO::testDestringifyCmd BlockTcl IO 1 $::BLOCK
IO::testWriteCmd BlockTcl IO 1 $::BLOCK
IO::testReadCmd BlockTcl IO 1 $::BLOCK

### Block::currentTrial ###
test "Block::currentTrial" "too few args" {
    Block::currentTrial
} {wrong \# args}
test "Block::currentTrial" "too many args" {
    Block::currentTrial $::BLOCK junk
} {wrong \# args}

test "Block::currentTrial" "normal use on empty block" {
    Block::removeAllTrials $::BLOCK
    Block::currentTrial $::BLOCK
} {^0$}

### Block::trialType ###
test "Block::trialType" "too few args" {
    Block::trialType 
} {wrong \# args}
test "Block::trialType" "too many args" {
    Block::trialType $::BLOCK junk
} {wrong \# args}

test "Block::trialType" "normal use on empty expt" {
    Block::removeAllTrials $::BLOCK
    Block::trialType $::BLOCK
} {^-1$}

### Block::isComplete ###
test "Block::isComplete" "too few args" {
    Block::isComplete 
} {wrong \# args}
test "Block::isComplete" "too many args" {
    Block::isComplete $::BLOCK junk
} {wrong \# args}

test "Block::isComplete" "normal use on empty expt" {
    Block::removeAllTrials $::BLOCK
    Block::isComplete $::BLOCK
} {^1$}

### Block::numCompleted ###
test "Block::numCompleted" "too few args" {
    Block::numCompleted
} {wrong \# args}
test "Block::numCompleted" "too many args" {
    Block::numCompleted $::BLOCK junk
} {wrong \# args}

test "Block::numCompleted" "use on empty expt" {
    Block::removeAllTrials $::BLOCK
    Block::numCompleted $::BLOCK
} {^0$}

### Block::numTrials ###
test "Block::numTrials" "too few args" {
    Block::numTrials 
} {wrong \# args}
test "Block::numTrials" "too many args" {
    Block::numTrials $::BLOCK junk
} {wrong \# args}

test "Block::numTrials" "use on empty expt" {
    Block::removeAllTrials $::BLOCK
    Block::numTrials   $::BLOCK 
} {^0$}

### Block::lastResponse ###
test "Block::lastResponse" "too few args" {
    Block::lastResponse 
} {wrong \# args}
test "Block::lastResponse" "too many args" {
    Block::lastResponse $::BLOCK junk
} {wrong \# args}

test "Block::lastResponse" "normal use on empty expt" {
    Block::removeAllTrials $::BLOCK
    Block::lastResponse $::BLOCK
} {^\-1$}

### Block::info ###
test "Block::info" "too few args" {
    Block::info 
} {wrong \# args}
test "Block::info" "too many args" {
    Block::info $::BLOCK junk
} {wrong \# args}

test "Block::info" "normal use on empty expt" {
    Block::removeAllTrials $::BLOCK
    Block::info $::BLOCK
} {^complete$}
