##############################################################################
###
### Toglet
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Toglet
package require Face
package require Tlist

### Togl::undrawCmd ###
test "Toglet-Togl::undraw" "too many args" {
	 Togl::undraw junk
} {wrong \# args: should be}
test "Toglet-Togl::undraw" "normal use" {
	 set f [Obj::new Face]
	 glIndexi 1; glColor 1.0 1.0 1.0 1.0
	 glClearIndex 0; glClearColor 0.0 0.0 0.0 1.0
	 clearscreen
	 Togl::see $f
	 Togl::undraw
	 set sum [pixelCheckSum]
	 return "[expr $sum == 0] $sum]"
} {^1 }
test "Toglet-Togl::undraw" "no error" {} $BLANK $no_test

### Togl::clearscreenCmd ###
test "Toglet-Togl::clearscreen" "too many args" {
    Togl::clearscreen junk
} {wrong \# args: should be}
test "Toglet-Togl::clearscreen" "normal use" {
	 glClearIndex 0; glClearColor 0.0 0.0 0.0 1.0
	 Togl::clearscreen
	 set p [pixelCheckSum]
	 return "[expr $p == 0] $p"
} {^1}

### Togl::showCmd ###
test "Toglet-Togl::show" "too few args" {
	 Togl::show
} {wrong \# args: should be}
test "Toglet-Togl::show" "too many args" {
	 Togl::show j u
} {wrong \# args: should be}
test "Toglet-Togl::show" "normal use on invalid trial id" { 
	 set code [catch {Togl::show -1} msg]
	 set result "$code $msg"
} {^1 Toglet::show:}
test "Toglet-Togl::show" "no error" {} $BLANK $no_test

### Togl::setVisibleCmd ###
test "Toglet-Togl::setVisible" "too few args" {
    Togl::setVisible
} {wrong \# args: should be}
test "Toglet-Togl::setVisible" "too many args" {
    Togl::setVisible j u
} {wrong \# args: should be}
test "Toglet-Togl::setVisible" "normal set off" {
	 catch {Togl::setVisible 0}
} {^0$}
test "Toglet-Togl::setVisible" "normal set on" {
	 catch {Togl::setVisible 1}
} {^0$}
test "Toglet-Togl::setVisible" "unusual set on" {
	 catch {Togl::setVisible -1}
} {^0$}
test "Toglet-Togl::setVisible" "error on non-numeric input" {
    Togl::setVisible junk
} {expected boolean value but got "junk"}

### Togl::setCurTrialCmd ###
test "Toglet-Togl::setCurTrial" "too few args" {
    Togl::setCurTrial
} {wrong \# args: should be}
test "Toglet-Togl::setCurTrial" "too many args" {
    Togl::setCurTrial j u
} {wrong \# args: should be}
test "Toglet-Togl::setCurTrial" "normal use" {
	 set f [Obj::new Face]
	 set t [Obj::new Trial]
	 Trial::addNode $t $f
	 catch {Togl::setCurTrial $t}
} {^0$}
test "Toglet-Togl::setCurTrial" "error on too low trial id" {
    Togl::setCurTrial -1
} {Toglet::setCurTrial:}
test "Toglet-Togl::setCurTrial" "error on too large trial id" {
    Togl::setCurTrial 10000000
} {Toglet::setCurTrial: attempted to access invalid object.*}

### dumpCmapCmd ###
test "Toglet-dumpCmap" "too many args" {
    Togl::dumpCmap junk junk junk
} {^wrong \# args: should be}
test "Toglet-dumpCmap" "normal use" {
    set str [Togl::dumpCmap 0 3]
	 lindex $str 3
} "3$SP$FLT$SP$FLT$SP$FLT"
test "Toglet-dumpCmap" "error from non-integral number arg1" {
    Togl::dumpCmap 0.5 1
} {expected integer but got "0\.5"}
test "Toglet-dumpCmap" "error from non-integral number arg2" {
    Togl::dumpCmap 0 1.5
} {expected integer but got "1\.5"}
test "Toglet-dumpCmap" "error from too small index" {
    Togl::dumpCmap -3 -1
} {^Toglet::dumpCmap:}
test "Toglet-dumpCmap" "error from too large index" {
    Togl::dumpCmap 100 900
} {^Toglet::dumpCmap: colormap index out of range$}

### pixelsPerUnitCmd ###
test "GxCamera-pixelsPerUnit" "args" {
    GxFixedScaleCamera::pixelsPerUnit
} {^wrong \# args: should be}
test "GxCamera-pixelsPerUnit" "norm1" {
    set c [new GxFixedScaleCamera]
    -> $c pixelsPerUnit 1
    delete $c
} {^$}
test "GxCamera-pixelsPerUnit" "norm2" {
    set c [new GxFixedScaleCamera]
    -> $c pixelsPerUnit 1.5
    delete $c
} {^$}
test "GxCamera-pixelsPerUnit" "err1" {
    set c [new GxFixedScaleCamera]
    catch {-> $c pixelsPerUnit 0} result
    delete $c
    set result
} {pixelsPerUnit: invalid scaling factor$}
test "GxCamera-pixelsPerUnit" "err2" {
    set c [new GxFixedScaleCamera]
    catch {-> $c pixelsPerUnit -1.2} result
    delete $c
    set result
} {pixelsPerUnit: invalid scaling factor$}

### unitAngle ###
test "GxCamera-unitAngle" "args" {
    GxFixedScaleCamera::unitAngle
} {^wrong \# args: should be}
test "GxCamera-unitAngle" "normal use" {
    set c [new GxFixedScaleCamera]
    -> $c unitAngle 2.3
    delete $c
} {^$}
test "GxCamera-unitAngle" "err1" {
    set c [new GxFixedScaleCamera]
    catch {-> $c unitAngle 0} result
    delete $c
    set result
} {unitAngle: unit angle must be positive$}
test "GxCamera-unitAngle" "err2" {
    set c [new GxFixedScaleCamera]
    catch {-> $c unitAngle 0} result
    delete $c
    set result
} {unitAngle: unit angle must be positive$}

### viewingDistIn ###
test "GxCamera-viewingDistIn" "args" {
    GxFixedScaleCamera::viewingDistIn
} {^wrong \# args: should be}
test "GxCamera-viewingDistIn" "normal use" {
    set c [new GxFixedScaleCamera]
    -> $c viewingDistIn 60
    delete $c
} {^$}
test "GxCamera-viewingDistIn" "err1" {
    set c [new GxFixedScaleCamera]
    catch {-> $c viewingDistIn 0} result
    delete $c
    set result
} {viewingDistIn: .*$}
test "GxCamera-viewingDistIn" "err2" {
    set c [new GxFixedScaleCamera]
    catch {-> $c viewingDistIn 0} result
    delete $c
    set result
} {viewingDistIn: .*$}

### setMinRectCmd ###
test "GxCamera-setMinRect" "args" {
    Togl::setMinRect
} {^wrong \# args: should be}
test "GxCamera-setMinRect" "normal use" {
	 catch {Togl::setMinRect -1.3 15.2 2.6 1.3}
} {^0$}
test "GxCamera-setMinRect" "err1" {
    Togl::setMinRect -1 0 1 0
} {^Toglet::setMinRect: invalid rect$}
test "GxCamera-setMinRect" "err2" {
    Togl::setMinRect 0 1 0 -1
} {^Toglet::setMinRect: invalid rect$}

