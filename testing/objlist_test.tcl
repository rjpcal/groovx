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
	 
### ObjList::resetCmd ###
test "ObjlistTcl-ObjList::reset" "too many args" {
    ObjList::reset junk
} {wrong \# args: should be "ObjList::reset"}
test "ObjlistTcl-ObjList::reset" "check number of objects -> 0" {
	 Face::face
	 Fixpt::fixpt
	 ObjList::reset
	 ObjList::count
} {^0$}
test "ObjlistTcl-ObjList::reset" "check first vacant -> 0" {
	 Face::face
	 Fixpt::fixpt
	 ObjList::reset
	 Face::face
} {^0$}
test "ObjlistTcl-ObjList::reset" "no error" {} $BLANK $no_test

### ObjList::countCmd ###
test "ObjlistTcl-ObjList::count" "too many args" {
    ObjList::count junk
} {wrong \# args: should be "ObjList::count"}
test "ObjlistTcl-ObjList::count" "normal use" {
	 ObjList::reset
	 Face::face
	 Fixpt::fixpt
	 ObjList::count
} {^2$}
test "ObjlistTcl-ObjList::count" "no error" {} $BLANK $no_test

### ObjList::stringifyCmd ###
test "ObjlistTcl-ObjList::stringify" "too many args" {
    ObjList::stringify junk
} {wrong \# args: should be "ObjList::stringify"}
test "ObjlistTcl-ObjList::stringify" "use on empty list" {
    ObjList::reset
	 ObjList::stringify
} {PtrList<GrObj> 100 0
0}
test "ObjlistTcl-ObjList::stringify" "use on filled list" {
	 ObjList::reset
	 set f [Face::face]
	 set f [Fixpt::fixpt]
	 ObjList::stringify
} "$OBJLIST $INT 2
0 Face .*
1 FixPt .*
2"
test "ObjlistTcl-ObjList::stringify" "no error" {} $BLANK $no_test
	 
### ObjList::destringifyCmd ###
test "ObjlistTcl-ObjList::destringify" "args" {
    ObjList::destringify
} {wrong \# args: should be "ObjList::destringify string"}
test "ObjlistTcl-ObjList::destringify" "use with filled list" {
	 ObjList::reset
	 Face::face
	 Fixpt::fixpt
	 set a [ObjList::stringify]
	 ObjList::destringify $a
	 expr [ObjList::count]==2 \
				&& ([string compare [GrObj::type 0] Face] == 0) \
				&& ([string compare [GrObj::type 1] FixPt] == 0)
} {^1$}
test "ObjlistTcl-ObjList::destringify" "stringify, destringify, restringify" {
	 ObjList::reset
	 Face::face
	 Fixpt::fixpt
	 set a [ObjList::stringify]
	 ObjList::destringify $a
	 set b [ObjList::stringify]
	 expr [string compare $a $b] == 0
} {^1$}
test "ObjlistTcl-ObjList::destringify" "use with empty list" {
	 ObjList::reset
	 set a [ObjList::stringify]
	 ObjList::destringify $a
	 expr [ObjList::count]==0
} {^1$}
test "ObjlistTcl-ObjList::destringify" "error on incomplete input" {
    ObjList::destringify "ObjList"
} {ObjList::destringify: InputError: ObjList}
test "ObjlistTcl-ObjList::destringify" "error on bad input" {
    ObjList::destringify "this is a bunch of bs"
} {ObjList::destringify: InputError: ObjList}


