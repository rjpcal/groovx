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
	 private variable itsVisible true
	 private variable itsControls
	 private variable itsFieldPane
	 private variable itsFieldControls
	 private variable itsObjects {}
	 private variable itsCurrentObj 0
	 private variable itsObjType Face
	 private variable itsFieldNames {}

	 private method addNewObject {} {
		  # create new object
		  eval set id [new $itsObjType]
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

		  foreach field $itsFieldNames {
				set val [${itsObjType}::$field $id]
				$itsFieldControls($field) set $val
		  }
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

	 private method setAttrib {name val} {
		  if { $itsCurrentObj } {
				${itsObjType}::$name $itsCurrentObj $val
		  }
	 }

	 constructor {parent objtype} {
		  set itsObjType $objtype

		  set itsPanes [iwidgets::panedwindow .panes -width 600 -height 600]

		  $itsPanes add toglet
		  Toglet::defaultParent [$itsPanes childsite toglet]
		  set itsToglet [new Toglet]
		  Toglet::currentToglet $itsToglet

		  $itsPanes add controls
		  set itsControls [$itsPanes childsite controls]

		  $itsPanes fraction 60 40

		  #
		  # Set up controls
		  #

		  scale $itsControls.viewingdist -showvalue false \
					 -from 1 -to 200 -orient horizontal \
					 -command [itcl::code $this viewingDist]
		  $itsControls.viewingdist set 60
		  pack $itsControls.viewingdist -side top -fill x

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

		  #
		  # Set up attribs
		  #

		  set itsFieldPane [iwidgets::panedwindow .fields \
					 -orient vertical -width 600 -height 600]

		  foreach field [${itsObjType}::fields]  {
				if {[lindex $field 4]} {
					 $itsFieldPane add [lindex $field 0]
				}

				set pane [$itsFieldPane childsite end]

				set name [lindex $field 0]
				set lower [lindex $field 1]
				set upper [lindex $field 2]
				set step [lindex $field 3]

				lappend itsFieldNames $name

				scale $pane.$name -label $name -from $lower -to $upper \
						  -resolution $step -bigincrement $step \
						  -digits [string length $step] \
						  -repeatdelay 500 -repeatinterval 250 \
						  -orient horizontal \
						  -command [itcl::code $this setAttrib $name]

				pack $pane.$name -side top

				set itsFieldControls($name) $pane.$name
		  }

		  pack $itsFieldPane -expand yes -fill x -side left
		  pack $itsPanes -side left

		  glColor 0.0 0.0 0.0 1.0
		  glClearColor 1.0 1.0 1.0 1.0

		  glClear $::GL_COLOR_BUFFER_BIT
		  glLineWidth 1
	 }

	 public method useAll {} {
		  set itsObjects [${itsObjType}::findAll]
	 }
}

# get rid of the default widget
Toglet::destroy [Toglet::currentToglet]

set ed [Editor #auto . [lindex $argv end]]
