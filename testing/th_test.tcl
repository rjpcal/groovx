##############################################################################
###
### ThTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

source ${::TEST_DIR}/list_test.tcl
List::testList ThListTcl ThList Th TimingHdlr TimingHandler

set SIMPLETH [IO::new TimingHandler]

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd SimpleThTcl IO 1 $::SIMPLETH
IO::testDestringifyCmd SimpleThTcl IO 1 $::SIMPLETH
IO::testWriteCmd SimpleThTcl IO 1 $::SIMPLETH
IO::testReadCmd SimpleThTcl IO 1 $::SIMPLETH

set TH [IO::new TimingHdlr]

IO::testStringifyCmd ThTcl IO 1 $::TH
IO::testDestringifyCmd ThTcl IO 1 $::TH
IO::testWriteCmd ThTcl IO 1 $::TH
IO::testReadCmd ThTcl IO 1 $::TH

### IO::new TimingHandler ###
test "ThTcl-IO::new SimpleTh" "too many args" {
    IO::new TimingHandler junk junk
} {^wrong \# args: should be "IO::new typename \?array_size=1\?"$}
test "ThTcl-IO::new TimingHandler" "normal use" {
	 catch {IO::new TimingHandler}
} {^0$}
