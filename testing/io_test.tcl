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

proc testWriteLGX { packagename item_id} {
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
    "} {"${cmdname}: signed/unsigned conversion failed.*$"}
    eval ::test $testname {"error from too large id"} {"
        $cmdname 10000000
    "} {"${cmdname}: attempted to access invalid object.*$"}
    eval ::test $testname {"error from non-integral id"} {"
        $cmdname 1.5
    "} {"expected long value but got \"1\.5\""}

    eval ::test $testname {"normal use"} {"
        set code \[catch {$cmdname $item_id} result\]
        return \"\$code \$result\"
    "} {^0.*$}
}

proc testReadLGX { packagename item_id} {
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
    "} {"${cmdname}: signed/unsigned conversion failed.*$"}
    eval ::test $testname {"error from too large id"} {"
        $cmdname 10000000 junk
    "} {"${cmdname}: attempted to access invalid object.*$"}
    eval ::test $testname {"error from non-integral id"} {"
        $cmdname 1.5 junk
    "} {"expected long value but got \"1\.5\""}

    eval ::test $testname {"normal use"} {"
        set str \[$stringify $item_id\]
        set code \[catch {$cmdname $item_id \$str} result\]
        return \"\$code \$result\"
    "} {^0.*$}
    eval ::test $testname {"error on junk"} {"
        set code \[catch {$cmdname $item_id junk} result\]
        return \"\$code \$result\"
    "} {^1.*$}
}

proc testWriteASW { packagename item_id} {
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
    "} {"${cmdname}: signed/unsigned conversion failed.*$"}
    eval ::test $testname {"error from too large id"} {"
        $cmdname 10000000
    "} {"${cmdname}: attempted to access invalid object.*$"}
    eval ::test $testname {"error from non-integral id"} {"
        $cmdname 1.5
    "} {"expected long value but got \"1\.5\""}

    eval ::test $testname {"normal use"} {"
        set code \[catch {$cmdname $item_id} result\]
        return \"\$code \$result\"
    "} {^0.*$}
}

proc testReadASW { packagename item_id} {
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
    "} {"${readcmd}: signed/unsigned conversion failed.*$"}
    eval ::test $testname {"error from too large id"} {"
        $readcmd 10000000 junk
    "} {"${readcmd}: attempted to access invalid object.*$"}
    eval ::test $testname {"error from non-integral id"} {"
        $readcmd 1.5 junk
    "} {"expected long value but got \"1\.5\""}

    eval ::test $testname {"normal use"} {"
        set str \[$writecmd $item_id\]
        set code \[catch {$readcmd $item_id \$str} msg\]
        set result \"\$code\$msg\"
    "} {^0$}
    eval ::test $testname {"error on junk"} {"
        set code \[catch {$readcmd $item_id junk} result\]
        return \"\$code \$result\"
    "} {^1.*$}
}

}
# end namespace eval IO
