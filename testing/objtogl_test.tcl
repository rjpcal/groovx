##############################################################################
###
### ObjTogl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Objtogl
package require Togl
package require Face
package require Pos
package require Tlist

set PACKAGE ObjTogl

### ObjTogl::InitCmd ###
test "ObjTogl-Togl::Init" "test widget create" {
	 if { ![Togl::inited] } { Togl::init "-rgba false"; update }
} {^$}

### Togl::undrawCmd ###
test "ObjTogl-Togl::undraw" "too many args" {
	 Togl::undraw junk
} {wrong \# args: should be "Togl::undraw"}
test "ObjTogl-Togl::undraw" "normal use" {
	 set f [Face::Face]
	 set p [Pos::Pos]
	 set t [Trial::Trial]
	 Trial::add $t $f $p
	 setForeground 1
	 setBackground 0
	 clearscreen
	 Togl::show 0
	 Togl::undraw
	 pixelCheckSum
} {^0$}
test "ObjTogl-Togl::undraw" "no error" {} $BLANK $no_test

### Togl::refreshCmd ###
test "ObjTogl-Togl::refresh" "too many args" {
	 Togl::refresh junk
} {wrong \# args: should be "Togl::refresh"}
test "ObjTogl-Togl::refresh" "normal use" {
	 set f [Face::Face]
	 set p [Pos::Pos]
	 set t [Trial::Trial]
	 Trial::add $t $f $p
	 Togl::setCurTrial 0
	 setForeground 1
	 setBackground 0
	 Togl::setVisible 1
	 Togl::refresh
	 # check to see if some pixels actually got drawn
	 expr [pixelCheckSum] > 500
} {^1$}
test "ObjTogl-Togl::refresh" "no error" {} $BLANK $no_test

### Togl::clearscreenCmd ###
test "ObjTogl-Togl::clearscreen" "too many args" {
    Togl::clearscreen junk
} {wrong \# args: should be "Togl::clearscreen"}
test "ObjTogl-Togl::clearscreen" "normal use" {
	 setBackground 0
	 Togl::clearscreen
	 pixelCheckSum
} {^0$}
test "ObjTogl-Togl::clearscreen" "no error" {} $BLANK $no_test

### Togl::showCmd ###
test "ObjTogl-Togl::show" "too few args" {
	 Togl::show
} {wrong \# args: should be "Togl::show trial_id"}
test "ObjTogl-Togl::show" "too many args" {
	 Togl::show j u
} {wrong \# args: should be "Togl::show trial_id"}
test "ObjTogl-Togl::show" "normal use on invalid trial id" { 
	 set code [catch {Togl::show -1} msg]
	 set result "$code $msg"
} {^1 Togl::show: attempt to access invalid id.*in Tlist$}
test "ObjTogl-Togl::show" "no error" {} $BLANK $no_test

### Togl::setVisibleCmd ###
test "ObjTogl-Togl::setVisible" "too few args" {
    Togl::setVisible
} {wrong \# args: should be "Togl::setVisible visibility"}
test "ObjTogl-Togl::setVisible" "too many args" {
    Togl::setVisible j u
} {wrong \# args: should be "Togl::setVisible visibility"}
test "ObjTogl-Togl::setVisible" "normal set off" {
	 catch {Togl::setVisible 0}
} {^0$}
test "ObjTogl-Togl::setVisible" "normal set on" {
	 catch {Togl::setVisible 1}
} {^0$}
test "ObjTogl-Togl::setVisible" "unusual set on" {
	 catch {Togl::setVisible -1}
} {^0$}
test "ObjTogl-Togl::setVisible" "error on non-numeric input" {
    Togl::setVisible junk
} {expected boolean value but got "junk"}

### Togl::setCurTrialCmd ###
test "ObjTogl-Togl::setCurTrial" "too few args" {
    Togl::setCurTrial
} {wrong \# args: should be "Togl::setCurTrial trial_id"}
test "ObjTogl-Togl::setCurTrial" "too many args" {
    Togl::setCurTrial j u
} {wrong \# args: should be "Togl::setCurTrial trial_id"}
test "ObjTogl-Togl::setCurTrial" "normal use" {
	 set f [Face::Face]
	 set p [Pos::Pos]
	 set t [Trial::Trial]
	 Trial::add $t $f $p
	 catch {Togl::setCurTrial 0}
} {^0$}
test "ObjTogl-Togl::setCurTrial" "error on too low trial id" {
    Togl::setCurTrial -1
} {Togl::setCurTrial: attempt to access invalid id.*in Tlist}
test "ObjTogl-Togl::setCurTrial" "error on too large trial id" {
	 Tlist::reset
    Togl::setCurTrial 10000
} {Togl::setCurTrial: attempt to access invalid id.*in Tlist}

### Togl::loadFontCmd ###
test "$PACKAGE-Togl::loadFont" "too many args" {
	 Togl::loadFont fixed junk
} {^wrong \# args: should be "Togl::loadFont \?fontname\?"$}
test "$PACKAGE-Togl::loadFont" "normal use" {
	 set code [catch {Togl::loadFont} msg]
	 set res "$code $msg"
} {^0 $}
test "$PACKAGE-Togl::loadFont" "error" {
	 Togl::loadFont junk
} {^Togl::loadFont: unable to load font junk$}

### Togl::loadFontiCmd ###
test "$PACKAGE-Togl::loadFonti" "too many args" {
	 Togl::loadFonti 3 junk
} {^wrong \# args: should be "Togl::loadFonti \?fontnumber\?"$}
test "$PACKAGE-Togl::loadFonti" "normal use" {
	 set code [catch {Togl::loadFonti 3} msg]
	 set res "$code $msg"
} {^0 $}
test "$PACKAGE-Togl::loadFonti" "error" {
	 Togl::loadFonti 20
} {^Togl::loadFonti: unable to load font$}

### dumpCmapCmd ###
test "ObjTogl-dumpCmap" "too many args" {
    Togl::dumpCmap junk junk junk
} {^wrong \# args: should be "Togl::dumpCmap \?start_index=0\? \?end_index=255\?"}
test "ObjTogl-dumpCmap" "normal use" {
    set str [Togl::dumpCmap 0 3]
	 lindex $str 3
} "3$SP$FLT$SP$FLT$SP$FLT
"
test "ObjTogl-dumpCmap" "error from non-integral number arg1" {
    Togl::dumpCmap 0.5 1
} {expected integer but got "0\.5"}
test "ObjTogl-dumpCmap" "error from non-integral number arg2" {
    Togl::dumpCmap 0 1.5
} {expected integer but got "1\.5"}
test "ObjTogl-dumpCmap" "error from too small index" {
    Togl::dumpCmap -3 -1
} {^Togl::dumpCmap: colormap index out of range$}
test "ObjTogl-dumpCmap" "error from too large index" {
    Togl::dumpCmap 100 900
} {^Togl::dumpCmap: colormap index out of range$}

### dumpEpsCmd ###
test "ObjTogl-dumpEps" "too few args" {
    Togl::dumpEps
} {^wrong \# args: should be "Togl::dumpEps filename"$}
test "ObjTogl-dumpEps" "too few args" {
    Togl::dumpEps j u
} {^wrong \# args: should be "Togl::dumpEps filename"$}
test "ObjTogl-dumpEps" "normal use" {} $BLANK $no_test
test "ObjTogl-dumpEps" "no error" {} $BLANK $no_test

### scaleRectCmd ###
test "ObjTogl-scaleRect" "too few args" {
    Togl::scaleRect
} {^wrong \# args: should be "Togl::scaleRect scale"$}
test "ObjTogl-scaleRect" "too few args" {
    Togl::scaleRect j u
} {^wrong \# args: should be "Togl::scaleRect scale"$}
test "ObjTogl-scaleRect" "normal use on integer" {
	 catch {Togl::scaleRect 1}
} {^0$}
test "ObjTogl-scaleRect" "normal use on floating point" {
	 catch {Togl::scaleRect 1.5}
} {^0$}
test "ObjTogl-scaleRect" "error on scale by zero" {
    Togl::scaleRect 0
} {^Togl::scaleRect: invalid scaling factor$}
test "ObjTogl-scaleRect" "error on negative scale factor" {
    Togl::scaleRect -1.2
} {^Togl::scaleRect: invalid scaling factor$}

### setColorCmd ###
test "ObjTogl-setColor" "too few args" {
    Togl::setColor
} {^wrong \# args: should be "Togl::setColor index r g b"$}
test "ObjTogl-setColor" "too few args" {
    Togl::setColor j u n k y
} {^wrong \# args: should be "Togl::setColor index r g b"$}
test "ObjTogl-setColor" "normal use" {
	 # can only try this command if we have a private colormap
	 set pc [lindex [.togl_private configure -privatecmap] 5]
	 if {$pc == 1} { 
		  Togl::setColor 0 0.5 0.5 0.5
	 }
} {^$}
test "ObjTogl-setColor" "error" {
	 Togl::setColor -1 0.5 0.5 0.5
} {^Togl::setColor: color index must be in \[0, 255\]$}

### setFixedScaleCmd ###
test "ObjTogl-setFixedScale" "args" {
    Togl::setFixedScale
} {^wrong \# args: should be "Togl::setFixedScale scale"$}
test "ObjTogl-setFixedScale" "norm1" {
	 catch {Togl::setFixedScale 1}
} {^0$}
test "ObjTogl-setFixedScale" "norm2" {
	 catch {Togl::setFixedScale 1.5}
} {^0$}
test "ObjTogl-setFixedScale" "err1" {
    Togl::setFixedScale 0
} {^Togl::setFixedScale: invalid scaling factor$}
test "ObjTogl-setFixedScale" "err2" {
    Togl::setFixedScale -1.2
} {^Togl::setFixedScale: invalid scaling factor$}

### setMinRectCmd ###
test "ObjTogl-setMinRect" "args" {
    Togl::setMinRect
} {^wrong \# args: should be "Togl::setMinRect left top right bottom"$}
test "ObjTogl-setMinRect" "normal use" {
	 catch {Togl::setMinRect -1.3 15.2 2.6 1.3}
} {^0$}
test "ObjTogl-setMinRect" "err1" {
    Togl::setMinRect -1 0 1 0
} {^Togl::setMinRect: invalid rect$}
test "ObjTogl-setMinRect" "err2" {
    Togl::setMinRect 0 1 0 -1
} {^Togl::setMinRect: invalid rect$}


### setUnitAngleCmd ###
test "ObjTogl-setUnitAngle" "args" {
    Togl::setUnitAngle
} {^wrong \# args: should be "Togl::setUnitAngle angle_in_degrees"$}
test "ObjTogl-setUnitAngle" "normal use" {
	 catch {Togl::setUnitAngle 2.3}
} {^0$}
test "ObjTogl-setUnitAngle" "err1" {
    Togl::setUnitAngle 0
} {^Togl::setUnitAngle: unit angle must be positive$}
test "ObjTogl-setUnitAngle" "err2" {
    Togl::setUnitAngle -1.5
} {^Togl::setUnitAngle: unit angle must be positive$}

### setViewingDistanceCmd ###
test "ObjTogl-setViewingDistance" "args" {
    Togl::setViewingDistance
} {^wrong \# args: should be "Togl::setViewingDistance distance_in_inches"$}
test "ObjTogl-setViewingDistance" "normal use" {
	 catch {Togl::setViewingDistance 60}
} {^0$}
test "ObjTogl-setViewingDistance" "err1" {
    Togl::setViewingDistance 0
} {^Togl::setViewingDistance: .*$}
test "ObjTogl-setViewingDistance" "err2" {
    Togl::setViewingDistance -1
} {^Togl::setViewingDistance: .*$}

### toglDestroyCallback ###
test "ObjTogl-toglDestroyCallback" "test with destroy widget" {
	 togl .togl2 -rgba false
	 catch {destroy .togl2}
} {^0$} $no_test


### cleanup
unset PACKAGE