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

IO::testWriteLGX SimpleThTcl $::SIMPLETH
IO::testReadLGX SimpleThTcl $::SIMPLETH
IO::testWriteASW SimpleThTcl $::SIMPLETH
IO::testReadASW SimpleThTcl $::SIMPLETH

set TH [Obj::new TimingHdlr]

IO::testWriteLGX ThTcl $::TH
IO::testReadLGX ThTcl $::TH
IO::testWriteASW ThTcl $::TH
IO::testReadASW ThTcl $::TH

### Obj::new TimingHandler ###
test "ThTcl-Obj::new SimpleTh" "too many args" {
    Obj::new TimingHandler junk junk
} {^wrong \# args: should be}
test "ThTcl-Obj::new TimingHandler" "normal use" {
	 catch {Obj::new TimingHandler}
} {^0$}
