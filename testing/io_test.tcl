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
    set cmdname "IO::writeLGX"
    set testname "${packagename}-${cmdname}"

    set usage "wrong \# args: should be "

    eval ::test $testname {"too few args"} {"
        $cmdname
    "} {$usage}
    eval ::test $testname {"too many args"} {"
        $cmdname 0 junk
    "} {$usage}
    eval ::test $testname {"error from negative id"} {"
        $cmdname -1
    "} {"expected.*but got"}
    eval ::test $testname {"error from too large id"} {"
        $cmdname 10000000
    "} {"attempted to access invalid object.*$"}
    eval ::test $testname {"error from non-integral id"} {"
        $cmdname 1.5
    "} {"expected.*but got"}

    eval ::test $testname {"normal use"} {"
        set code \[catch {$cmdname $objref} result\]
        return \"\$code \$result\"
    "} {^0.*$}
}

proc testReadLGX { packagename objref} {
    set cmdname "IO::readLGX"
    set stringify "IO::writeLGX"
    set testname "${packagename}-${cmdname}"

    set usage "wrong \# args: should be "

    eval ::test $testname {"too few args"} {"
        $cmdname 0
    "} {$usage}
    eval ::test $testname {"too many args"} {"
        $cmdname 0 string junk
    "} {$usage}
    eval ::test $testname {"error from negative id"} {"
        $cmdname -1 junk
    "} {"expected.*but got"}
    eval ::test $testname {"error from too large id"} {"
        $cmdname 10000000 junk
    "} {"attempted to access invalid object.*$"}
    eval ::test $testname {"error from non-integral id"} {"
        $cmdname 1.5 junk
    "} {"expected.*but got"}

    eval ::test $testname {"normal use"} {"
        set str \[$stringify $objref\]
        set code \[catch {$cmdname $objref \$str} result\]
        return \"\$code \$result\"
    "} {^0.*$}
    eval ::test $testname {"error on junk"} {"
        set code \[catch {$cmdname $objref junk} result\]
        return \"\$code \$result\"
    "} {^1.*$}
}

proc testWriteASW { packagename objref} {
    set cmdname "IO::writeASW"
    set testname "${packagename}-${cmdname}"

    set usage "wrong \# args: should be "

    eval ::test $testname {"too few args"} {"
        $cmdname
    "} {$usage}
    eval ::test $testname {"too many args"} {"
        $cmdname 0 junk
    "} {$usage}
    eval ::test $testname {"error from negative id"} {"
        $cmdname -1
    "} {"expected.*but got"}
    eval ::test $testname {"error from too large id"} {"
        $cmdname 10000000
    "} {"attempted to access invalid object.*$"}
    eval ::test $testname {"error from non-integral id"} {"
        $cmdname 1.5
    "} {"expected.*but got"}

    eval ::test $testname {"normal use"} {"
        set code \[catch {$cmdname $objref} result\]
        return \"\$code \$result\"
    "} {^0.*$}
}

proc testReadASW { packagename objref} {
    set readcmd "IO::readASW"
    set writecmd "IO::writeASW"
    set testname "${packagename}-${readcmd}"

    set usage "wrong \# args: should be "

    eval ::test $testname {"too few args"} {"
        $readcmd 0
    "} {$usage}
    eval ::test $testname {"too many args"} {"
        $readcmd 0 string junk
    "} {$usage}
    eval ::test $testname {"error from negative id"} {"
        $readcmd -1 junk
    "} {"expected.*but got"}
    eval ::test $testname {"error from too large id"} {"
        $readcmd 10000000 junk
    "} {"attempted to access invalid object.*$"}
    eval ::test $testname {"error from non-integral id"} {"
        $readcmd 1.5 junk
    "} {"expected.*but got"}

    eval ::test $testname {"normal use"} {"
        set str \[$writecmd $objref\]
        set code \[catch {$readcmd $objref \$str} msg\]
        set result \"\$code\$msg\"
    "} {^0$}
    eval ::test $testname {"error on junk"} {"
        set code \[catch {$readcmd $objref junk} result\]
        return \"\$code \$result\"
    "} {^1.*$}
}

}
# end namespace eval IO
