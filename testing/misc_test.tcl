##############################################################################
###
### MiscTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### randCmd ###
test "::rand" "too few args" {
    rand
} {wrong \# args}
test "::rand" "too many args" {
    rand j u n k
} {wrong \# args}
test "::rand" "normal use" {
    srand [clock seconds]
    set val [rand 0 1]
    expr $val >= 0 && $val < 1
} {^1$}
test "::rand" "normal use with repetition" {
    srand [clock seconds]
    set n [expr int([rand 1 10])]
    set result [rand 0 1 $n]
    return "[expr [llength $result] == $n] [llength $result] $n"
} {^1}
test "::rand" "error from non-numeric input" {
    rand junk 0
} {expected.*but got}

### srandCmd ###
test "::srand" "too few args" {
    srand
} {wrong \# args: should be "srand seed"}
test "::srand" "too few args" {
    srand j u
} {wrong \# args: should be "srand seed"}
test "::srand" "normal use" {
    srand 10
    expr [rand 0 1] == 0.138649044093
} {^1$}
test "::srand" "error from non-numeric input" {
    srand junk
} {expected.*but got}
test "::srand" "error from non-integral number" {
    srand 1.5
} {expected.*but got}

### sleepCmd ###
test "::sleep" "too few args" {
    sleep
} {^wrong # args:}
test "::sleep" "too many args" {
    sleep a b
} {^wrong # args:}
test "::sleep" "normal use" {
    set res [time {sleep 1}]
    set us [lindex $res 0]
    expr $us > 900000 && $us < 1500000
} {^1$} $no_test
test "::sleep" "error from negative input" {
    sleep -1
} {expected.*but got}

### usleepCmd ###
test "::usleep" "too few args" {
    usleep
} {^wrong # args:}
test "::usleep" "too many args" {
    usleep a b
} {^wrong # args:}
test "::usleep" "normal use" {
    set res [time {usleep 10000} 10]
    set us [lindex $res 0]
    return "[expr $us > 9000 && $us < 25000] $us"
} {^1 }
test "::usleep" "error from negative input" {
    usleep -1
} {expected.*but got}

### usleeprCmd ###
test "::usleepr" "too few args" {
    usleepr
} {^wrong # args:}
test "::usleepr" "too many args" {
    usleepr a b c
} {^wrong # args:}
test "::usleepr" "normal use" {
    set res [time {usleepr 100 10}]
    set us [lindex $res 0]
    return "[expr $us > 0 && $us < 250000] $us"
} {^1 }
test "::usleepr" "error from negative input" {
    usleepr -1 -1
} {expected.*but got}
