##############################################################################
###
### PositionTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Pos
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

### Pos::rotationAxis ###
test "PositionTcl-Pos::rotationAxis" "too few args" {
    Pos::rotationAxis
} {wrong \# args: should be}
test "PositionTcl-Pos::rotationAxis" "too many args" {
    Pos::rotationAxis posid angle j u n k
} {wrong \# args: should be}
test "PositionTcl-Pos::rotationAxis" "normal use with angle and axis of rotation" {
	 catch {Pos::rotationAxis $::pos {1.5 0.0 -2.2}}
} {^0$}
test "PositionTcl-Pos::rotationAxis" "error on non-numeric input" {
    Pos::rotationAxis $::pos junk
} {Pos::rotationAxis: }
test "PositionTcl-Pos::rotationAxis" "error on bad posid" {
    Pos::rotationAxis -1 {1 1 1}
} {Pos::rotationAxis: }

### Pos::scaling ###
test "PositionTcl-Pos::scaling" "too few args" {
    Pos::scaling
} {wrong \# args: should be}
test "PositionTcl-Pos::scaling" "too many args" {
    Pos::scaling posid j u n k
} {wrong \# args: should be}
test "PositionTcl-Pos::scaling" "normal use" {
	 catch {Pos::scaling $::pos {1.5 3.0 4.5}}
} {^0$}
test "PositionTcl-Pos::scaling" "error on non-numeric input" {
    Pos::scaling $::pos junk
} {Pos::scaling: }
test "PositionTcl-Pos::scaling" "error on bad posid" {
    Pos::scaling -1 {-1.2 1.0 1.0}
} {Pos::scaling: .*}

### Pos::translation ###
test "PositionTcl-Pos::translation" "too few args" {
    Pos::translation
} {wrong \# args: should be}
test "PositionTcl-Pos::translation" "too many args" {
    Pos::translation posid x y z junk
} {wrong \# args: should be}
test "PositionTcl-Pos::translation" "normal use" {
	 catch {Pos::translation $::pos {10.0 -3.6 9.4}}
} {^0$}
test "PositionTcl-Pos::translation" "error on non-numeric input" {
    Pos::translation $::pos junk
} {Pos::translation: }
test "PositionTcl-Pos::translation" "error on bad posid" {
    Pos::translation -1 {1 1 1}
} {Pos::translation: .*}

### Pos::stringifyCmd ###
test "PositionTcl-Pos::stringify" "too few args" {
    IO::stringify
} {wrong \# args: should be}
test "PositionTcl-IO::stringify" "too many args" {
    IO::stringify posid junk
} {wrong \# args: should be}
test "PositionTcl-IO::stringify" "normal use" {
	 Pos::scaling $::pos {1.0 2.5 4.0}
	 Pos::translation $::pos {5.3 10.6 15.9}
	 IO::stringify $::pos
} "Position @$::INT"
test "PositionTcl-IO::stringify" "error on bad posid" { 
    IO::stringify -1
} {IO::stringify: }
test "PositionTcl-IO::stringify" "error on non-integral posid" {
    IO::stringify 1.5
} {IO::stringify: }

### Pos::typeCmd ###
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
test "PositionTcl-IO::type" "error on too large" {
	 IO::type 10000
} {IO::type: .*}

unset pos


