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

::testReadWrite JitterTcl $::JITTER

unset ::JITTER

### Obj::new Jitter ###
test "Obj::new Jitter" "too many args" {
    new Jitter junk junk
} {wrong \# args: should be}
test "Obj::new Jitter" "normal create" {
    new Jitter
} {[0-9]+}

### Jitter::setJitter ###
test "Jitter::setJitter" "too few args" {
    Jitter::setJitter
} {wrong \# args: should be "Jitter::setJitter posid x_jitter y_jitter r_jitter"}
test "Jitter::setJitter" "too many args" {
    Jitter::setJitter j u n k y
} {wrong \# args: should be "Jitter::setJitter posid x_jitter y_jitter r_jitter"}
test "Jitter::setJitter" "normal use" {
    set jit [new Jitter]
    -> $jit setJitter 1.0 1.0 1.0
} {^$}
test "Jitter::setJitter" "error from non-numeric input" {
    set jit [new Jitter]
    -> $jit setJitter junk 1.0 1.0
} {expected.*but got}
test "Jitter::setJitter" "error from wrong type" {
    set p [new GxTransform]
    Jitter::setJitter $p 1 1 1
} {failed cast}
