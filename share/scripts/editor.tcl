##############################################################################
#
# editor.tcl
# Rob Peters rjpeters@klab.caltech.edu
# created: Apr-99
# $Id$
#
##############################################################################

puts "in editor.tcl"

set RGBA 0
set DOUBLE 0

if { [string equal $::env(HOST) "curie.klab.caltech.edu"] || \
	  [string equal $::env(HOST) "hume.klab.caltech.edu"] } {
	 set ::DOUBLE 1
}

set ::CLASSNAME [lindex $argv end]

set ::XZOOM 8.0
set ::YZOOM 8.0

if { [string equal $::CLASSNAME "Fish"] } {
#	 set ::XZOOM 3.0
#	 set ::YZOOM 3.0
} elseif { [string equal $::CLASSNAME "Face"] } {
#	 set ::XZOOM 1.5
#	 set ::YZOOM 1.5
} elseif { [string equal $::CLASSNAME "MorphyFace"] } {
	 set ::XZOOM 6.0
	 set ::YZOOM 6.0
	 source facemaker.tcl
} elseif { [string equal $::CLASSNAME "House"] } {
#	 set ::XZOOM 5.0
#	 set ::YZOOM 5.0
	 source homemaker.tcl
} elseif { [string equal $::CLASSNAME "Gabor"] } {
#	 set ::XZOOM 2.0
#	 set ::YZOOM 2.0
	 if { [string equal $::env(HOST) "curie.klab.caltech.edu"] } {
		  set ::RGBA 1
	 }
}

if { ![Togl::inited] } {
	 Togl::init
	 Togl::width 1200
	 Togl::height 1000
	 Togl::setViewingDistance 30
	 Togl::setUnitAngle 2.05
}
pack forget .togl_private

#########################################################################

. configure -bg LightGray

toplevel .controls

frame .top_frame   -borderwidth 4 -relief raised
frame .controls.frame  -borderwidth 4 -relief raised
frame .fig_frame -borderwidth 4 -relief sunken

pack .top_frame   -padx 4 -pady 4 -anchor n  -fill x -expand 1 -side top 
pack .controls.frame  -padx 4 -pady 4 -anchor nw -fill y -expand 1 -side top
pack .fig_frame -padx 4 -pady 4 -anchor ne -fill both -expand 1 -side bottom 

button .new -text "New object" -relief raised -font {Helvetica 14} \
		  -command add_new_object
pack .new -side left -fill y -anchor nw -in .top_frame

button .redraw -text "Redraw" -relief raised -font Helvetica \
		  -command {clearscreen; request_draw}
pack .redraw -side left -fill y -anchor nw -in .top_frame

scale .id -label "Object id" -from 0 -to [expr [ObjList::count]-1] \
		  -resolution 1 -bigincrement 1 -repeatdelay 500 -repeatinterval 250 \
		  -font {Helvetica 9} -orient horizontal -command _id

pack .id -side left -fill y -anchor nw -in .top_frame
bind .id <ButtonPress> { update_max_id }

scale .zoomx -label "X Zoom" -from 0.1 -to 10.0 -resolution 0.1 \
		  -font {Helvetica 9} -orient horizontal -command reposition
.zoomx set $::XZOOM

scale .zoomy -label "Y Zoom" -from 0.1 -to 10.0 -resolution 0.1 \
		  -font {Helvetica 9} -orient horizontal -command reposition
.zoomy set $::YZOOM

scale .posx -label "X Position" -from \-10.0 -to 10.0 -resolution 0.1 \
		  -font {Helvetica 9} -orient horizontal -command reposition

scale .posy -label "Y Position" -from \-10.0 -to 10.0 -resolution 0.1 \
		  -font {Helvetica 9} -orient horizontal -command reposition

pack .zoomx .zoomy .posx .posy -side left -fill y -in .top_frame

set ID 0;
set POS [Pos::Pos]

proc reposition {dummy} {
	 if { [ObjList::isValidId $::ID] } undraw
	 Pos::scale $::POS [.zoomx get] [.zoomy get] 1.0
	 Pos::translate $::POS [.posx get] [.posy get] 1.0
	 request_draw
}

# NOSHOW, if non-zero, will temporarily disable redraws upon slider changes
set NOSHOW 1

Pos::scale $POS 0.8 0.8 1.0

set f 0
set ATTRIBUTES [${::CLASSNAME}::properties]
foreach attrib $ATTRIBUTES {

	 if { [lindex $attrib 4] } {
		  incr f
		  set current_frame .controls.frame.frame$f
		  frame $current_frame
		  pack $current_frame -padx 2 -pady 2 -side left -anchor nw -in .controls.frame
	 }

	 set name [lindex $attrib 0]
	 set lower [lindex $attrib 1]
	 set upper [lindex $attrib 2]
	 set step [lindex $attrib 3]
	 
	 eval scale .controls.frame.$name -label \"$name\" \
				-from \"$lower\" -to \"$upper\" \
				-resolution $step -bigincrement $step -digits [string length $step]\
				-repeatdelay 500 -repeatinterval 250 \
				-font {{Helvetica 9}} -orient horizontal -command _$name
	 set eval_id {$::ID}
	 set eval_val {$val}
	 eval "proc _$::name {val} { \
				if { \[ObjList::isValidId \$::ID\] } \
				{ ${::CLASSNAME}::$name \$::ID \$val }; \
				request_draw \
		  }"
	 
	 pack .controls.frame.$name -side top -anchor nw -in $current_frame
}


proc update_max_id {} {
	 set slider_max [lindex [.id configure -to] 4]
	 set max_objid [expr [ObjList::count] - 1]

	 if { $slider_max != $max_objid } {
		  .id configure -to $max_objid
	 }

	 return $max_objid
}

proc _id { val } {
	 set max_objid [update_max_id]
	 
	 if { $val > $max_objid } {
		  set val $max_objid
	 }

	 set ::ID $val
	 set prev_noshow $::NOSHOW
	 set ::NOSHOW 1
	 catch {
		  foreach attrib $::ATTRIBUTES {
				set name [lindex $attrib 0]
				if { ![string equal "FRAME" $name] } {
					 if { [ObjList::isValidId $::ID] } {
						  eval .controls.frame.$name set [${::CLASSNAME}::$name $::ID]
					 }
				}
		  }
	 }
	 update
	 set ::NOSHOW $prev_noshow
	 request_draw
}

proc load_faces { datafile } {
	 Face::loadFaces $::env(HOME)/face/faces_triads/$datafile
	 Tlist::makeSingles $::POS
	 .id configure -to [expr [ObjList::count]-1]
}

proc request_draw {} {
	 if { $::NOSHOW == 0 && [ObjList::isValidId $::ID] } {
		  if { $::DOUBLE } { clearscreen }
		  show $::ID
		  if { $::DOUBLE } { Togl::swapBuffers }
	 }
}

proc add_new_object {} {
	 # create new object
	 eval set id [${::CLASSNAME}::${::CLASSNAME}]
	 GrObj::alignmentMode $id $GrObj::CENTER_ON_CENTER
	 GrObj::scalingMode $id $GrObj::MAINTAIN_ASPECT_SCALING
	 GrObj::unRenderMode $id $GrObj::GROBJ_CLEAR_BOUNDING_BOX
	 GrObj::height $id 1.0
	 # add it to the trial list
	 Tlist::makeSingles $::POS
	 # rescale the id slider
	 .id configure -to [expr [ObjList::count]-1]
	 .id set $id
}

#pack .togl_private -side right -in .fig_frame
pack .togl_private -fill both -expand 1

source $env(HOME)/face/shared/app.tcl

if { $RGBA } {
	 glColor 0.0 0.0 0.0 1.0
	 glClearColor 1.0 1.0 1.0 1.0
} else {
	 glIndexi 0
	 glClearIndex 1

	 if { [string equal $::env(HOST) "curie.klab.caltech.edu"] } {
		  glClearIndex 7
	 }

}
glClear $::GL_COLOR_BUFFER_BIT
glLineWidth 4

Togl::setVisible false
update
add_new_object
update

Togl::setVisible true

set NOSHOW 0
request_draw

bind all <ButtonPress-2> { clearscreen; request_draw }
