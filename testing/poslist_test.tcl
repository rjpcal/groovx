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

### PosList::resetCmd ###
test "PoslistTcl-PosList::reset" "too many args" {
    PosList::reset junk
} {wrong \# args: should be "PosList::reset"}
test "PoslistTcl-PosList::reset" "check number of positions -> 0" {
	 Pos::Pos
	 Jitter::Jitter
	 PosList::reset
	 PosList::count
} {^0$}
test "PoslistTcl-PosList::reset" "check first vacant -> 0" {
	 Pos::Pos
	 Jitter::Jitter
	 PosList::reset
	 Pos::Pos
} {^0$}
test "PoslistTcl-PosList::reset" "no error" {} $BLANK $no_test

### PosList::countCmd ###
test "PoslistTcl-PosList::count" "args" {
    PosList::count junk
} {wrong \# args: should be "PosList::count"}
test "PoslistTcl-PosList::count" "normal use" { 
	 PosList::reset
	 Pos::Pos
	 Jitter::Jitter
	 PosList::count
} {^2$}
test "PoslistTcl-PosList::count" "no error" {} $BLANK $no_test

### PosList::stringifyCmd ###
test "PoslistTcl-PosList::stringify" "args" {
    PosList::stringify junk
} {wrong \# args: should be "PosList::stringify"}
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

test "PoslistTcl-PosList::stringify" "stringify an empty PosList" {
	 PosList::reset
	 PosList::stringify
} "$POSLIST $INT 0
0"
test "PoslistTcl-PosList::stringify" "no error" {} $BLANK $no_test

### PosList::destringifyCmd ###
test "PoslistTcl-PosList::destringify" "args" {
    PosList::destringify
} {wrong \# args: should be "PosList::destringify string"}
test "PoslistTcl-PosList::destringify" "normal use" {
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
	 set str [PosList::stringify]
	 PosList::reset
	 PosList::destringify $str
	 expr [PosList::count]==2 \
				&& ([string compare [Pos::type 0] Position] == 0) \
				&& ([string compare [Pos::type 1] Jitter] == 0)
} {^1$}
test "PoslistTcl-PosList::destringify" "error" {
    PosList::destringify "this is all just a bunch of bunk"
} {PosList::destringify: InputError: PosList}


