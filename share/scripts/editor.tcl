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
#     parent.itsPanes.itsControls.objtypes
#     parent.itsPanes.itsControls.new
#     parent.itsPanes.itsControls.preview
#     parent.itsPanes.itsControls.redraw
#     parent.itsPanes.itsControls.editobjlist
#     parent.itsPanes.itsControls.viewobjlist
#   parent.itsToglet

proc debug {msg} {
	 if {0} { puts $msg }
}

itcl::class FieldControls {
	 private variable itsObjType
	 private variable itsNames
	 private variable isItTransient
	 private variable isItString
	 private variable isItMulti
	 private variable isItGettable
	 private variable isItSettable
	 private variable itsFrame
	 private variable itsControls
	 private variable itsMultiControls
	 private variable itsCachedValues

	 private method setControl {fname val} {
		  set control $itsControls($fname)
		  if { $isItString($fname) } {
				$control delete 0 end
				$control insert 0 $val
		  } elseif { $isItMulti($fname) } {
				foreach ctrl $itsMultiControls($fname) v $val {
					 $ctrl set $v
				}
		  } else {
				$control set $val
		  }
		  set itsCachedValues($fname) $val
	 }

	 private method onControl {callback fname {val {}}} {
		  if { $isItString($fname) } {
				set control $itsControls($fname)
				set val [$control get]
		  } elseif { $isItMulti($fname) } {
				set val [list]
				foreach ctrl $itsMultiControls($fname) {
					 lappend val [$ctrl get]
				}
		  }
		  $callback $fname $val
	 }

	 private method buildScale {path label min max step callback} {
		  scale $path -label $label \
					 -from $min -to $max \
					 -resolution $step \
					 -bigincrement $step \
					 -digits [string length $step] \
					 -repeatdelay 500 -repeatinterval 250 \
					 -orient horizontal \
					 -command $callback
	 }

	 constructor {panes objtype setCallback} {
		  $panes insert 0 $objtype

		  set parent [$panes childsite $objtype]

		  set itsObjType $objtype
		  set itsNames [list]

		  set itsFrame [frame $parent.fields]

		  set currentframe ""

		  set align_us [list]

		  set column 0

		  foreach finfo [${objtype}::allFields] {
				set fname [namespace tail [lindex $finfo 0]]
				set lower [lindex $finfo 1]
				set upper [lindex $finfo 2]
				set step [lindex $finfo 3]
				set flags [lindex $finfo 4]

				set is_private [expr [lsearch $flags PRIVATE] != -1]

				if {$is_private} { continue }

				set isItTransient($fname) [expr [lsearch $flags TRANSIENT] != -1]
				set isItString($fname) [expr [lsearch $flags STRING] != -1]
				set isItMulti($fname) [expr [lsearch $flags MULTI] != -1]
				set isItGettable($fname) [expr [lsearch $flags NO_GET] == -1]
				set isItSettable($fname) [expr [lsearch $flags NO_SET] == -1]
				set startsnewgroup [expr [lsearch $flags NEW_GROUP] != -1]

				if {$startsnewgroup} {
					 set currentframe [frame $itsFrame.column[incr column]]
					 pack $currentframe -side left -fill y -expand yes
				}

				lappend itsNames $fname
				set itsCachedValues($fname) 0

				set pane $currentframe

				if { $isItString($fname) } {
					 iwidgets::entryfield $pane.$fname \
								-labeltext $fname -labelpos n \
								-width 15 \
								-command [itcl::code $this onControl $setCallback $fname]
					 lappend align_us $pane.$fname

					 set itsControls($fname) $pane.$fname
				} elseif { $isItMulti($fname) } {
					 frame $pane.$fname -relief ridge -borderwidth 2

					 label $pane.$fname.label -text "$fname" -foreground darkgreen

					 pack $pane.$fname.label -side top -anchor nw

					 set itsMultiControls($fname) [list]

					 set i 0

					 foreach min $lower max $upper res $step {
						  set path $pane.$fname.multi$i
						  incr i

						  buildScale $path "" $min $max $res \
									 [itcl::code $this onControl $setCallback $fname]

						  pack $path -side top

						  lappend itsMultiControls($fname) $path
					 }
					 set itsControls($fname) $pane.$fname
				} else {

					 buildScale $pane.$fname $fname $lower $upper $step \
								[itcl::code $this onControl $setCallback $fname]
					 set itsControls($fname) $pane.$fname
				}

				if {$isItTransient($fname)} {
					 $pane.$fname configure -foreground blue
				}

				pack $pane.$fname -side top
		  }

		  eval iwidgets::Labeledwidget::alignlabels $align_us

		  pack $itsFrame -fill y -side left		  
	 }

	 public method update {obj} {
		  foreach fname $itsNames {
				set val ""
				if { $isItGettable($fname) } {
					 set val [${itsObjType}::$fname $obj]
				}
				setControl $fname $val
		  }
	 }

	 public method setAttribForObjs {objs fname val} {
		  debug "setting $objs $fname to $val"
		  if { $isItSettable($fname) } {
				set val [subst $val]
				if { $itsCachedValues($fname) != $val } {
					 ${itsObjType}::$fname $objs $val
					 set itsCachedValues($fname) $val
				}
		  }
	 }
}

itcl::class Editor {
	 private variable itsPanes
	 private variable itsControls
	 private variable itsButtons
	 private variable itsToglet
	 private variable itsControlSets
	 private variable itsUpdateInProgress 0

	 private method setObjType {type} { $itsButtons.objtypes select $type }

	 private method curObjType {} { return [$itsButtons.objtypes get] }

	 private method standardSettings {objs} {
		  set grobjs [dlist_select $objs [GrObj::is $objs]]

		  GrObj::alignmentMode $grobjs $GrObj::CENTER_ON_CENTER
		  GrObj::scalingMode $grobjs $GrObj::MAINTAIN_ASPECT_SCALING
		  GrObj::renderMode $grobjs $GrObj::DIRECT_RENDER
		  GrObj::height $grobjs 1.0
	 }

	 private method addNewObject {} {
		  # create new object
		  eval set obj [new [curObjType]]

		  standardSettings $obj

		  addListObj $obj

		  setEditSelection $obj
		  setViewSelection $obj
	 }

	 # The edit obj list

	 private method getEditAll {} {
		  set objs [list]

		  foreach pair [$itsControls.editobjlist get 0 end] {
				lappend objs [lindex $pair 0]
		  }

		  return $objs
	 }

	 private method getEditSelection {} {
		  set objs [list]

		  foreach pair [$itsControls.editobjlist getcurselection] {
				lappend objs [lindex $pair 0]
		  }

		  return $objs
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

	 private method onEditObjSelect {} {
		  set objs [getEditSelection]
		  if { [llength $objs] > 0 } {
				setObjType [IO::type [lindex $objs 0]]
				updateControls [lindex $objs 0]
		  }
	 }

	 # The view obj list

	 private method getViewAll {} {
		  set objs [list]

		  foreach pair [$itsControls.viewobjlist get 0 end] {
				lappend objs [lindex $pair 0]
		  }

		  return $objs
	 }

	 private method getViewSelection {} {
		  set sel [$itsControls.viewobjlist getcurselection]
		  if { [llength $sel] > 0 } {
				return [lindex [lindex $sel 0] 0]
		  } else {
				return 0
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

	 private method onViewObjSelect {} {
		  requestDraw
	 }


	 #
	 # Both lists together
	 #

	 private method addListObj {obj} {
		  $itsControls.editobjlist insert end "$obj [IO::type $obj]"	
		  $itsControls.viewobjlist insert end "$obj [IO::type $obj]"
	 }

	 private method setListAll {objs} {
		  $itsControls.editobjlist clear
		  $itsControls.viewobjlist clear

		  set objs [lsort -integer $objs]

		  foreach obj $objs {
				addListObj $obj
		  }
	 }

	 private method refreshLists {} {
		  set editsel [getEditSelection]
		  set viewsel [getViewSelection]

		  set all [GxNode::findAll]

		  setListAll $all

		  setEditSelection $editsel
		  setViewSelection $viewsel
	 }



	 private method makePreviewObj {} {
		  set obj [Tlist::createPreview [getEditSelection]]

		  addListObj $obj
		  setViewSelection $obj
	 }

	 private method requestDraw {} {
		  set viewobj [getViewSelection]
		  if { $viewobj != 0 } {
				Toglet::see $itsToglet $viewobj
		  }
	 }

	 private method updateControls {obj} {
		  set itsUpdateInProgress 1

		  set objtype [IO::type $obj]

		  set controls $itsControlSets($objtype)

		  $controls update $obj

		  # need to force the controls to update before we draw the object
		  update

		  set itsUpdateInProgress 0
	 }

	 private method viewingDist {val} {
		  Toglet::setViewingDistance $itsToglet $val
	 }

	 private method setAttrib {fname val} {
		  debug "in setAttrib..."

		  set objtype [curObjType]

		  debug "objtype $objtype"

		  set selection [getEditSelection]

		  debug "selection $selection"

		  set editobjs [dlist_select $selection [${objtype}::is $selection]]

		  debug "editobjs $editobjs"

		  set controls $itsControlSets($objtype)

		  debug "controls $controls"

		  debug "fname $fname val $val"

		  if { !$itsUpdateInProgress && [llength $editobjs] > 0 } {
				Toglet::allowRefresh $itsToglet 0
				$controls setAttribForObjs $editobjs $fname $val
				Toglet::allowRefresh $itsToglet 1

				updateControls [lindex $editobjs 0]
		  }
	 }

	 private method showFieldControls {} {

		  set objtype [curObjType]

		  set alltypes [$itsButtons.objtypes get 0 end]

		  foreach type $alltypes {
				if { ![string equal $objtype $type] } {
					 if { [info exists itsControlSets($type)] } {
						  $itsPanes hide $type
					 }
				}
		  }

		  if { ![info exists itsControlSets($objtype)] } {

				set itsControlSets($objtype) \
						  [FieldControls #auto $itsPanes $objtype \
						  [itcl::code $this setAttrib]]

		  }

		  $itsPanes show $objtype

		  $itsPanes fraction 75 25
	 }

	 constructor {parent {objtype Gabor} } {
		  set itsPanes [iwidgets::panedwindow $parent.panes \
					 -width 900 -height 1000]

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

		  set itsButtons [frame $itsControls.buttons]

		  iwidgets::optionmenu $itsButtons.objtypes -labeltext "Object type:" \
					 -command [itcl::code $this showFieldControls]
		  $itsButtons.objtypes insert 0 \
					 Face Fish Gabor Gtext GxDrawStyle GxColor GxSeparator \
					 House MaskHatch MorphyFace Position
		  $itsButtons.objtypes sort ascending
		  setObjType $objtype
		  pack $itsButtons.objtypes -side top -anchor nw

		  button $itsButtons.new -text "New Object" -relief raised \
					 -command [itcl::code $this addNewObject]
		  pack $itsButtons.new -side top -anchor nw

		  button $itsButtons.preview -text "Make Preview" -relief raised \
					 -command [itcl::code $this makePreviewObj]
		  pack $itsButtons.preview -side top -anchor nw

		  button $itsButtons.redraw -text "Redraw" -relief raised \
					 -command [itcl::code $this requestDraw]
		  pack $itsButtons.redraw -side top -anchor nw

		  button $itsButtons.refreshlist -text "Refresh list" -relief raised \
					 -command [itcl::code $this refreshLists]
		  pack $itsButtons.refreshlist -side top -anchor nw

		  pack $itsButtons -side left -anchor nw

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

		  showFieldControls

		  pack $itsPanes -side left -fill both -expand yes
	 }

	 public method loadObjects {filename} {
		  set objs [ObjDb::loadObjects $filename]

		  set all [GxNode::findAll]

		  setListAll $all
		  standardSettings $all

		  return [llength $objs]
	 }

	 public method loadExpt {filename} {
		  Expt::load $filename

		  set all [GxNode::findAll]

		  setListAll $all

		  standardSettings $all
	 }

	 public method saveObjects {filename} {
		  set objs [getEditAll] 
		  ObjDb::saveObjects $filename no
		  return [llength $objs]
	 }

	 public method saveBitmaps {basename} {
		  set objs [getEditAll]
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

		  set itsEditor [Editor #auto $itsFrame]
	 }
}

set app [Menuapp #auto]
