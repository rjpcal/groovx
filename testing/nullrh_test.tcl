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

IO::testWriteLGX NullRhTcl $::NULLRH
IO::testReadLGX NullRhTcl $::NULLRH
IO::testWriteASW NullRhTcl $::NULLRH
IO::testReadASW NullRhTcl $::NULLRH
