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
	 Pos::position
	 Jitter::jitter
	 PosList::reset
	 PosList::count
} {^0$}
test "PoslistTcl-PosList::reset" "check first vacant -> 0" {
	 Pos::position
	 Jitter::jitter
	 PosList::reset
	 Pos::position
} {^0$}
test "PoslistTcl-PosList::reset" "no error" {} $BLANK $no_test

### PosList::countCmd ###
test "PoslistTcl-PosList::count" "args" {
    PosList::count junk
} {wrong \# args: should be "PosList::count"}
test "PoslistTcl-PosList::count" "normal use" { 
	 PosList::reset
	 Pos::position
	 Jitter::jitter
	 PosList::count
} {^2$}
test "PoslistTcl-PosList::count" "no error" {} $BLANK $no_test

### Pos::typeCmd ###
test "PositionTcl-Pos::type" "args" {
    Pos::type
} {wrong \# args: should be "Pos::type posid"}
test "PositionTcl-Pos::type" "normal use on Position" { 
	 set f [Pos::position]
	 Pos::type $f
} {Position}
test "PositionTcl-Pos::type" "normal use on Jitter" {
	 set f [Jitter::jitter]
	 Pos::type $f
} {Jitter}
test "PositionTcl-Pos::type" "error on too small posid" {
	 Pos::type -1
} {Pos::type: posid out of range}
test "PositionTcl-Pos::type" "error on too large" {
	 Pos::type 10000
} {Pos::type: posid out of range}

### PosList::stringifyCmd ###
test "PoslistTcl-PosList::stringify" "args" {
    PosList::stringify junk
} {wrong \# args: should be "PosList::stringify"}
test "PoslistTcl-PosList::stringify" "stringify a non-empty PosList" {
	 PosList::reset
	 set p [Pos::position]
	 Pos::rotate $p 3.0 1.5 0.0 -2.2
	 Pos::scale $p 1.0 2.5 4.0
	 Pos::translate $p 5.3 10.6 15.9
	 set j [Jitter::jitter]
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
	 set p [Pos::position]
	 Pos::rotate $p 3.0 1.5 0.0 -2.2
	 Pos::scale $p 1.0 2.5 4.0
	 Pos::translate $p 5.3 10.6 15.9
	 set j [Jitter::jitter]
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


