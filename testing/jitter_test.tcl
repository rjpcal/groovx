##############################################################################
###
### JitterTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

source ${::TEST_DIR}/io_test.tcl

set ::JITTER [Obj::new Jitter]

IO::testStringifyCmd JitterTcl IO 1 $::JITTER
IO::testDestringifyCmd JitterTcl IO 1 $::JITTER
IO::testWriteCmd JitterTcl IO 1 $::JITTER
IO::testReadCmd JitterTcl IO 1 $::JITTER

unset ::JITTER

### Obj::new Jitter ###
test "JitterTcl-Obj::new Jitter" "too many args" {
	 Obj::new Jitter junk junk
} {wrong \# args: should be}
test "JitterTcl-Obj::new Jitter" "normal create" {
	 Obj::new Jitter
} {[0-9]+}

set jit [Obj::new Jitter]

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
    set p [Obj::new GxTransform]
	 Jitter::setJitter $p 1 1 1
} {^Jitter::setJitter: an error of type (std::)?bad_cast occurred:.*$}

unset jit


