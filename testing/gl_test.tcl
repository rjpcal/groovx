##############################################################################
###
### TclGL
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### glBeginCmd ###
test "TclGL-glBegin" "args" {glBegin} {wrong \# args: should be "glBegin mode"}
test "TclGL-glBegin" "normal use" {} {^$}
test "TclGL-glBegin" "error" {} {^$}

### glEndCmd ###
test "TclGL-glEnd" "args" {glEnd junk} {wrong \# args: should be "glEnd"}
test "TclGL-glEnd" "normal use" {} {^$}
test "TclGL-glEnd" "error" {} {^$}

### glVertex3fCmd ###
test "TclGL-glVertex3f" "args" {
	 glVertex3f
} {wrong \# args: should be "glVertex3f x y z"}
test "TclGL-glVertex3f" "normal use" {} {^$}
test "TclGL-glVertex3f" "error" {} {^$}

### glIndexiCmd ###
test "TclGL-glIndexi" "args" {
	 glIndexi
} {wrong \# args: should be "glIndexi index"}
test "TclGL-glIndexi" "normal use" {} {^$}
test "TclGL-glIndexi" "error" {} {^$}

### glLineWidthCmd ###
test "TclGL-glLineWidth" "args" {
	 glLineWidth
} {wrong \# args: should be "glLineWidth width"}
test "TclGL-glLineWidth" "normal use" {} {^$}
test "TclGL-glLineWidth" "error" {} {^$}

### glFlushCmd ###
test "TclGL-glFlush" "args" {glFlush junk} {wrong \# args: should be "glFlush"}
test "TclGL-glFlush" "normal use" {} {^$}
test "TclGL-glFlush" "error" {} {^$}

### setBackgroundCmd ###
test "TclGL-setBackground" "args" {
	 setBackground
} {wrong \# args: should be "setBackground { \[index\] or \[R G B\] }"}
test "TclGL-setBackground" "normal use" {} {^$}
test "TclGL-setBackground" "error" {} {^$}

### setForegroundCmd ###
test "TclGL-setForeground" "args" {
	 setForeground
} {wrong \# args: should be "setForeground { \[index\] or \[R G B\] }"}
test "TclGL-setForeground" "normal use" {} {^$}
test "TclGL-setForeground" "error" {} {^$}

### drawOneLineCmd ###
test "TclGL-drawOneLine" "args" {
	 drawOneLine
} {wrong \# args: should be "drawOneLine x1 y1 x2 y2"}
test "TclGL-drawOneLine" "normal use" {} {^$}
test "TclGL-drawOneLine" "error" {} {^$}

### drawThickLineCmd ###
test "TclGL-drawThickLine" "args" {
	 drawThickLine
} {wrong \# args: should be "drawThickLine x1 y1 x2 y2 thickness"}
test "TclGL-drawThickLine" "normal use" {} {^$}
test "TclGL-drawThickLine" "error" {} {^$}

### antialiasCmd ###
test "TclGL-antialias" "args" {
	 antialias
} {wrong \# args: should be "antialias on_off"}
test "TclGL-antialias" "normal use" {} {^$}
test "TclGL-antialias" "error" {} {^$}

### lineInfoCmd ###
test "TclGL-lineInfo" "args" {
	 lineInfo junk
} {wrong \# args: should be "lineInfo"}
test "TclGL-lineInfo" "normal use" {} {^$}
test "TclGL-lineInfo" "error" {} {^$}


