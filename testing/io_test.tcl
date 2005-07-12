##############################################################################
###
### io_test.tcl
### Rob Peters
### Oct-1999
### $Id$
###
##############################################################################

# Multiple inclusion guard
if { [info exists ::IO_TEST_DEFINED] } return;

set ::IO_TEST_DEFINED 1

proc testReadWrite { pkg objref } {

    ::test "${pkg}::write_lgx" "normal use" [format {
        set code [catch {io::write_lgx %s} result]
        return "$code $result"
    } $objref] {^0.*$}

    ::test "${pkg}::read_lgx" "normal use" [format {
        set str [io::write_lgx %s]
        set code [catch {io::read_lgx %s $str} result]
        return "$code $result"
    } $objref $objref] {^0.*$}

    ::test "${pkg}::read_lgx" "error on junk" [format {
        set code [catch {io::read_lgx %s junk} result]
        return "$code $result"
    } $objref] {^1.*$}

    ::test "${pkg}::write_asw" "normal use" [format {
        set code [catch {io::write_asw %s} result]
        return "$code $result"
    } $objref] {^0.*$}

    ::test "${pkg}::read_asw" "normal use" [format {
        set str [io::write_asw %s]
        set code [catch {io::read_asw %s $str} msg]
        set result "$code$msg"
    } $objref $objref] {^0$}

    ::test "${pkg}::read_asw" "error on junk" [format {
        set code [catch {io::read_asw %s junk} result]
        return "$code $result"
    } $objref] {^1.*$}
}
