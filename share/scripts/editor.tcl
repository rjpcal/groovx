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
	 private variable itsUpper
	 private variable itsLower
	 private variable itsToglet
	 private variable itsVisible true
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

		  $itsLower.idspinner delete 0 end
		  $itsLower.idspinner insert 0 $itsCurrentObj

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

		  set itsPanes [iwidgets::panedwindow $parent.panes \
					 -width 1000 -height 600]

		  $itsPanes add upper
		  $itsPanes add lower

		  set itsLower [$itsPanes childsite lower]
		  set itsUpper [$itsPanes childsite upper]

		  $itsPanes fraction 80 20

		  #
		  # Set up controls
		  #

		  scale $itsLower.viewingdist -showvalue false \
					 -from 1 -to 200 -orient horizontal \
					 -command [itcl::code $this viewingDist]
		  $itsLower.viewingdist set 60
		  pack $itsLower.viewingdist -side top -fill x

		  button $itsLower.new -text "New Object" -relief raised \
					 -command [itcl::code $this addNewObject]
		  pack $itsLower.new -side left -anchor nw

		  button $itsLower.redraw -text "Redraw" -relief raised \
					 -command [itcl::code $this requestDraw]
		  pack $itsLower.redraw -side left -anchor nw

		  iwidgets::spinner $itsLower.idspinner \
					 -labeltext "Object id: " -fixed 5 \
					 -increment [itcl::code $this nextObject 1] \
					 -decrement [itcl::code $this nextObject -1]
		  pack $itsLower.idspinner -side left -anchor nw

		  #
		  # Set up attribs
		  #

		  set itsFieldPane [frame [$itsPanes childsite upper].fields]

		  set currentframe $itsFieldPane

		  foreach field [${itsObjType}::fields]  {
				set name [lindex $field 0]
				set lower [lindex $field 1]
				set upper [lindex $field 2]
				set step [lindex $field 3]
				set startsnewgroup [lindex $field 4]

				if {$startsnewgroup} {
					 set currentframe [frame $itsFieldPane.$name]
					 pack $currentframe -side left -fill y -expand yes
				}

				set pane $currentframe

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

		  pack $itsFieldPane -fill y -side right

		  Toglet::defaultParent $itsUpper
		  set itsToglet [new Toglet]
		  Toglet::currentToglet $itsToglet

		  pack $itsPanes -side left -fill both -expand yes

		  glColor 0.0 0.0 0.0 1.0
		  glClearColor 1.0 1.0 1.0 1.0

		  glClear $::GL_COLOR_BUFFER_BIT
		  glLineWidth 1
	 }

	 public method loadObjects {filename} {
		  set ids [ObjDb::loadObjects $filename]
		  set itsObjects [${itsObjType}::findAll]
		  return [llength $ids]
	 }

	 public method loadExpt {filename} {
		  Expt::load $filename
		  set itsObjects [${itsObjType}::findAll]
	 }

	 public method saveObjects {filename} {
		  ObjDb::saveObjects $itsObjects $filename
		  return [llength $itsObjects]
	 }
}

# get rid of the default widget
Toglet::destroy [Toglet::currentToglet]

itcl::class Menuapp {
	 private variable itsFrame
	 private variable itsHelpEntry
	 private variable itsMenuBar
	 private variable itsEditor
	 private variable itsLoadObjDialog ""
	 private variable itsLoadExptDialog ""
	 private variable itsSaveObjDialog ""


	 public method loadExpt {} {
		  if { [string length $itsLoadExptDialog] == 0 } {
				set itsLoadExptDialog [iwidgets::fileselectiondialog \
						  ${itsFrame}.lexd -title "Load experiment" \
						  -modality application]

				loadExpt
		  } else {
				if {[$itsLoadExptDialog activate]} {
					 set fname [$itsLoadExptDialog get]
					 $itsEditor loadExpt $fname
					 set ::statusInfo "Experiment loaded from '$fname'."
				} else {
					 set ::statusInfo "Load cancelled."
				}
		  }
	 }

	 public method loadObjects {} {
		  if { [string length $itsLoadObjDialog] == 0 } {
				set itsLoadObjDialog [iwidgets::fileselectiondialog \
						  ${itsFrame}.lobjd -title "Load objects" \
						  -modality application]

				loadObjects
		  } else {
				if {[$itsLoadObjDialog activate]} {
					 set fname [$itsLoadObjDialog get]
					 set num [$itsEditor loadObjects $fname]
					 set ::statusInfo "Loaded $num objects from '$fname'."
				} else {
					 set ::statusInfo "Load cancelled."
				}
		  }
	 }

	 public method saveObjects {} {
		  if { [string length $itsSaveObjDialog] == 0 } {
				set itsSaveObjDialog [iwidgets::fileselectiondialog \
						  ${itsFrame}.sobjd -title "Save objects" \
						  -modality application]

				saveObjects
		  } else {
				if {[$itsSaveObjDialog activate]} {
					 set fname [$itsSaveObjDialog get]
					 set num [$itsEditor saveObjects $fname]
					 set ::statusInfo "Saved $num objects to '$fname'."
				} else {
					 set ::statusInfo "Save cancelled."
				}
		  }
	 }

	 constructor {} {
		  set itsFrame [frame .fr]
		  set itsHelpEntry [entry .ef -textvariable statusInfo]

		  iwidgets::menubar .mb -helpvariable statusInfo -menubuttons {
				menubutton file -text File -menu {
					 options -tearoff true
					 command loadExpt -label {Load expt...} \
								-helpstr {Load an experiment} \
								-command {[itcl::code $this loadExpt]}
					 command loadObjects -label {Load objects...} \
								-helpstr {Load a set of objects} \
								-command {[itcl::code $this loadObjects]}
					 command saveObjects -label {Save objects...} \
								-helpstr {Save a set of objects} \
								-command {[itcl::code $this saveObjects]}
					 separator sep1
					 command exit -label Exit -command {exit} \
								-helpstr {Quit application}
				}
				menubutton edit -text Edit -menu {
					 options -tearoff true
					 if 0 {
						  command undo -label Undo -underline 0 -helpstr {Undo last command} -command {puts UNDO}
						  separator sep2
						  command cut -label Cut -underline 1 -helpstr {Cut selection to clipboard} -command {puts CUT}
						  command copy -label Copy -underline 1 -helpstr {Copy selection to clipboard} -command {puts COPY}
						  command paste -label Paste -underline 0 -helpstr {Paste clipboard contents} -command {puts PASTE}
					 }
				}
				menubutton options -text Options -menu {
					 options -tearoff true -selectcolor blue
					 if 0 {
						  radiobutton byName -variable viewMode -value NAME -label "by Name" -helpstr "View files by name order" -command {puts NAME}
						  radiobutton byDate -variable viewMode -value DATE -label "by Date" -helpstr "View files by date order" -command {puts DATE}
						  cascade prefs -label Preferences -menu {
								command colors -label Colors... -helpstr "Change text colors" -command {puts COLORS}
								command fonts -label Fonts... -helpstr "Change text font" -command {puts FONT}
						  }
					 }
				}
		  }

		  set itsMenuBar .mb

		  pack $itsMenuBar -anchor nw -fill x -expand yes
		  pack $itsFrame -fill both -expand yes
		  pack $itsHelpEntry -anchor sw -fill x -expand yes

		  set itsEditor [Editor #auto $itsFrame [lindex $::argv end]]
	 }
}

set app [Menuapp #auto]
