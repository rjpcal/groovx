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

IO::testStringifyCmd SimpleThTcl SimpleTh 1 $::SIMPLETH
IO::testDestringifyCmd SimpleThTcl SimpleTh 1 $::SIMPLETH
IO::testWriteCmd SimpleThTcl SimpleTh 1 $::SIMPLETH
IO::testReadCmd SimpleThTcl SimpleTh 1 $::SIMPLETH

set TH [IO::new TimingHdlr]

IO::testStringifyCmd ThTcl Th 1 $::TH
IO::testDestringifyCmd ThTcl Th 1 $::TH
IO::testWriteCmd ThTcl Th 1 $::TH
IO::testReadCmd ThTcl Th 1 $::TH

### IO::new TimingHandler ###
test "ThTcl-IO::new SimpleTh" "too many args" {
    IO::new TimingHandler junk
} {^wrong \# args: should be "IO::new typename"$}
test "ThTcl-IO::new TimingHandler" "normal use" {
	 catch {IO::new TimingHandler}
} {^0$}
