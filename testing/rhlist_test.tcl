##############################################################################
###
### RhListTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

source ${::TEST_DIR}/list_test.tcl
List::testList RhlistTcl RhList Rh KbdRh NullRh

### RhList::stringifyCmd ###
### RhList::destringifyCmd ###
test "RhListTcl-RhList::stringify" "write, read, write and compare" {
	 RhList::reset
	 KbdRh::KbdRh
	 KbdRh::KbdRh
	 set str1 [RhList::stringify]
	 RhList::reset
	 RhList::destringify $str1
	 set str2 [RhList::stringify]
	 string equal $str1 $str2
} {^1$}


