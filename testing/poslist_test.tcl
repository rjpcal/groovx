##############################################################################
###
### PoslistTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### regexp to match PosList in serializations
set POSLIST {PosList|PtrList<Position>}

source ${::TEST_DIR}/list_test.tcl
List::testList PoslistTcl PosList Pos Pos Jitter

### PosList::stringifyCmd ###
test "PoslistTcl-PosList::stringify" "stringify a non-empty PosList" {
	 PosList::reset
	 set p [Pos::Pos]
	 Pos::rotate $p 3.0 1.5 0.0 -2.2
	 Pos::scale $p 1.0 2.5 4.0
	 Pos::translate $p 5.3 10.6 15.9
	 set j [Jitter::Jitter]
	 Pos::rotate $j 3.0 1.5 0.0 -2.2
	 Pos::scale $j 1.0 2.5 4.0
	 Pos::translate $j 5.3 10.6 15.9
	 Jitter::setJitter $j 1.5 3.0 4.5
	 PosList::stringify
} "$POSLIST $INT 2
0 Position 5\.3 10\.6 15\.9 1 2\.5 4 1\.5 0 -2\.2 3
1 Jitter 1\.5 3 4\.5
Position 5\.3 10\.6 15\.9 1 2\.5 4 1\.5 0 -2\.2 3
2"
