##############################################################################
###
### NullRhTcl
### Rob Peters
### Oct-2000
### $Id$
###
##############################################################################

source ${::TEST_DIR}/io_test.tcl

set ::NULLRH [Obj::new NullResponseHdlr]

source ${::TEST_DIR}/io_test.tcl

::testReadWrite NullRhTcl $::NULLRH
