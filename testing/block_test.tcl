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

### Block::currentElement ###
test "Block::currentElement" "too few args" {
    Block::currentElement
} {wrong \# args}
test "Block::currentElement" "too many args" {
    Block::currentElement $::BLOCK junk
} {wrong \# args}

test "Block::currentElement" "normal use on empty block" {
    Block::clearElements $::BLOCK
    Block::currentElement $::BLOCK
} {^0$}

### Block::trialType ###
test "Block::trialType" "too few args" {
    Block::trialType 
} {wrong \# args}
test "Block::trialType" "too many args" {
    Block::trialType $::BLOCK junk
} {wrong \# args}

test "Block::trialType" "normal use on empty expt" {
    Block::clearElements $::BLOCK
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
    Block::clearElements $::BLOCK
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
    Block::clearElements $::BLOCK
    Block::numCompleted $::BLOCK
} {^0$}

### Block::numElements ###
test "Block::numElements" "too few args" {
    Block::numElements 
} {wrong \# args}
test "Block::numElements" "too many args" {
    Block::numElements $::BLOCK junk
} {wrong \# args}

test "Block::numElements" "use on empty expt" {
    Block::clearElements $::BLOCK
    Block::numElements   $::BLOCK 
} {^0$}

### Block::lastResponse ###
test "Block::lastResponse" "too few args" {
    Block::lastResponse 
} {wrong \# args}
test "Block::lastResponse" "too many args" {
    Block::lastResponse $::BLOCK junk
} {wrong \# args}

test "Block::lastResponse" "normal use on empty expt" {
    Block::clearElements $::BLOCK
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
    Block::clearElements $::BLOCK
    Block::info $::BLOCK
} {^complete$}
