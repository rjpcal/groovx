##############################################################################
###
### MiscTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### randCmd ###
test "MiscTcl-rand" "too few args" {
    rand
} {wrong \# args: should be "rand min max"}
test "MiscTcl-rand" "too many args" {
    rand j u n
} {wrong \# args: should be "rand min max"}
test "MiscTcl-rand" "normal use" {
    srand [clock seconds]
    set val [rand 0 1]
    expr $val >= 0 && $val < 1
} {^1$}
test "MiscTcl-rand" "error from non-numeric input" {
    rand junk 0
} {expected.*but got}

### srandCmd ###
test "MiscTcl-srand" "too few args" {
    srand
} {wrong \# args: should be "srand seed"}
test "MiscTcl-srand" "too few args" {
    srand j u
} {wrong \# args: should be "srand seed"}
test "MiscTcl-srand" "normal use" {
    srand 10
    expr [rand 0 1] == 0.138649044093
} {^1$}
test "MiscTcl-srand" "error from non-numeric input" {
    srand junk
} {expected.*but got}
test "MiscTcl-srand" "error from non-integral number" {
    srand 1.5
} {expected.*but got}

### sleepCmd ###
test "MiscTcl-sleep" "too few args" {
    sleep
} {^wrong # args:}
test "MiscTcl-sleep" "too many args" {
    sleep a b
} {^wrong # args:}
test "MiscTcl-sleep" "normal use" {
    set res [time {sleep 1}]
    set us [lindex $res 0]
    expr $us > 900000 && $us < 1500000
} {^1$}
test "MiscTcl-sleep" "error from negative input" {
    sleep -1
} {expected.*but got}

### usleepCmd ###
test "MiscTcl-usleep" "too few args" {
    usleep
} {^wrong # args:}
test "MiscTcl-usleep" "too many args" {
    usleep a b
} {^wrong # args:}
test "MiscTcl-usleep" "normal use" {
    set res [time {usleep 10000} 10]
    set us [lindex $res 0]
    return "[expr $us > 9000 && $us < 25000] $us"
} {^1 }
test "MiscTcl-usleep" "error from negative input" {
    usleep -1
} {expected.*but got}

### usleeprCmd ###
test "MiscTcl-usleepr" "too few args" {
    usleepr
} {^wrong # args:}
test "MiscTcl-usleepr" "too many args" {
    usleepr a b c
} {^wrong # args:}
test "MiscTcl-usleepr" "normal use" {
    set res [time {usleepr 100 10}]
    set us [lindex $res 0]
    return "[expr $us > 0 && $us < 250000] $us"
} {^1 }
test "MiscTcl-usleepr" "error from negative input" {
    usleepr -1 -1
} {expected.*but got}
