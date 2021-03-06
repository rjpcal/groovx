##############################################################################
#
# test.tcl
# Rob Peters
# Jul-99
#
##############################################################################

# Multiple inclusion check
if { [info exists TEST_TCL_DEFINED] } { return }
set TEST_TCL_DEFINED 1

set START_MSECS [clock milliseconds]

if { ![info exists ::VERBOSE] } { set ::VERBOSE 0 }
if { ![info exists ::TEST_DIR] } {
    set ::TEST_DIR [file dirname [info script]]
}

set DATE [clock format [clock seconds] -format %H%M%d%b%Y]

set num_tests 0
set num_success 0

set BAD_TESTFILES [list]
set FAILED [list]
set TO_IMPLEMENT [list]
set KNOWN_BUGS [list]
set INTERMITTENT_BUGS [list]

# Flags to pass into 'flags' argument of test.
set no_test 0
set must_implement -1
set skip_known_bug -2
set normal_test 1
set intermittent_bug 2

# If the 'flags' argument is 0, the test is skipped.
proc test {package_name test_name script expected_result_regexp {flags 1}} {
    if { $flags == $::must_implement } {
        lappend ::TO_IMPLEMENT "$package_name $test_name"
    }
    if { $flags == $::skip_known_bug } {
        lappend ::KNOWN_BUGS "$package_name $test_name"
    }
    if { $flags == $::intermittent_bug } {
        lappend ::INTERMITTENT_BUGS "$package_name $test_name"
    }
    if {$flags <= 0} { return }
    if {$::VERBOSE > 0} {
        puts "================================================================"
        puts "==== $package_name $test_name"
    }
    if {$::VERBOSE > 1} {
        puts "==== Contents of test case:"
        puts $script
    }
    set ::errorInfo ""
    catch {eval $script} result
    incr ::num_tests
    if { [regexp -- $expected_result_regexp $result] == 1 } {
        if {$::VERBOSE != 0} { 
            puts "---- $package_name $test_name succeeded\n"
        }
        incr ::num_success
    } else {
        if {$::VERBOSE <= 0} {
            puts "================================================================"
            puts "==== $package_name $test_name"
        }
        if {$::VERBOSE <= 1} {
            puts "==== Contents of test case:"
            puts $script
        }
        puts "==== Result was:"
        puts $result
        puts "---- Result should have been:"
        puts $expected_result_regexp
        if { [string length $::errorInfo] > 0 } {
            puts "---- ErrorInfo:"
            puts $::errorInfo
        }
        puts "---- $package_name $test_name FAILED\n"
#        finish
        lappend ::FAILED "$package_name $test_name"
    }
}

proc finish {} {
    set ::END_MSECS [clock milliseconds]
    set elapsed_ms [expr ($::END_MSECS-$::START_MSECS)]
    set per_test [expr $::num_tests ? ($elapsed_ms/$::num_tests) : 0]
    set msg ""
    if { $::num_success < $::num_tests || [llength $::BAD_TESTFILES] > 0 } {
        append msg "-- WARNING -- BUGGY SOFTWARE --\n"
    }
    append msg "$::num_success tests succeeded of $::num_tests tests attempted.\n"
    append msg "Testing lasted $elapsed_ms msec, or $per_test msec per test.\n"

    if { [llength $::FAILED] > 0 } {
        append msg "[llength $::FAILED] TEST(S) FAILED:\n"
        foreach item $::FAILED {
            append msg "\t${item}\n"
        }
    }

    if { [llength $::TO_IMPLEMENT] > 0 } {
        append msg "[llength $::TO_IMPLEMENT] test(s) remain to be implemented:\n"
        foreach item $::TO_IMPLEMENT {
            append msg "\t${item}\n"
        }
    }

    if { [llength $::KNOWN_BUGS] > 0 } {
        append msg "[llength $::KNOWN_BUGS] known bug(s) reported:\n"
        foreach bug $::KNOWN_BUGS {
            append msg "\t${bug}\n"
        }
    }

    if { [llength $::INTERMITTENT_BUGS] > 0 } {
        append msg "[llength $::INTERMITTENT_BUGS] intermittent bug(s) reported:\n"
        foreach bug $::INTERMITTENT_BUGS {
            append msg "\t${bug}\n"
        }
    }

    if { [llength $::BAD_TESTFILES] > 0 } {
        append msg "[llength $::BAD_TESTFILES] TESTFILE(S) FAILED TO COMPLETE:\n"
        foreach f $::BAD_TESTFILES {
            append msg "\t${f}\n"
        }
    }

    append msg "Testing complete."
    puts $msg
    after 10000 { event generate . <KeyPress> -keysym Return; exit }
    tk_messageBox -icon info -type ok -message $msg
    exit
}

proc testfile {file_arg} {
    puts "running test file ${file_arg}..."

    set result [catch {
	namespace eval :: "source $file_arg"
    }]

    if { $result != 0 } {
	puts "...an error occurred while evaluating ${file_arg}:"
	puts $::errorInfo
	lappend ::BAD_TESTFILES $file_arg
    }
}

# test the test procedure
test test test { expr {2+2} } 4

# helper regexps
set SP {[ \t]+}
set BIT {[01]}
set HEX {[0-9a-f]+}
set INT {-?[0-9]+}
set FLT {-?([0-9]+\.)?[0-9]+}
set BLANK {^$}

