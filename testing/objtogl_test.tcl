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
	 if { ![Togl::inited] } { Togl::init; update }
} {^$}

### Togl::undrawCmd ###
test "ObjTogl-Togl::undraw" "too many args" {
	 Togl::undraw junk
} {wrong \# args: should be}
test "ObjTogl-Togl::undraw" "normal use" {
	 set f [Obj::new Face]
	 glIndexi 1
	 glClearIndex 0
	 clearscreen
	 Togl::see $f
	 Togl::undraw
	 set sum [pixelCheckSum]
	 return "[expr $sum == 0] $sum]"
} {^1 }
test "ObjTogl-Togl::undraw" "no error" {} $BLANK $no_test

### Togl::refreshCmd ###
test "ObjTogl-Togl::refresh" "too many args" {
	 Togl::refresh junk
} {wrong \# args: should be}
test "ObjTogl-Togl::refresh" "normal use" {
	 set f [Obj::new Face]
	 set p [Obj::new Position]
	 set t [Obj::new Trial]
	 Trial::add $t $f $p
	 Togl::setCurTrial $t
	 glIndexi 1
	 glClearIndex 0
	 Togl::setVisible 1
	 Togl::refresh
	 # check to see if some pixels actually got drawn
	 expr [pixelCheckSum] > 500
} {^1$}
test "ObjTogl-Togl::refresh" "no error" {} $BLANK $no_test

### Togl::clearscreenCmd ###
test "ObjTogl-Togl::clearscreen" "too many args" {
    Togl::clearscreen junk
} {wrong \# args: should be}
test "ObjTogl-Togl::clearscreen" "normal use" {
	 glClearIndex 0
	 Togl::clearscreen
	 set p [pixelCheckSum]
	 return "[expr $p == 0] $p"
} {^1}

### Togl::showCmd ###
test "ObjTogl-Togl::show" "too few args" {
	 Togl::show
} {wrong \# args: should be}
test "ObjTogl-Togl::show" "too many args" {
	 Togl::show j u
} {wrong \# args: should be}
test "ObjTogl-Togl::show" "normal use on invalid trial id" { 
	 set code [catch {Togl::show -1} msg]
	 set result "$code $msg"
} {^1 Toglet::show: attempted to access invalid object.*}
test "ObjTogl-Togl::show" "no error" {} $BLANK $no_test

### Togl::setVisibleCmd ###
test "ObjTogl-Togl::setVisible" "too few args" {
    Togl::setVisible
} {wrong \# args: should be}
test "ObjTogl-Togl::setVisible" "too many args" {
    Togl::setVisible j u
} {wrong \# args: should be}
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
} {wrong \# args: should be}
test "ObjTogl-Togl::setCurTrial" "too many args" {
    Togl::setCurTrial j u
} {wrong \# args: should be}
test "ObjTogl-Togl::setCurTrial" "normal use" {
	 set f [Obj::new Face]
	 set p [Obj::new Position]
	 set t [Obj::new Trial]
	 Trial::add $t $f $p
	 catch {Togl::setCurTrial $t}
} {^0$}
test "ObjTogl-Togl::setCurTrial" "error on too low trial id" {
    Togl::setCurTrial -1
} {Toglet::setCurTrial: attempted to access invalid object.*}
test "ObjTogl-Togl::setCurTrial" "error on too large trial id" {
    Togl::setCurTrial 10000000
} {Toglet::setCurTrial: attempted to access invalid object.*}

### Togl::loadFontCmd ###
test "$PACKAGE-Togl::loadFont" "too many args" {
	 Togl::loadFont fixed junk
} {^wrong \# args: should be}
test "$PACKAGE-Togl::loadDefaultFont" "normal use" {
	 set code [catch {Togl::loadDefaultFont} msg]
	 set res "$code $msg"
} {^0 $}
test "$PACKAGE-Togl::loadFont" "error" {
	 Togl::loadFont junk
} {^Toglet::loadFont: unable to load font junk$}

### Togl::loadFontiCmd ###
test "$PACKAGE-Togl::loadFonti" "too many args" {
	 Togl::loadFonti 3 junk
} {^wrong \# args: should be}
test "$PACKAGE-Togl::loadFonti" "normal use" {
	 set code [catch {Togl::loadFonti 3} msg]
	 set res "$code $msg"
} {^0 $}

### dumpCmapCmd ###
test "ObjTogl-dumpCmap" "too many args" {
    Togl::dumpCmap junk junk junk
} {^wrong \# args: should be}
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
} {^Toglet::dumpCmap: colormap index out of range$}
test "ObjTogl-dumpCmap" "error from too large index" {
    Togl::dumpCmap 100 900
} {^Toglet::dumpCmap: colormap index out of range$}

### dumpEpsCmd ###
test "ObjTogl-dumpEps" "too few args" {
    Togl::dumpEps
} {^wrong \# args: should be}
test "ObjTogl-dumpEps" "too few args" {
    Togl::dumpEps j u
} {^wrong \# args: should be}
test "ObjTogl-dumpEps" "normal use" {} $BLANK $no_test
test "ObjTogl-dumpEps" "no error" {} $BLANK $no_test

### scaleRectCmd ###
test "ObjTogl-scaleRect" "too few args" {
    Togl::scaleRect
} {^wrong \# args: should be}
test "ObjTogl-scaleRect" "too few args" {
    Togl::scaleRect j u
} {^wrong \# args: should be}
test "ObjTogl-scaleRect" "normal use on integer" {
	 catch {Togl::scaleRect 1}
} {^0$}
test "ObjTogl-scaleRect" "normal use on floating point" {
	 catch {Togl::scaleRect 1.5}
} {^0$}
test "ObjTogl-scaleRect" "error on scale by zero" {
    Togl::scaleRect 0
} {^Toglet::scaleRect: invalid scaling factor$}
test "ObjTogl-scaleRect" "error on negative scale factor" {
    Togl::scaleRect -1.2
} {^Toglet::scaleRect: invalid scaling factor$}

### setColorCmd ###
test "ObjTogl-setColor" "too few args" {
    Togl::setColor
} {^wrong \# args: should be}
test "ObjTogl-setColor" "too few args" {
    Togl::setColor j u n k y
} {^wrong \# args: should be}
test "ObjTogl-setColor" "error" {
	 Togl::setColor -1 0.5 0.5 0.5
} {^Toglet::setColor: color index must be in \[0, 255\]$}

### setFixedScaleCmd ###
test "ObjTogl-setFixedScale" "args" {
    Togl::setFixedScale
} {^wrong \# args: should be}
test "ObjTogl-setFixedScale" "norm1" {
	 catch {Togl::setFixedScale 1}
} {^0$}
test "ObjTogl-setFixedScale" "norm2" {
	 catch {Togl::setFixedScale 1.5}
} {^0$}
test "ObjTogl-setFixedScale" "err1" {
    Togl::setFixedScale 0
} {^Toglet::setFixedScale: invalid scaling factor$}
test "ObjTogl-setFixedScale" "err2" {
    Togl::setFixedScale -1.2
} {^Toglet::setFixedScale: invalid scaling factor$}

### setMinRectCmd ###
test "ObjTogl-setMinRect" "args" {
    Togl::setMinRect
} {^wrong \# args: should be}
test "ObjTogl-setMinRect" "normal use" {
	 catch {Togl::setMinRect -1.3 15.2 2.6 1.3}
} {^0$}
test "ObjTogl-setMinRect" "err1" {
    Togl::setMinRect -1 0 1 0
} {^Toglet::setMinRect: invalid rect$}
test "ObjTogl-setMinRect" "err2" {
    Togl::setMinRect 0 1 0 -1
} {^Toglet::setMinRect: invalid rect$}


### setUnitAngleCmd ###
test "ObjTogl-setUnitAngle" "args" {
    Togl::setUnitAngle
} {^wrong \# args: should be}
test "ObjTogl-setUnitAngle" "normal use" {
	 catch {Togl::setUnitAngle 2.3}
} {^0$}
test "ObjTogl-setUnitAngle" "err1" {
    Togl::setUnitAngle 0
} {^Toglet::setUnitAngle: unit angle must be positive$}
test "ObjTogl-setUnitAngle" "err2" {
    Togl::setUnitAngle -1.5
} {^Toglet::setUnitAngle: unit angle must be positive$}

### setViewingDistanceCmd ###
test "ObjTogl-setViewingDistance" "args" {
    Togl::setViewingDistance
} {^wrong \# args: should be}
test "ObjTogl-setViewingDistance" "normal use" {
	 catch {Togl::setViewingDistance 60}
} {^0$}
test "ObjTogl-setViewingDistance" "err1" {
    Togl::setViewingDistance 0
} {^Toglet::setViewingDistance: .*$}
test "ObjTogl-setViewingDistance" "err2" {
    Togl::setViewingDistance -1
} {^Toglet::setViewingDistance: .*$}

### toglDestroyCallback ###
test "ObjTogl-toglDestroyCallback" "test with destroy widget" {
	 togl .togl2 -rgba false
	 catch {destroy .togl2}
} {^0$} $no_test


### cleanup
unset PACKAGE