##############################################################################
###
### GxTransform
### Rob Peters
### $Id$
###
##############################################################################

package require Gxtransform
package require Jitter

set ::POS [Obj::new GxTransform]

source ${::TEST_DIR}/io_test.tcl

::testReadWrite GxTransform $::POS

### Obj::new GxTransform ###
test "GxTransform-Obj::new GxTransform" "too many args" {
    Obj::new GxTransform junk junk
} {wrong \# args: should be}
test "GxTransform-Obj::new GxTransform" "normal create" {
	 Obj::new GxTransform
} {^[0-9]+$}

set pos [Obj::new GxTransform]

### GxTransform::rotationAxis ###
test "GxTransform-GxTransform::rotationAxis" "too few args" {
    GxTransform::rotationAxis
} {wrong \# args: should be}
test "GxTransform-GxTransform::rotationAxis" "too many args" {
    GxTransform::rotationAxis posid angle j u n k
} {wrong \# args: should be}
test "GxTransform-GxTransform::rotationAxis" "normal use with angle and axis of rotation" {
	 GxTransform::rotationAxis $::pos {1.5 0.1 -2.2}
	 GxTransform::rotationAxis $::pos
} {^1.5 0.1 -2.2$}
test "GxTransform-GxTransform::rotationAxis" "error on from too few values" {
    GxTransform::rotationAxis $::pos {1,2,3}
} {GxTransform::rotationAxis: }
test "GxTransform-GxTransform::rotationAxis" "error on non-numeric input" {
    GxTransform::rotationAxis $::pos junk
} {GxTransform::rotationAxis: }

### GxTransform::scaling ###
test "GxTransform-GxTransform::scaling" "too few args" {
    GxTransform::scaling
} {wrong \# args: should be}
test "GxTransform-GxTransform::scaling" "too many args" {
    GxTransform::scaling posid j u n k
} {wrong \# args: should be}
test "GxTransform-GxTransform::scaling" "normal use" {
	 GxTransform::scaling $::pos {1.5 3.3 4.5}
	 GxTransform::scaling $::pos
} {^1.5 3.3 4.5$}
test "GxTransform-GxTransform::scaling" "error on from too few values" {
    GxTransform::scaling $::pos 1.0
} {GxTransform::scaling: }
test "GxTransform-GxTransform::scaling" "error on non-numeric input" {
    GxTransform::scaling $::pos {1.0junk}
} {GxTransform::scaling: }

### GxTransform::translation ###
test "GxTransform-GxTransform::translation" "too few args" {
    GxTransform::translation
} {wrong \# args: should be}
test "GxTransform-GxTransform::translation" "too many args" {
    GxTransform::translation posid x y z junk
} {wrong \# args: should be}
test "GxTransform-GxTransform::translation" "normal use" {
	 GxTransform::translation $::pos {10.9 -3.6 9.4}
	 GxTransform::translation $::pos
} {^10.9 -3.6 9.4$}
test "GxTransform-GxTransform::translation" "error on from too few values" {
    GxTransform::translation $::pos {1.0 2.0}
} {GxTransform::translation: }
test "GxTransform-GxTransform::translation" "error on non-numeric input" {
    GxTransform::translation $::pos {1.0 2.0 junk}
} {GxTransform::translation: }

### GxTransform::writeLGX ###
test "GxTransform-GxTransform::writeLGX" "too few args" {
    IO::writeLGX
} {wrong \# args: should be}
test "GxTransform-IO::writeLGX" "too many args" {
    IO::writeLGX posid junk
} {wrong \# args: should be}
test "GxTransform-IO::writeLGX" "normal use" {
	 GxTransform::scaling $::pos {1.0 2.5 4.0}
	 GxTransform::translation $::pos {5.3 10.6 15.9}
	 IO::writeLGX $::pos
} "GxTransform @$::INT"
test "GxTransform-IO::writeLGX" "error on non-integral posid" {
    IO::writeLGX 1.5
} {IO::writeLGX: }

### GxTransform::typeCmd ###
test "GxTransform-Obj::type" "args" {
    Obj::type
} {wrong \# args: should be}
test "GxTransform-Obj::type" "normal use on GxTransform" { 
	 set f [Obj::new GxTransform]
	 Obj::type $f
} {GxTransform}
test "GxTransform-Obj::type" "normal use on Jitter" {
	 set f [Obj::new Jitter]
	 Obj::type $f
} {Jitter}
test "GxTransform-Obj::type" "error on too small posid" {
	 Obj::type -1
} {expected.*but got}

unset pos


