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
source /cit/rjpeters/grsh/testing/test.tcl

set VERBOSE 0

### Run test procedures
set files {
	 bitmap_test.tcl
	 block_test.tcl
	 blocklist_test.tcl
	 dlist_test.tcl
	 expt_test.tcl
	 eventrh_test.tcl
	 face_test.tcl
	 fish_test.tcl
	 fixpt_test.tcl
	 gabor_test.tcl
	 gl_test.tcl
	 grobj_test.tcl
	 gtext_test.tcl
	 house_test.tcl
	 jitter_test.tcl
	 kbdrh_test.tcl
	 maskhatch_test.tcl
	 misc_test.tcl
	 morphyface_test.tcl
	 objlist_test.tcl
	 objtogl_test.tcl
	 position_test.tcl
	 poslist_test.tcl
	 rhlist_test.tcl
	 sound_test.tcl
	 subject_test.tcl
	 th_test.tcl
	 tlist_test.tcl
	 trial_test.tcl
}

set others {
}

foreach file $files { testfile ${TEST_DIR}/$file }

### Compute test statistics and exit
finish
