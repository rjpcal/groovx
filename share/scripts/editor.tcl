##############################################################################
#
# editor.tcl
# Rob Peters rjpeters@klab.caltech.edu
# created: Apr-99
# $Id$
#
##############################################################################

package require Iwidgets

itcl::class Editor {
	 private variable itsPanes
	 private variable itsToglet
	 private variable itsControls
	 private variable itsAttribs
	 private variable itsObjects {}
	 private variable itsCurrentObj {}
	 private variable itsVisible true
	 private variable itsObjectType Face

	 private method addNewObject {} {
		  # create new object
		  eval set id [new $itsObjectType]
		  GrObj::alignmentMode $id $GrObj::CENTER_ON_CENTER
		  GrObj::scalingMode $id $GrObj::MAINTAIN_ASPECT_SCALING
		  GrObj::unRenderMode $id $GrObj::CLEAR_BOUNDING_BOX
		  GrObj::height $id 1.0

		  lappend itsObjects $id
		  selectObject $id
	 }

	 private method selectObject {id} {
		  set itsCurrentObj $id

		  $itsControls.idspinner delete 0 end
		  $itsControls.idspinner insert 0 $itsCurrentObj

		  requestDraw
	 }

	 private method requestDraw {} {
		  if { $itsVisible } {
				Toglet::clearscreen $itsToglet
				Toglet::see $itsToglet $itsCurrentObj
				Toglet::swapBuffers $itsToglet
		  }
	 }

	 private method nextObject {step} {
		  set current [lsearch $itsObjects $itsCurrentObj]
		  incr current $step
		  set overflow [expr $current - [llength $itsObjects]]
		  if { $overflow >= 0 } {
				set current $overflow
		  } elseif { $current < 0 } {
				set current [expr [llength $itsObjects] + $current]
		  }

		  selectObject [lindex $itsObjects $current]
	 }

	 private method viewingDist {val} {
		  Toglet::setViewingDistance $itsToglet $val
	 }

	 constructor {} {
		  set itsObjectType [lindex $::argv end]

		  set itsPanes [iwidgets::panedwindow .panes -width 600 -height 600]

		  $itsPanes add toglet
		  Toglet::defaultParent [$itsPanes childsite toglet]
		  set itsToglet [new Toglet]
		  Toglet::currentToglet $itsToglet

		  $itsPanes add controls
		  set itsControls [$itsPanes childsite controls]

		  $itsPanes add attribs
		  set itsAttribs [$itsPanes childsite attribs]

		  $itsPanes fraction 50 20 30

		  button $itsControls.new -text "New Object" -relief raised \
					 -command [itcl::code $this addNewObject]
		  pack $itsControls.new -side left -anchor nw

		  button $itsControls.redraw -text "Redraw" -relief raised \
					 -command [itcl::code $this requestDraw]
		  pack $itsControls.redraw -side left -anchor nw

		  iwidgets::spinner $itsControls.idspinner \
					 -labeltext "Object id: " -fixed 5 \
					 -increment [itcl::code $this nextObject 1] \
					 -decrement [itcl::code $this nextObject -1]
		  pack $itsControls.idspinner -side left -anchor nw

		  scale $itsControls.viewingdist -label "Scale" \
					 -from 1 -to 200 -showvalue true -orient horizontal \
					 -command [itcl::code $this viewingDist]
		  $itsControls.viewingdist set 60
		  pack $itsControls.viewingdist -side left -anchor nw

		  pack $itsPanes

		  glColor 0.0 0.0 0.0 1.0
		  glClearColor 1.0 1.0 1.0 1.0

		  glClear $::GL_COLOR_BUFFER_BIT
		  glLineWidth 4
	 }
}

# get rid of the default widget
Toglet::destroy [Toglet::currentToglet]

Editor #auto

return

set ID 0;
set POS [Pos::Pos]

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

source $env(HOME)/face/shared/app.tcl

bind all <ButtonPress-2> { clearscreen; request_draw }
