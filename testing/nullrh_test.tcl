##############################################################################
###
### NullRhTcl
### Rob Peters
### Oct-2000
### $Id$
###
##############################################################################

source ${::TEST_DIR}/io_test.tcl

set ::NULLRH [IO::new NullResponseHdlr]

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd NullRhTcl IO 1 $::NULLRH
IO::testDestringifyCmd NullRhTcl IO 1 $::NULLRH
IO::testWriteCmd NullRhTcl IO 1 $::NULLRH
IO::testReadCmd NullRhTcl IO 1 $::NULLRH
