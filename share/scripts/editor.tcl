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
	 private variable itsControls
	 private variable itsToglet
	 private variable itsVisible true
	 private variable itsFieldPanes
	 private variable itsFieldControls
	 private variable itsEditObjList {}
	 private variable itsViewObjList {}
	 private variable itsEditObj 0
	 private variable itsViewObj 0
	 private variable itsObjType Face
	 private variable itsFieldNames {}

	 private method standardSettings {objs} {
		  GrObj::alignmentMode $objs $GrObj::CENTER_ON_CENTER
		  GrObj::scalingMode $objs $GrObj::MAINTAIN_ASPECT_SCALING
		  GrObj::renderMode $objs $GrObj::DIRECT_RENDER
		  GrObj::unRenderMode $objs $GrObj::CLEAR_BOUNDING_BOX
		  GrObj::height $objs 1.0
	 }

	 private method addNewObject {} {
		  # create new object
		  eval set obj [new $itsObjType]

		  standardSettings $obj

		  lappend itsEditObjList $obj
		  lappend itsViewObjList $obj
		  selectEditObj $obj
		  selectViewObj $obj
	 }

	 private method makePreviewObj {} {
		  set obj [Tlist::createPreview $itsEditObjList]
		  lappend itsViewObjList $obj
		  selectViewObj $obj
	 }

	 private method selectViewObj {obj} {
		  set itsViewObj $obj

		  $itsControls.viewobjspinner delete 0 end
		  $itsControls.viewobjspinner insert 0 $itsViewObj

		  requestDraw
	 }

	 private method selectEditObj {obj} {
		  set itsEditObj $obj

		  $itsControls.editobjspinner delete 0 end
		  $itsControls.editobjspinner insert 0 $itsEditObj

		  if { $itsEditObj == $itsViewObj } {
				Toglet::setVisible $itsToglet false
		  }

		  foreach field $itsFieldNames {
				set val [${itsObjType}::$field $obj]
				$itsFieldControls($field) set $val
		  }

		  if { $itsEditObj == $itsViewObj } {
				# need to force the controls to update before we draw the object
				update

				Toglet::setVisible $itsToglet true

				requestDraw
		  }
	 }

	 private method requestDraw {} {
		  if { $itsVisible && $itsViewObj != 0 } {
				Toglet::clearscreen $itsToglet
				Toglet::see $itsToglet $itsViewObj
				Toglet::swapBuffers $itsToglet
		  }
	 }

	 private method findNextObj {step objlist current} {
		  set current [lsearch $objlist $current]
		  incr current $step
		  set overflow [expr $current - [llength $objlist]]
		  if { $overflow >= 0 } {
				set current $overflow
		  } elseif { $current < 0 } {
				set current [expr [llength $objlist] + $current]
		  }

		  return [lindex $objlist $current]
	 }

	 private method nextEditObj {step} {
		  selectEditObj [findNextObj $step $itsEditObjList $itsEditObj]
	 }

	 private method nextViewObj {step} {
		  selectViewObj [findNextObj $step $itsViewObjList $itsViewObj]
	 }

	 private method viewingDist {val} {
		  Toglet::setViewingDistance $itsToglet $val
	 }

	 private method setAttrib {name val} {
		  if { $itsEditObj } {
				${itsObjType}::$name $itsEditObj $val
		  }
	 }

	 private method makeFieldControls {objtype} {

		  if { ![info exists itsFieldPanes($objtype)] } {
				$itsPanes insert 0 $objtype

				set fieldFrame [frame [$itsPanes childsite $objtype].fields]

				set currentframe $fieldFrame

				foreach field [${objtype}::fields]  {
					 set name [lindex $field 0]
					 set lower [lindex $field 1]
					 set upper [lindex $field 2]
					 set step [lindex $field 3]
					 set startsnewgroup [lindex $field 4]

					 if {$startsnewgroup} {
						  set currentframe [frame $fieldFrame.$name]
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

				pack $fieldFrame -fill y -side left
		  }

		  if { ![string equal $itsObjType $objtype] } {
				$itsPanes hide $itsObjType
		  }

		  $itsPanes show $objtype
		  set itsObjType $objtype
	 }

	 constructor {parent objtype} {
		  set itsObjType $objtype

		  set itsPanes [iwidgets::panedwindow $parent.panes \
					 -width 1000 -height 600]

		  $itsPanes add controls

		  set itsControls [$itsPanes childsite controls]

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

		  button $itsControls.preview -text "Make Preview" -relief raised \
					 -command [itcl::code $this makePreviewObj]
		  pack $itsControls.preview -side left -anchor nw

		  button $itsControls.redraw -text "Redraw" -relief raised \
					 -command [itcl::code $this requestDraw]
		  pack $itsControls.redraw -side left -anchor nw

		  iwidgets::spinner $itsControls.editobjspinner \
					 -labeltext "Edit object: " -fixed 5 \
					 -increment [itcl::code $this nextEditObj 1] \
					 -decrement [itcl::code $this nextEditObj -1]
		  pack $itsControls.editobjspinner -side left -anchor nw

		  iwidgets::spinner $itsControls.viewobjspinner \
					 -labeltext "View object: " -fixed 5 \
					 -increment [itcl::code $this nextViewObj 1] \
					 -decrement [itcl::code $this nextViewObj -1]
		  pack $itsControls.viewobjspinner -side left -anchor nw

		  #
		  # Set up attribs
		  #

		  Toglet::defaultParent $parent
		  set itsToglet [new Toglet]
		  Toglet::currentToglet $itsToglet

		  makeFieldControls $itsObjType

		  pack $itsPanes -side left -fill both -expand yes

		  glColor 0.0 0.0 0.0 1.0
		  glClearColor 1.0 1.0 1.0 1.0

		  glClear $::GL_COLOR_BUFFER_BIT
		  glLineWidth 1
	 }

	 public method loadObjects {filename} {
		  set ids [ObjDb::loadObjects $filename]
		  set itsEditObjList [${itsObjType}::findAll]
		  set itsViewObjList [GxNode::findAll]
		  standardSettings $itsEditObjList
		  return [llength $ids]
	 }

	 public method loadExpt {filename} {
		  Expt::load $filename
		  set itsEditObjList [${itsObjType}::findAll]
		  set itsViewObjList [GxNode::findAll]
		  standardSettings $itsEditObjList
	 }

	 public method saveObjects {filename} {
		  ObjDb::saveObjects $itsEditObjList $filename no
		  return [llength $itsEditObjList]
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
						  -mask "*.asw" \
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
						  -mask "*.obj" \
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
						  -fileson no \
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
