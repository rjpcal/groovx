##############################################################################
###
### ObjlistTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### regexp to match ObjList in serializations
set OBJLIST {ObjList|PtrList<GrObj>}
	 
source ${::TEST_DIR}/list_test.tcl
List::testList ObjlistTcl ObjList GrObj Face Fish

### ObjList::stringifyCmd ###
test "ObjlistTcl-ObjList::stringify" "use on empty list" {
    ObjList::reset
	 ObjList::stringify
} "(ObjList|PtrList<GrObj>) $::INT $::INT"
test "ObjlistTcl-ObjList::stringify" "use on filled list" {
	 ObjList::reset
	 set f [Face::Face]
	 set f [Fixpt::Fixpt]
	 ObjList::stringify
} "$OBJLIST $INT 2
0 Face .*
1 FixPt .*
2"

