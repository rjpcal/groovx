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

### Toglet::undrawCmd ###
test "Toglet::undraw" "too many args" {
    ::undraw junk
} {wrong \# args: should be}
test "Toglet::undraw" "normal use" {
    set f [Obj::new Face]
    glIndexi 1; glColor 1.0 1.0 1.0 1.0
    glClearIndex 0; glClearColor 0.0 0.0 0.0 1.0
    ::clearscreen
    ::see $f
    ::undraw
    set sum [pixelCheckSum]
    return "[expr $sum == 0] $sum"
} {^1 }
test "Toglet::undraw" "no error" {} $BLANK $no_test

### Toglet::clearscreenCmd ###
test "Toglet::clearscreen" "too many args" {
    ::clearscreen junk
} {wrong \# args: should be}
test "Toglet::clearscreen" "normal use" {
    glClearIndex 0; glClearColor 0.0 0.0 0.0 1.0
    ::clearscreen
    set p [pixelCheckSum]
    return "[expr $p == 0] $p"
} {^1}

### Toglet::setVisibleCmd ###
test "Toglet::setVisible" "too few args" {
    Toglet::setVisible
} {wrong \# args: should be}
test "Toglet::setVisible" "too many args" {
    Toglet::setVisible j u n
} {wrong \# args: should be}
test "Toglet::setVisible" "normal set off" {
    catch {Toglet::setVisible [Toglet::current] 0}
} {^0$}
test "Toglet::setVisible" "normal set on" {
    catch {Toglet::setVisible [Toglet::current] 1}
} {^0$}
test "Toglet::setVisible" "unusual set on" {
    catch {Toglet::setVisible [Toglet::current] -1}
} {^0$}
test "Toglet::setVisible" "error on non-numeric input" {
    Toglet::setVisible [Toglet::current] junk
} {expected boolean value but got "junk"}

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
} {pixelsPerUnit: invalid scaling factor}
test "GxCamera-pixelsPerUnit" "err2" {
    set c [new GxFixedScaleCamera]
    catch {-> $c pixelsPerUnit -1.2} result
    delete $c
    set result
} {pixelsPerUnit: invalid scaling factor}

### unitAngle ###
test "GxCamera-unitAngle" "args" {
    GxPsyphyCamera::unitAngle
} {^wrong \# args: should be}
test "GxCamera-unitAngle" "normal use" {
    set c [new GxPsyphyCamera]
    -> $c unitAngle 2.3
    delete $c
} {^$}
test "GxCamera-unitAngle" "err1" {
    set c [new GxPsyphyCamera]
    catch {-> $c unitAngle 0} result
    delete $c
    set result
} {unitAngle: unit angle must be positive}
test "GxCamera-unitAngle" "err2" {
    set c [new GxPsyphyCamera]
    catch {-> $c unitAngle 0} result
    delete $c
    set result
} {unitAngle: unit angle must be positive}

### viewingDistIn ###
test "GxCamera-viewingDistIn" "args" {
    GxPsyphyCamera::viewingDistIn
} {^wrong \# args: should be}
test "GxCamera-viewingDistIn" "normal use" {
    set c [new GxPsyphyCamera]
    -> $c viewingDistIn 60
    delete $c
} {^$}
test "GxCamera-viewingDistIn" "err1" {
    set c [new GxPsyphyCamera]
    catch {-> $c viewingDistIn 0} result
    delete $c
    set result
} {viewingDistIn: .*$}
test "GxCamera-viewingDistIn" "err2" {
    set c [new GxPsyphyCamera]
    catch {-> $c viewingDistIn 0} result
    delete $c
    set result
} {viewingDistIn: .*$}

### reshape bugfix ###
test "GxCamera-reshape" "bugfix" {
    # Fixed 2-Jun-2004 in gfx/gxcamera.cc
    set t [Toglet::current]
    wm geometry . 500x500
    update idletasks
    set viewport [glGetInteger $::GL_VIEWPORT]
} {^0 0 500 500$}
