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

    ::test $testname "too few args" {
	IO::writeLGX
    } "wrong \# args: should be "

    ::test $testname "too many args" {
        IO::writeLGX 0 junk
    } "wrong \# args: should be "

    ::test $testname "error from negative id" {
        IO::writeLGX -1
    } {expected.*but got}

    ::test $testname "error from too large id" {
        IO::writeLGX 10000000
    } {attempted to access invalid object.*$}

    ::test $testname "error from non-integral id" {
        IO::writeLGX 1.5
    } {expected.*but got}

    ::test $testname "normal use" [format {
        set code [catch {IO::writeLGX %s} result]
        return "$code $result"
    } $objref] {^0.*$}
}

proc testReadLGX { packagename objref} {
    set testname "${packagename}-IO::readLGX"

    ::test $testname "too few args" {
        IO::readLGX 0
    } "wrong \# args: should be "

    ::test $testname "too many args" {
        IO::readLGX 0 string junk
    } "wrong \# args: should be "

    ::test $testname "error from negative id" {
        IO::readLGX -1 junk
    } {expected.*but got}

    ::test $testname "error from too large id" {
        IO::readLGX 10000000 junk
    } {attempted to access invalid object.*$}

    ::test $testname "error from non-integral id" {
        IO::readLGX 1.5 junk
    } {expected.*but got}

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

    ::test $testname "too few args" {
        IO::writeASW
    } "wrong \# args: should be "

    ::test $testname "too many args" {
        IO::writeASW 0 junk
    } "wrong \# args: should be "

    ::test $testname "error from negative id" {
        IO::writeASW -1
    } {expected.*but got}

    ::test $testname "error from too large id" {
        IO::writeASW 10000000
    } {attempted to access invalid object.*$}

    ::test $testname "error from non-integral id" {
        IO::writeASW 1.5
    } {expected.*but got}

    ::test $testname "normal use" [format {
        set code [catch {IO::writeASW %s} result]
        return "$code $result"
    } $objref] {^0.*$}
}

proc testReadASW { packagename objref} {
    set testname "${packagename}-IO::readASW"

    ::test $testname "too few args" {
        IO::readASW 0
    } "wrong \# args: should be "

    ::test $testname "too many args" {
        IO::readASW 0 string junk
    } "wrong \# args: should be "

    ::test $testname "error from negative id" {
        IO::readASW -1 junk
    } {expected.*but got}

    ::test $testname "error from too large id" {
        IO::readASW 10000000 junk
    } {attempted to access invalid object.*$}

    ::test $testname "error from non-integral id" {
        IO::readASW 1.5 junk
    } {expected.*but got}

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
