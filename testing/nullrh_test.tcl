##############################################################################
###
### NullRhTcl
### Rob Peters
### Oct-2000
### $Id$
###
##############################################################################

source ${::TEST_DIR}/io_test.tcl

RhList::reset
set ::NULLRH [NullRh::NullRh]
NullRh::keyRespPairs $::NULLRH { {{^[aA]$} 0} {{^[lL]$} 1} }

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd NullRhTcl NullRh 1 $::NULLRH
IO::testDestringifyCmd NullRhTcl NullRh 1 $::NULLRH
IO::testWriteCmd NullRhTcl NullRh 1 $::NULLRH
IO::testReadCmd NullRhTcl NullRh 1 $::NULLRH
