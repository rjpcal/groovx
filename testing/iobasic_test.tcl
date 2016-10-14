##############################################################################
###
### io_test.tcl
### Rob Peters
### Oct-2004
###
##############################################################################

::test io::write_lgx "too few args" {
    io::write_lgx
} "wrong \# args: should be "

::test io::write_lgx "too many args" {
    io::write_lgx 0 junk
} "wrong \# args: should be "

::test io::write_lgx "error from negative id" {
    io::write_lgx -1
} {expected.*but got}

::test io::write_lgx "error from too large id" {
    io::write_lgx 10000000
} {attempted to access invalid object.*$}

::test io::write_lgx "error from non-integral id" {
    io::write_lgx 1.5
} {expected.*but got}

::test io::read_lgx "too few args" {
    io::read_lgx 0
} "wrong \# args: should be "

::test io::read_lgx "too many args" {
    io::read_lgx 0 string junk
} "wrong \# args: should be "

::test io::read_lgx "error from negative id" {
    io::read_lgx -1 junk
} {expected.*but got}

::test io::read_lgx "error from too large id" {
    io::read_lgx 10000000 junk
} {attempted to access invalid object.*$}

::test io::read_lgx "error from non-integral id" {
    io::read_lgx 1.5 junk
} {expected.*but got}

::test io::write_asw "too few args" {
    io::write_asw
} "wrong \# args: should be "

::test io::write_asw "too many args" {
    io::write_asw 0 junk
} "wrong \# args: should be "

::test io::write_asw "error from negative id" {
    io::write_asw -1
} {expected.*but got}

::test io::write_asw "error from too large id" {
    io::write_asw 10000000
} {attempted to access invalid object.*$}

::test io::write_asw "error from non-integral id" {
    io::write_asw 1.5
} {expected.*but got}

::test io::read_asw "too few args" {
    io::read_asw 0
} "wrong \# args: should be "

::test io::read_asw "too many args" {
    io::read_asw 0 string junk
} "wrong \# args: should be "

::test io::read_asw "error from negative id" {
    io::read_asw -1 junk
} {expected.*but got}

::test io::read_asw "error from too large id" {
    io::read_asw 10000000 junk
} {attempted to access invalid object.*$}

::test io::read_asw "error from non-integral id" {
    io::read_asw 1.5 junk
} {expected.*but got}
