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

    ::test "${pkg}::writeLGX" "normal use" [format {
        set code [catch {IO::writeLGX %s} result]
        return "$code $result"
    } $objref] {^0.*$}

    ::test "${pkg}::readLGX" "normal use" [format {
        set str [IO::writeLGX %s]
        set code [catch {IO::readLGX %s $str} result]
        return "$code $result"
    } $objref $objref] {^0.*$}

    ::test "${pkg}::readLGX" "error on junk" [format {
        set code [catch {IO::readLGX %s junk} result]
        return "$code $result"
    } $objref] {^1.*$}

    ::test "${pkg}::writeASW" "normal use" [format {
        set code [catch {IO::writeASW %s} result]
        return "$code $result"
    } $objref] {^0.*$}

    ::test "${pkg}::readASW" "normal use" [format {
        set str [IO::writeASW %s]
        set code [catch {IO::readASW %s $str} msg]
        set result "$code$msg"
    } $objref $objref] {^0$}

    ::test "${pkg}::readASW" "error on junk" [format {
        set code [catch {IO::readASW %s junk} result]
        return "$code $result"
    } $objref] {^1.*$}
}
