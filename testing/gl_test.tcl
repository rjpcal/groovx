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
} {wrong \# args: should be "glBegin mode"}
test "TclGL-glBegin" "normal use" {} {^$}
test "TclGL-glBegin" "error" {} {^$}

### glEndCmd ###
test "TclGL-glEnd" "args" {
	 glEnd junk
} {wrong \# args: should be "glEnd"}
test "TclGL-glEnd" "normal use" {} {^$}
test "TclGL-glEnd" "error" {} {^$}

### glVertexCmd ###
test "TclGL-glVertex" "args" {
	 glVertex
} {wrong \# args: should be "glVertex x y \?z=0\? \?w=1\?"}
test "TclGL-glVertex" "normal use" {} {^$}
test "TclGL-glVertex" "error" {} {^$}

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
test "TclGL-glFlush" "args" {
	 glFlush junk
} {wrong \# args: should be "glFlush"}
test "TclGL-glFlush" "normal use" {} {^$}
test "TclGL-glFlush" "error" {} {^$}

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


