##############################################################################
#
# editor.tcl
# Rob Peters rjpeters@klab.caltech.edu
# created: Apr-99
# $Id$
#
##############################################################################

package require Iwidgets

# parent
#   parent.itsPanes
#     parent.itsPanes.objtype (childsite)
#       parent.itsPanes.objtype.fields (frame)
#     parent.itsPanes.itsControls (childsite)
#     parent.itsPanes.itsControls.viewingdist
#     parent.itsPanes.itsControls.new
#     parent.itsPanes.itsControls.preview
#     parent.itsPanes.itsControls.redraw
#     parent.itsPanes.itsControls.editobjlist
#     parent.itsPanes.itsControls.viewobjlist
#   parent.itsToglet

itcl::class FieldControls {
	 private variable itsObjType
	 private variable itsNames
	 private variable itsControls
	 private variable itsCachedValues

	 constructor {objtype} {
		  set itsObjType $objtype
		  set itsNames [list]
		  foreach field [${objtype}::allFields] {
				lappend itsNames [lindex $field 0]
				set itsCachedValues($field) 0
		  }
	 }

	 public method names {} { return $itsNames }

	 public method addControl {name path} { set itsControls($name) $path }

	 public method getControl {name} { return $itsControls($name) }

	 public method getCachedVal {name} { return $itsCachedValues($name) }

	 public method setCachedVal {name val} { set itsCachedValues($name) $val }

	 public method update {obj} {
		  foreach field $itsNames {
				set val [${itsObjType}::$field $obj]
				set control $itsControls($field)
				$control set $val
				set itsCachedValues($field) $val
		  }
	 }
}

itcl::class Editor {
	 private variable itsPanes
	 private variable itsControls
	 private variable itsToglet
	 private variable itsObjType Face
	 private variable itsControlsArray
	 private variable itsUpdateInProgress 0

	 private method standardSettings {objs} {
		  set grobjs [dlist_select $objs [GrObj::is $objs]]

		  if { [llength $grobjs] > 0 } {
				GrObj::alignmentMode $grobjs $GrObj::CENTER_ON_CENTER
				GrObj::scalingMode $grobjs $GrObj::MAINTAIN_ASPECT_SCALING
				GrObj::renderMode $grobjs $GrObj::DIRECT_RENDER
				GrObj::unRenderMode $grobjs $GrObj::CLEAR_BOUNDING_BOX
				GrObj::height $grobjs 1.0
		  }
	 }

	 private method addNewObject {} {
		  # create new object
		  eval set obj [new $itsObjType]

		  standardSettings $obj

		  $itsControls.editobjlist insert end $obj
		  setEditSelection $obj

		  addViewObj $obj
		  setViewSelection $obj
	 }

	 private method addViewObj {obj} {
		  $itsControls.viewobjlist insert end $obj
	 }

	 private method makePreviewObj {} {
		  set obj [Tlist::createPreview [getEditSelection]]

		  addViewObj $obj
		  setViewSelection $obj
	 }

	 private method getEditAll {} {
		  return [$itsControls.editobjlist get 0 end]
	 }

	 private method getViewAll {} {
		  return [$itsControls.viewobjlist get 0 end]
	 }

	 private method getEditSelection {} {
		  return [$itsControls.editobjlist getcurselection]
	 }

	 private method getViewSelection {} {
		  set sel [$itsControls.viewobjlist getcurselection]
		  if { [llength $sel] > 0 } {
				return [lindex $sel 0]
		  } else {
				return 0
		  }
	 }

	 private method setEditSelection {objs} {
		  $itsControls.editobjlist selection clear 0 end
		  set all [getEditAll]
		  foreach obj $objs {
				set index [lsearch $all $obj]
				if { $index >= 0 } {
					 $itsControls.editobjlist selection set $index
					 onEditObjSelect
				}
		  }
	 }

	 private method setViewSelection {objs} {
		  $itsControls.viewobjlist selection clear 0 end
		  set all [getViewAll]
		  foreach obj $objs {
				set index [lsearch $all $obj]
				if { $index >= 0 } {
					 $itsControls.viewobjlist selection set $index
					 onViewObjSelect
					 requestDraw
					 return
				}
		  }
	 }

	 private method onEditObjSelect {} {
		  set objs [getEditSelection]
		  if { [llength $objs] > 0 } {
				updateControls [lindex $objs 0]
		  }
	 }

	 private method onViewObjSelect {} {
		  requestDraw
	 }

	 private method requestDraw {} {
		  set viewobj [getViewSelection]
		  if { $viewobj != 0 } {
				Toglet::see $itsToglet $viewobj
		  }
	 }

	 private method updateControls {obj} {
		  set itsUpdateInProgress 1

		  set viewobj [getViewSelection]

		  set objtype [IO::type $obj]

		  set controls $itsControlsArray($objtype)

		  $controls update $obj

		  # need to force the controls to update before we draw the object
		  update

		  set itsUpdateInProgress 0
	 }

	 private method viewingDist {val} {
		  Toglet::setViewingDistance $itsToglet $val
	 }

	 private method setAttrib {name val} {
		  set editobjs [getEditSelection]

		  set controls $itsControlsArray($itsObjType)

		  if { !$itsUpdateInProgress && [llength $editobjs] > 0 } {
				if { [$controls getCachedVal $name] != $val } {
					 #puts "setting $editobjs $name to $val"
					 Toglet::allowRefresh $itsToglet 0
					 ${itsObjType}::$name $editobjs $val
					 $controls setCachedVal $name $val
					 Toglet::allowRefresh $itsToglet 1
				}

				updateControls [lindex $editobjs 0]
		  }
	 }

	 private method makeFieldControls {objtype} {

		  if { ![info exists itsControlsArray($objtype)] } {
				$itsPanes insert 0 $objtype

				set fieldFrame [frame [$itsPanes childsite $objtype].fields]

				set currentframe ""

				set itsControlsArray($objtype) [FieldControls #auto $objtype]

				foreach field [${objtype}::allFields] {
					 set name [lindex $field 0]
					 set lower [lindex $field 1]
					 set upper [lindex $field 2]
					 set step [lindex $field 3]
					 set flags [lindex $field 4]

					 set startsnewgroup [expr [lsearch $flags NEW_GROUP] != -1]
					 set transient [expr [lsearch $flags TRANSIENT] != -1]

					 if {$startsnewgroup} {
						  set currentframe [frame $fieldFrame.$name]
						  pack $currentframe -side left -fill y -expand yes
					 }

					 set pane $currentframe

					 scale $pane.$name -label $name -from $lower -to $upper \
								-resolution $step -bigincrement $step \
								-digits [string length $step] \
								-repeatdelay 500 -repeatinterval 250 \
								-orient horizontal \
								-command [itcl::code $this setAttrib $name]

					 if {$transient} {
						  $pane.$name configure -fg blue
					 }

					 pack $pane.$name -side top

					 $itsControlsArray($objtype) addControl $name $pane.$name
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
					 -width 900 -height 900]

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

		  iwidgets::scrolledlistbox $itsControls.editobjlist \
					 -labeltext "Edit objects:" -hscrollmode dynamic \
					 -selectmode extended -exportselection false \
					 -selectioncommand [itcl::code $this onEditObjSelect]
		  pack $itsControls.editobjlist -side left -anchor nw

		  iwidgets::scrolledlistbox $itsControls.viewobjlist \
					 -labeltext "View object: " -hscrollmode dynamic \
					 -selectmode browse -exportselection false \
					 -selectioncommand [itcl::code $this onViewObjSelect]
		  pack $itsControls.viewobjlist -side left -anchor nw

		  #
		  # Set up attribs
		  #

		  Toglet::defaultParent $parent
		  set itsToglet [new Toglet]
		  Toglet::width $itsToglet 600
		  Toglet::currentToglet $itsToglet

		  makeFieldControls $itsObjType

		  $itsPanes fraction 75 25

		  pack $itsPanes -side left -fill both -expand yes

		  glColor 0.0 0.0 0.0 1.0
		  glClearColor 1.0 1.0 1.0 1.0

		  glClear $::GL_COLOR_BUFFER_BIT
		  glLineWidth 1
	 }

	 public method loadObjects {filename} {
		  set ids [ObjDb::loadObjects $filename]

		  $itsControls.editobjlist clear
		  eval $itsControls.editobjlist insert end [${itsObjType}::findAll]
		  standardSettings [$itsControls.editobjlist get 0 end]

		  $itsControls.viewobjlist clear
		  eval $itsControls.viewobjlist insert end [GxNode::findAll]

		  return [llength $ids]
	 }

	 public method loadExpt {filename} {
		  Expt::load $filename

		  $itsControls.editobjlist clear
		  eval $itsControls.editobjlist insert end [${itsObjType}::findAll]
		  standardSettings [$itsControls.editobjlist get 0 end]

		  $itsControls.viewobjlist clear
		  eval $itsControls.viewobjlist insert end [GxNode::findAll]
	 }

	 public method saveObjects {filename} {
		  set objs [$itsControls.editobjlist get 0 end] 
		  ObjDb::saveObjects $filename no
		  return [llength $objs]
	 }

	 public method saveBitmaps {basename} {
		  set objs [$itsControls.editobjlist get 0 end]
		  set grobjs [dlist_select $objs [GrObj::is $objs]]
		  foreach obj $grobjs {
				GrObj::renderMode $obj $GrObj::GL_BITMAP_CACHE
				GrObj::saveBitmapCache $obj "${basename}${obj}.pbm"
				GrObj::renderMode $obj $GrObj::DIRECT_RENDER
		  }
		  return [llength $objs]
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
	 private variable itsSaveBitmapsDialog ""

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

	 public method saveBitmaps {} {
		  if { [string length $itsSaveBitmapsDialog] == 0 } {
				set itsSaveBitmapsDialog [iwidgets::fileselectiondialog \
						  ${itsFrame}.sbmapd -title "Save bitmaps" \
						  -fileson no \
						  -modality application]

				saveBitmaps
		  } else {
				if {[$itsSaveBitmapsDialog activate]} {
					 set fname [$itsSaveBitmapsDialog get]
					 set num [$itsEditor saveBitmaps $fname]
					 set ::statusInfo "Saved $num bitmaps to '$fname*'."
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
					 command saveBitmaps -label {Save bitmaps...} \
								-helpstr {Save a set of objects as bitmaps...} \
								-command {[itcl::code $this saveBitmaps]}
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
