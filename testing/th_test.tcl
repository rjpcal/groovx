##############################################################################
###
### ThTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

set SIMPLETH [Obj::new TimingHandler]

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd SimpleThTcl IO 1 $::SIMPLETH
IO::testDestringifyCmd SimpleThTcl IO 1 $::SIMPLETH
IO::testWriteCmd SimpleThTcl IO 1 $::SIMPLETH
IO::testReadCmd SimpleThTcl IO 1 $::SIMPLETH

set TH [Obj::new TimingHdlr]

IO::testStringifyCmd ThTcl IO 1 $::TH
IO::testDestringifyCmd ThTcl IO 1 $::TH
IO::testWriteCmd ThTcl IO 1 $::TH
IO::testReadCmd ThTcl IO 1 $::TH

### Obj::new TimingHandler ###
test "ThTcl-Obj::new SimpleTh" "too many args" {
    Obj::new TimingHandler junk junk
} {^wrong \# args: should be}
test "ThTcl-Obj::new TimingHandler" "normal use" {
	 catch {Obj::new TimingHandler}
} {^0$}
