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
package require Poslist

set ::POS [Pos::Pos]

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd PositionTcl Pos 1 $::POS
IO::testDestringifyCmd PositionTcl Pos 1 $::POS
IO::testWriteCmd PositionTcl Pos 1 $::POS
IO::testReadCmd PositionTcl Pos 1 $::POS

### Pos::PosCmd ###
test "PositionTcl-Pos::Pos" "too many args" {
    Pos::Pos junk
} {wrong \# args: should be "Pos::Pos"}
test "PositionTcl-Pos::Pos" "normal create" {
	 Pos::Pos
} {^[0-9]+$}
test "PositionTcl-Pos::Pos" "no error" {} $BLANK $no_test

set pos [Pos::Pos]

### Pos::rotateCmd ###
test "PositionTcl-Pos::rotate" "too few args" {
    Pos::rotate
} {wrong \# args: should be "Pos::rotate posid angle \?rot_axis_x rot_axis_y rot_axis_z\?"}
test "PositionTcl-Pos::rotate" "illegal arg count" {
    Pos::rotate posid angle junk
} {wrong \# args: should be "Pos::rotate posid angle \?rot_axis_x rot_axis_y rot_axis_z\?"}
test "PositionTcl-Pos::rotate" "too many" {
    Pos::rotate posid angle j u n k
} {wrong \# args: should be "Pos::rotate posid angle \?rot_axis_x rot_axis_y rot_axis_z\?"}
test "PositionTcl-Pos::rotate" "normal use with angle only" {
	 catch {Pos::rotate $::pos 3.0}
} {^0$}
test "PositionTcl-Pos::rotate" "normal use with angle and axis of rotation" {
	 catch {Pos::rotate $::pos 3.0 1.5 0.0 -2.2}
} {^0$}
test "PositionTcl-Pos::rotate" "error on non-numeric input" {
    Pos::rotate $::pos junk
} {expected floating-point number but got "junk"}
test "PositionTcl-Pos::rotate" "error on bad posid" {
    Pos::rotate -1 1 1 1 1
} {Pos::rotate: attempt to access invalid id '.*' in}

### scaleCmd ###
test "PositionTcl-Pos::scale" "too few args" {
    Pos::scale posid
} {wrong \# args: should be "Pos::scale posid scale_x \?scale_y\? \?scale_z\?"}
test "PositionTcl-Pos::scale" "too many args" {
    Pos::scale posid j u n k
} {wrong \# args: should be "Pos::scale posid scale_x \?scale_y\? \?scale_z\?"}
test "PositionTcl-Pos::scale" "normal use" {
	 catch {Pos::scale $::pos 1.5 3.0 4.5}
} {^0$}
test "PositionTcl-Pos::scale" "error on non-numeric input" {
    Pos::scale $::pos junk
} {expected floating-point number but got "junk"}
test "PositionTcl-Pos::scale" "error on bad posid" {
    Pos::scale -1 -1.2 1.0 1.0
} {Pos::scale: attempt to access invalid id '.*' in}

### Pos::translateCmd ###
test "PositionTcl-Pos::translate" "too few args" {
    Pos::translate posid x
} {wrong \# args: should be "Pos::translate posid new_x new_y \?new_z\?"}
test "PositionTcl-Pos::translate" "too many args" {
    Pos::translate posid x y z junk
} {wrong \# args: should be "Pos::translate posid new_x new_y \?new_z\?"}
test "PositionTcl-Pos::translate" "normal use" {
	 catch {Pos::translate $::pos 10.0 -3.6 9.4}
} {^0$}
test "PositionTcl-Pos::translate" "error on non-numeric input" {
    Pos::translate $::pos junk junk junk
} {expected floating-point number but got "junk"}
test "PositionTcl-Pos::translate" "error on bad posid" {
    Pos::translate -1 1 1 1
} {Pos::translate: attempt to access invalid id '.*' in}

### Pos::stringifyCmd ###
test "PositionTcl-Pos::stringify" "too few args" {
    Pos::stringify
} {wrong \# args: should be "Pos::stringify item_id"}
test "PositionTcl-Pos::stringify" "too many args" {
    Pos::stringify posid junk
} {wrong \# args: should be "Pos::stringify item_id"}
test "PositionTcl-Pos::stringify" "normal use" {
	 Pos::rotate $::pos 3.0 1.5 0.0 -2.2
	 Pos::scale $::pos 1.0 2.5 4.0
	 Pos::translate $::pos 5.3 10.6 15.9
	 Pos::stringify $::pos
} "Position @$::INT { 5\.3 10\.6 15\.9 1 2\.5 4 1\.5 0 -2\.2 3"
test "PositionTcl-Pos::stringify" "error on bad posid" { 
    Pos::stringify -1
} {Pos::stringify: attempt to access invalid id '.*' in}
test "PositionTcl-Pos::stringify" "error on non-integral posid" {
    Pos::stringify 1.5
} {expected integer but got "1\.5"}
	 
### Pos::typeCmd ###
test "PositionTcl-Pos::type" "args" {
    Pos::type
} {wrong \# args: should be "Pos::type item_id"}
test "PositionTcl-Pos::type" "normal use on Position" { 
	 set f [Pos::Pos]
	 Pos::type $f
} {Position}
test "PositionTcl-Pos::type" "normal use on Jitter" {
	 set f [Jitter::Jitter]
	 Pos::type $f
} {Jitter}
test "PositionTcl-Pos::type" "error on too small posid" {
	 Pos::type -1
} {Pos::type: attempt to access invalid id '.*' in}
test "PositionTcl-Pos::type" "error on too large" {
	 Pos::type 10000
} {Pos::type: attempt to access invalid id '.*' in}

PosList::reset
unset pos


