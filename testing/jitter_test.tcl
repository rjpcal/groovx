##############################################################################
###
### JitterTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

source ${::TEST_DIR}/io_test.tcl

set ::JITTER [Jitter::Jitter]

IO::testStringifyCmd JitterTcl Jitter 1 $::JITTER
IO::testDestringifyCmd JitterTcl Jitter 1 $::JITTER
IO::testWriteCmd JitterTcl Jitter 1 $::JITTER
IO::testReadCmd JitterTcl Jitter 1 $::JITTER

unset ::JITTER

### Jitter::JitterCmd ###
test "JitterTcl-Jitter::Jitter" "too many args" {
	 Jitter::Jitter junk
} {wrong \# args: should be "Jitter::Jitter"}
test "JitterTcl-Jitter::Jitter" "normal create" {
	 Jitter::Jitter
} {[0-9]+}
test "JitterTcl-Jitter::Jitter" "no error" {} $BLANK $no_test

set jit [Jitter::Jitter]

### Jitter::setJitterCmd ###
test "JitterTcl-Jitter::setJitter" "too few args" {
    Jitter::setJitter
} {wrong \# args: should be "Jitter::setJitter posid x_jitter y_jitter r_jitter"}
test "JitterTcl-Jitter::setJitter" "too many args" {
    Jitter::setJitter j u n k y
} {wrong \# args: should be "Jitter::setJitter posid x_jitter y_jitter r_jitter"}
test "JitterTcl-Jitter::setJitter" "normal use" {
	 catch {Jitter::setJitter $::jit 1.0 1.0 1.0}
} {^0$}
test "JitterTcl-Jitter::setJitter" "error from non-numeric input" {
    Jitter::setJitter $::jit junk 1.0 1.0
} {expected floating-point number but got "junk"}
test "JitterTcl-Jitter::setJitter" "error from wrong type" {
    set p [Pos::Pos]
	 Jitter::setJitter $p 1 1 1
} {^Jitter::setJitter: an error of type (std::)?bad_cast occurred:.*$}

PosList::reset
unset jit


