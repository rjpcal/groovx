##############################################################################
###
### ThTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

source ${::TEST_DIR}/io_test.tcl

set SIMPLETH [Obj::new TimingHandler]
set TH [Obj::new TimingHdlr]

::testReadWrite SimpleThTcl $::SIMPLETH
::testReadWrite ThTcl $::TH

### Obj::new TimingHandler ###
test "ThTcl-Obj::new SimpleTh" "too many args" {
    Obj::new TimingHandler junk junk
} {^wrong \# args: should be}
test "ThTcl-Obj::new TimingHandler" "normal use" {
	 catch {Obj::new TimingHandler}
} {^0$}
