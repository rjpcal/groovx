##############################################################################
###
### PositionTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Position
package require Jitter

set ::POS [Obj::new Position]

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd PositionTcl IO 1 $::POS
IO::testDestringifyCmd PositionTcl IO 1 $::POS
IO::testWriteCmd PositionTcl IO 1 $::POS
IO::testReadCmd PositionTcl IO 1 $::POS

### Obj::new Position ###
test "PositionTcl-Obj::new Position" "too many args" {
    Obj::new Position junk junk
} {wrong \# args: should be}
test "PositionTcl-Obj::new Position" "normal create" {
	 Obj::new Position
} {^[0-9]+$}

set pos [Obj::new Position]

### Position::rotationAxis ###
test "PositionTcl-Position::rotationAxis" "too few args" {
    Position::rotationAxis
} {wrong \# args: should be}
test "PositionTcl-Position::rotationAxis" "too many args" {
    Position::rotationAxis posid angle j u n k
} {wrong \# args: should be}
test "PositionTcl-Position::rotationAxis" "normal use with angle and axis of rotation" {
	 Position::rotationAxis $::pos {1.5 0.1 -2.2}
	 Position::rotationAxis $::pos
} {^1.5 0.1 -2.2$}
test "PositionTcl-Position::rotationAxis" "error on from too few values" {
    Position::rotationAxis $::pos {1,2,3}
} {Position::rotationAxis: }
test "PositionTcl-Position::rotationAxis" "error on non-numeric input" {
    Position::rotationAxis $::pos junk
} {Position::rotationAxis: }

### Position::scaling ###
test "PositionTcl-Position::scaling" "too few args" {
    Position::scaling
} {wrong \# args: should be}
test "PositionTcl-Position::scaling" "too many args" {
    Position::scaling posid j u n k
} {wrong \# args: should be}
test "PositionTcl-Position::scaling" "normal use" {
	 Position::scaling $::pos {1.5 3.3 4.5}
	 Position::scaling $::pos
} {^1.5 3.3 4.5$}
test "PositionTcl-Position::scaling" "error on from too few values" {
    Position::scaling $::pos 1.0
} {Position::scaling: }
test "PositionTcl-Position::scaling" "error on non-numeric input" {
    Position::scaling $::pos {1.0junk}
} {Position::scaling: }

### Position::translation ###
test "PositionTcl-Position::translation" "too few args" {
    Position::translation
} {wrong \# args: should be}
test "PositionTcl-Position::translation" "too many args" {
    Position::translation posid x y z junk
} {wrong \# args: should be}
test "PositionTcl-Position::translation" "normal use" {
	 Position::translation $::pos {10.9 -3.6 9.4}
	 Position::translation $::pos
} {^10.9 -3.6 9.4$}
test "PositionTcl-Position::translation" "error on from too few values" {
    Position::translation $::pos {1.0 2.0}
} {Position::translation: }
test "PositionTcl-Position::translation" "error on non-numeric input" {
    Position::translation $::pos {1.0 2.0 junk}
} {Position::translation: }

### Position::stringifyCmd ###
test "PositionTcl-Position::stringify" "too few args" {
    IO::stringify
} {wrong \# args: should be}
test "PositionTcl-IO::stringify" "too many args" {
    IO::stringify posid junk
} {wrong \# args: should be}
test "PositionTcl-IO::stringify" "normal use" {
	 Position::scaling $::pos {1.0 2.5 4.0}
	 Position::translation $::pos {5.3 10.6 15.9}
	 IO::stringify $::pos
} "Position @$::INT"
test "PositionTcl-IO::stringify" "error on non-integral posid" {
    IO::stringify 1.5
} {IO::stringify: }

### Position::typeCmd ###
test "PositionTcl-IO::type" "args" {
    IO::type
} {wrong \# args: should be}
test "PositionTcl-IO::type" "normal use on Position" { 
	 set f [Obj::new Position]
	 IO::type $f
} {Position}
test "PositionTcl-IO::type" "normal use on Jitter" {
	 set f [Obj::new Jitter]
	 IO::type $f
} {Jitter}
test "PositionTcl-IO::type" "error on too small posid" {
	 IO::type -1
} {IO::type: signed/unsigned.*}

unset pos


