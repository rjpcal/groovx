##############################################################################
###
### TclGL
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### glBeginCmd ###
test "TclGL-glBegin" "args" {
	 glBegin
} {wrong \# args: should be}
test "TclGL-glBegin" "normal use" {} {^$}
test "TclGL-glBegin" "error" {} {^$}

### glEndCmd ###
test "TclGL-glEnd" "args" {
	 glEnd junk
} {wrong \# args: should be}
test "TclGL-glEnd" "normal use" {} {^$}
test "TclGL-glEnd" "error" {} {^$}

### glVertex4Cmd ###
test "TclGL-glVertex4" "args" {
	 glVertex4
} {wrong \# args: should be}
test "TclGL-glVertex4" "normal use" {} {^$}
test "TclGL-glVertex4" "error" {} {^$}

### glLineWidthCmd ###
test "TclGL-glLineWidth" "args" {
	 glLineWidth
} {wrong \# args: should be}
test "TclGL-glLineWidth" "normal use" {
	 glLineWidth 2.0
	 glGetDouble $::GL_LINE_WIDTH
} {^2.0$}
test "TclGL-glLineWidth" "error" {} {^$}

### glFlushCmd ###
test "TclGL-glFlush" "args" {
	 glFlush junk
} {wrong \# args: should be}
test "TclGL-glFlush" "normal use" {} {^$}
test "TclGL-glFlush" "error" {} {^$}

### drawOneLineCmd ###
test "TclGL-drawOneLine" "args" {
	 drawOneLine
} {wrong \# args: should be}
test "TclGL-drawOneLine" "normal use" {} {^$}
test "TclGL-drawOneLine" "error" {} {^$}

### drawThickLineCmd ###
test "TclGL-drawThickLine" "args" {
	 drawThickLine
} {wrong \# args: should be}
test "TclGL-drawThickLine" "normal use" {} {^$}
test "TclGL-drawThickLine" "error" {} {^$}

### antialiasCmd ###
test "TclGL-antialias" "args" {
	 antialias
} {wrong \# args: should be}
test "TclGL-antialias" "normal use" {} {^$}
test "TclGL-antialias" "error" {} {^$}

### lineInfoCmd ###
test "TclGL-lineInfo" "args" {
	 lineInfo junk
} {wrong \# args: should be}
test "TclGL-lineInfo" "normal use" {} {^$}
test "TclGL-lineInfo" "error" {} {^$}


