##############################################################################
###
### io_test.tcl
### Rob Peters
### Oct-1999
### $Id$
###
##############################################################################

# Multiple inclusion guard
if { [info exists IO::TEST_DEFINED] } return;


namespace eval IO {

variable TEST_DEFINED 1

proc testWriteLGX { packagename objref} {
    set testname "${packagename}-IO::writeLGX"

    ::test $testname "normal use" [format {
        set code [catch {IO::writeLGX %s} result]
        return "$code $result"
    } $objref] {^0.*$}
}

proc testReadLGX { packagename objref} {
    set testname "${packagename}-IO::readLGX"

    ::test $testname "normal use" [format {
        set str [IO::writeLGX %s]
        set code [catch {IO::readLGX %s $str} result]
        return "$code $result"
    } $objref $objref] {^0.*$}

    ::test $testname "error on junk" [format {
        set code [catch {IO::readLGX %s junk} result]
        return "$code $result"
    } $objref] {^1.*$}
}

proc testWriteASW {packagename objref} {
    set testname "${packagename}-IO::writeASW"

    ::test $testname "normal use" [format {
        set code [catch {IO::writeASW %s} result]
        return "$code $result"
    } $objref] {^0.*$}
}

proc testReadASW { packagename objref} {
    set testname "${packagename}-IO::readASW"

    ::test $testname "normal use" [format {
        set str [IO::writeASW %s]
        set code [catch {IO::readASW %s $str} msg]
        set result "$code$msg"
    } $objref $objref] {^0$}

    ::test $testname "error on junk" [format {
        set code [catch {IO::readASW %s junk} result]
        return "$code $result"
    } $objref] {^1.*$}
}

}
# end namespace eval IO
