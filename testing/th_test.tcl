##############################################################################
###
### ThTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

source ${::TEST_DIR}/list_test.tcl
List::testList ThListTcl ThList Th Th SimpleTh

### SimpleTh::timingHandlerCmd ###
test "ThTcl-SimpleTh::timingHandler" "too many args" {
    SimpleTh::timingHandler junk
} {^wrong \# args: should be "SimpleTh::timingHandler"$}
test "ThTcl-SimpleTh::timingHandler" "normal use" {
	 catch {SimpleTh::timingHandler}
} {^0$}
