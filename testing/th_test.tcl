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
test "ThTcl-SimpleTh::SimpleTh" "too many args" {
    SimpleTh::SimpleTh junk
} {^wrong \# args: should be "SimpleTh::SimpleTh"$}
test "ThTcl-SimpleTh::SimpleTh" "normal use" {
	 catch {SimpleTh::SimpleTh}
} {^0$}
