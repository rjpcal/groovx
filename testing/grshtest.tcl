##############################################################################
#
# grshtest.tcl
# Rob Peters Apr-99
#
# $Id$
#
# This test script which is to be run on grsh. The spirit of this test file is 
# to test every nook and cranny of the C++ code that has been used to create
# various Tcl/Tk extension packages. In practice, this means making sure that
# functions exhibit expected behavior under normal conditions, as well as 
# testing all exits by TCL_ERROR.
#
##############################################################################

### Load test procedures
source /cit/rjpeters/sorcery/grsh/testing/test.tcl

set VERBOSE 0

set RUN_IF_EQUAL 1

### Run test procedures
set files {
    { bitmap_test.tcl 1 }
    { block_test.tcl 1 }
    { blocklist_test.tcl 1 }
    { dlist_test.tcl 1 }
    { expt_test.tcl 1 }
    { eventrh_test.tcl 1 }
    { face_test.tcl 1 }
    { fish_test.tcl 1 }
    { fixpt_test.tcl 1 }
    { gabor_test.tcl 1 }
    { gl_test.tcl 1 }
    { grobj_test.tcl 1 }
    { gtext_test.tcl 1 }
    { gxsep_test.tcl 1 }
    { house_test.tcl 1 }
    { jitter_test.tcl 1 }
    { kbdrh_test.tcl 1 }
    { maskhatch_test.tcl 1 }
    { misc_test.tcl 1 }
    { morphyface_test.tcl 1 }
    { nullrh_test.tcl 1 }
    { objlist_test.tcl 1 }
    { objtogl_test.tcl 1 }
    { position_test.tcl 1 }
    { poslist_test.tcl 1 }
    { rhlist_test.tcl 1 }
    { sound_test.tcl 1 }
    { subject_test.tcl 1 }
    { th_test.tcl 1 }
    { tlist_test.tcl 1 }
    { trial_test.tcl 1 }
}

set others {
}

set seed_arg [lsearch -exact $argv "-seed"]
if { $seed_arg != -1 } {
    set seed [lindex $argv [expr $seed_arg + 1]]
} else {
    set seed [clock clicks]
}

set verbose_arg [lsearch -exact $argv "-verbose"]
if { $verbose_arg != -1 } {
    set ::VERBOSE [lindex $argv [expr $verbose_arg +1]]
}

puts "seed $seed"
expr srand($seed)
proc rand_cmp {a1 a2} { return [expr round(200*rand() - 100.0)] }

set permuted_files [lsort -command rand_cmp $files]

foreach file $permuted_files {
    IoDb::clear
	 if { [lindex $file 1] == $::RUN_IF_EQUAL } {
		  testfile ${TEST_DIR}/[lindex $file 0]
	 }

	 puts "IO::count [IO::countAll]"
}

### Compute test statistics and exit
finish
