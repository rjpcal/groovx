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
} "PtrList<GrObj> $::INT 0
0"
test "ObjlistTcl-ObjList::stringify" "use on filled list" {
	 ObjList::reset
	 set f [Face::Face]
	 set f [Fixpt::Fixpt]
	 ObjList::stringify
} "$OBJLIST $INT 2
0 Face .*
1 FixPt .*
2"
	 
### ObjList::destringifyCmd ###
test "ObjlistTcl-ObjList::destringify" "stringify, destringify, restringify" {
	 ObjList::reset
	 Face::Face
	 Fixpt::Fixpt
	 set a [ObjList::stringify]
	 ObjList::destringify $a
	 set b [ObjList::stringify]
	 expr [string compare $a $b] == 0
} {^1$}
