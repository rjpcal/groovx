##############################################################################
#
# editor.tcl
#
# Copyright (c) 1999-2002 Rob Peters <rjpeters@klab.caltech.edu>
# 
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

set ::statusInfo "VisEdit started."

set ::COUNTER 0

set ::SCREENWIDTH [winfo screenwidth .]
set ::SCREENHEIGHT [winfo screenheight .]

if { $::SCREENWIDTH < 1500 || $::SCREENHEIGHT < 1100 } {
    set ::FONT {-size 10}
    set ::SCALE_WIDTH 5
    set ::PANE_FRACTION 70
} else {
    set ::FONT {-family Helvetica -size -10}
    set ::SCALE_WIDTH 10
    set ::PANE_FRACTION 70
}

set ::MAINPANE_WIDTH [expr int(0.55 * $::SCREENWIDTH)]
set ::MAINPANE_HEIGHT [expr int(0.93 * $::SCREENHEIGHT)]

set ::TOGLET_WIDTH [expr $::SCREENWIDTH - $::MAINPANE_WIDTH - 50]

proc AUTO {} { return "::object[incr ::COUNTER]" }

proc debug {msg} {
    if {0} { puts $msg }
}

proc buildScale {path label min max step callback} {
    scale $path -label $label \
	    -from $min -to $max \
	    -resolution $step \
	    -bigincrement $step \
	    -digits [string length $step] \
	    -repeatdelay 500 -repeatinterval 250 \
	    -orient horizontal \
	    -command $callback \
	    -width $::SCALE_WIDTH \
	    -font $::FONT
}

itcl::class FieldInfo {
    private variable itsName
    private variable itsMin
    private variable itsMax
    private variable itsRes
    private variable itsStartsNewGroup
    private variable isItTransient
    private variable isItString
    private variable isItMulti
    private variable isItGettable
    private variable isItSettable
    private variable isItBoolean
    private variable isItPrivate

    constructor {finfo} {
	set itsName [namespace tail [lindex $finfo 0]]
	set itsMin  [lindex $finfo 1]
	set itsMax  [lindex $finfo 2]
	set itsRes  [lindex $finfo 3]

	set flags [lindex $finfo 4]

	set itsStartsNewGroup [expr [lsearch $flags NEW_GROUP] != -1]
	set isItTransient [expr [lsearch $flags TRANSIENT] != -1]
	set isItString    [expr [lsearch $flags STRING] != -1]
	set isItMulti     [expr [lsearch $flags MULTI] != -1]
	set isItGettable  [expr [lsearch $flags NO_GET] == -1]
	set isItSettable  [expr [lsearch $flags NO_SET] == -1]
	set isItBoolean   [expr [lsearch $flags BOOLEAN] != -1]
	set isItPrivate   [expr [lsearch $flags PRIVATE] != -1]
    }

    public method name {} { return $itsName }
    public method max {} { return $itsMax }
    public method min {} { return $itsMin }
    public method res {} { return $itsRes }
    public method startsNewGroup {} { return $itsStartsNewGroup }

    public method isTransient {} { return $isItTransient }
    public method isString    {} { return $isItString }
    public method isMulti     {} { return $isItMulti }
    public method isGettable  {} { return $isItGettable }
    public method isSettable  {} { return $isItSettable }
    public method isBoolean   {} { return $isItBoolean }
    public method isPrivate   {} { return $isItPrivate }
}

itcl::class StringController {
    private variable itsWidget

    private common toBeAligned

    constructor {finfo parent command} {
	set fname [$finfo name]

	set itsWidget [iwidgets::entryfield $parent.$fname \
		-labeltext $fname -labelpos n \
		-width 15 \
		-command $command \
		-labelfont $::FONT]

	lappend toBeAligned $parent.$fname
    }

    public method setVal {val} {
	$itsWidget delete 0 end
	$itsWidget insert 0 $val
    }

    public method getVal {} {
	return [$itsWidget get]
    }

    public proc alignAll {} {
	eval iwidgets::Labeledwidget::alignlabels $toBeAligned
    }
}

itcl::class MultiController {
    private variable itsWidgets

    constructor {finfo parent command} {
	set itsWidgets [list]

	set fname [$finfo name]

	set theFrame [frame $parent.$fname -relief ridge -borderwidth 2]

	label $theFrame.label \
		-text "$fname" \
		-foreground darkgreen \
		-font $::FONT

	pack $theFrame.label -side top -anchor nw

	set i 0

	foreach min [$finfo min] max [$finfo max] res [$finfo res] {
	    set path $theFrame.multi$i
	    incr i

	    buildScale $path "" $min $max $res $command

	    pack $path -side top

	    lappend itsWidgets $path
	}

    }

    public method setVal {val} {
	foreach widg $itsWidgets v $val {
	    $widg set $v
	}
    }

    public method getVal {} {
	set val [list]
	foreach widg $itsWidgets {
	    lappend val [$widg get]
	}
	return $val
    }
}

itcl::class BooleanController {
    private variable itsWidget
    private variable itsFname

    constructor {finfo parent command} {
	set itsFname [$finfo name]

	set itsWidget [iwidgets::checkbox $parent.$itsFname \
		-borderwidth 0 \
		-labelfont $::FONT]

	$itsWidget add $itsFname -text $itsFname -command $command

	$itsWidget configure -foreground brown
    }

    public method setVal {val} {
	if { $val } {
	    $itsWidget select $itsFname
	} else {
	    $itsWidget deselect $itsFname
	}
    }

    public method getVal {} {
	return [$itsWidget get $itsFname]
    }
}

itcl::class ScaleController {
    private variable itsWidget

    constructor {finfo parent command} {
	set fname [$finfo name]
	set itsWidget [buildScale $parent.$fname $fname \
		[$finfo min] [$finfo max] [$finfo res] \
		$command]
    }

    public method setVal {val} {
	$itsWidget set $val
    }

    public method getVal {} {
	return [$itsWidget get]
    }
}

#
# FieldControlSet class definition
#

itcl::class FieldControlSet {
    private variable itsObjType
    private variable itsFieldInfos
    private variable itsFrame
    private variable itsControllers
    private variable itsCachedValues
    private variable itsCallback

    private method setControl {fname val} {
	$itsControllers($fname) setVal $val

	set itsCachedValues($fname) $val
    }

    private method onControl {fname {val {}}} {
	set val [$itsControllers($fname) getVal]

	$itsCallback $fname $val
    }

    private method init { panes objtype setCallback }

    constructor {panes objtype setCallback} { init $panes $objtype $setCallback }

    public method update {obj}

    public method setAttribForObjs {objs fname val}
}

#
# FieldControlSet body definitions
#

itcl::body FieldControlSet::init { panes objtype setCallback } {
    $panes insert 0 $objtype

    set parent [$panes childsite $objtype]

    set itsObjType $objtype

    set itsCallback $setCallback

    set itsFrame [frame $parent.fields]

    set subframe ""

    set column 0

    foreach fdata [${objtype}::allFields] {
	set finfo [FieldInfo [::AUTO] $fdata]

	if { [$finfo isPrivate] } { itcl::delete object $finfo; continue }

	set fname [$finfo name]

	set itsCachedValues($fname) 0

	set itsFieldInfos($fname) $finfo

	if { [$finfo startsNewGroup] } {
	    set subframe [frame $itsFrame.column$column]
	    incr column
	    pack $subframe -side left -fill y -expand yes
	}

	set type ScaleController

	if { [$finfo isString] } { set type StringController }
	if { [$finfo isMulti] }  { set type MultiController }
	if { [$finfo isBoolean] } { set type BooleanController }

	set itsControllers($fname) [$type [::AUTO] \
					$finfo $subframe \
					[itcl::code $this onControl $fname]]

	if { [$finfo isTransient] } {
	    $subframe.$fname configure -foreground blue
	}

	pack $subframe.$fname -side top
    }

    StringController::alignAll

    pack $itsFrame -fill y -side left		  
}

itcl::body FieldControlSet::update {obj} {
    foreach fname [array names itsFieldInfos] {
	set val ""
	if { [$itsFieldInfos($fname) isGettable] } {
	    set val [${itsObjType}::$fname $obj]
	}
	setControl $fname $val
    }
}

itcl::body FieldControlSet::setAttribForObjs {objs fname val} {
    debug "setting $objs $fname to $val"
    if { [$itsFieldInfos($fname) isSettable] } {
	set val [subst $val]
	if { $itsCachedValues($fname) != $val } {
	    ${itsObjType}::$fname $objs $val
	    set itsCachedValues($fname) $val
	}
    }
}

#
# Editor class definition
#

itcl::class Editor {
    private variable itsPanes
    private variable itsControls
    private variable itsButtons
    private variable itsToglet
    private variable itsControlSets
    private variable itsUpdateInProgress 0

    private method setObjType {type}
    private method curObjType {}
    private method standardSettings {objs}
    private method addNewObject {}

    # The edit obj list
    private method getEditAll {}
    private method getEditSelection {}
    private method setEditSelection {objs}
    private method onEditObjSelect {}

    # The view obj list
    private method getViewAll {}
    private method getViewSelection {}
    private method setViewSelection {objs}
    private method onViewObjSelect {}

    # Both lists together
    private method addListObj {obj}
    private method setListAll {objs}
    private method refreshLists {}

    # Callbacks
    private method makePreviewObj {}
    private method requestDraw {}
    private method updateControls {obj}
    private method viewingDist {val}
    private method setAttrib {fname val}
    private method showFieldControls {}
    private method runCmd {}

    private method init {parent objtype}

    constructor {parent {objtype Gabor} } { init $parent $objtype }

    public method loadObjects {filename}
    public method loadExpt {filename}
    public method saveObjects {filename}
    public method saveBitmaps {basename}
}

#
# Editor body definitions
#

itcl::body Editor::setObjType {type} { $itsButtons.objtypes select $type }

itcl::body Editor::curObjType {} { return [$itsButtons.objtypes get] }

itcl::body Editor::standardSettings {objs} {
    set grobjs [dlist::select $objs [GrObj::is $objs]]

    GrObj::alignmentMode $grobjs $GrObj::CENTER_ON_CENTER
    GrObj::scalingMode $grobjs $GrObj::MAINTAIN_ASPECT_SCALING
    GrObj::renderMode $grobjs $GrObj::DIRECT_RENDER
    GrObj::height $grobjs 1.0
}

itcl::body Editor::addNewObject {} {
    # create new object
    eval set obj [new [curObjType]]

    standardSettings $obj

    addListObj $obj

    setEditSelection $obj
    setViewSelection $obj
}

itcl::body Editor::getEditAll {} {
    set objs [list]

    foreach pair [$itsControls.editobjlist get 0 end] {
	lappend objs [lindex $pair 0]
    }

    return $objs
}

itcl::body Editor::getEditSelection {} {
    set objs [list]

    foreach pair [$itsControls.editobjlist getcurselection] {
	lappend objs [lindex $pair 0]
    }

    return $objs
}

itcl::body Editor::setEditSelection {objs} {
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

itcl::body Editor::onEditObjSelect {} {
    set objs [getEditSelection]
    if { [llength $objs] > 0 } {
	setObjType [IO::type [lindex $objs 0]]
	updateControls [lindex $objs 0]
    }
}

itcl::body Editor::getViewAll {} {
    set objs [list]

    foreach pair [$itsControls.viewobjlist get 0 end] {
	lappend objs [lindex $pair 0]
    }

    return $objs
}

itcl::body Editor::getViewSelection {} {
    set sel [$itsControls.viewobjlist getcurselection]
    if { [llength $sel] > 0 } {
	return [lindex [lindex $sel 0] 0]
    } else {
	return 0
    }
}

itcl::body Editor::setViewSelection {objs} {
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

itcl::body Editor::onViewObjSelect {} {
    requestDraw
}

itcl::body Editor::addListObj {obj} {
    $itsControls.editobjlist insert end "$obj [IO::type $obj]"	
    $itsControls.viewobjlist insert end "$obj [IO::type $obj]"
}

itcl::body Editor::setListAll {objs} {
    $itsControls.editobjlist clear
    $itsControls.viewobjlist clear

    set objs [lsort -integer $objs]

    foreach obj $objs {
	addListObj $obj
    }
}

itcl::body Editor::refreshLists {} {
    set editsel [getEditSelection]
    set viewsel [getViewSelection]

    set all [GxNode::findAll]

    setListAll $all

    setEditSelection $editsel
    setViewSelection $viewsel
}

itcl::body Editor::makePreviewObj {} {
    set obj [Tlist::createPreview [getEditSelection]]

    addListObj $obj
    setViewSelection $obj
}

itcl::body Editor::requestDraw {} {
    set viewobj [getViewSelection]
    if { $viewobj != 0 } {
	Toglet::see $itsToglet $viewobj
    }
}

itcl::body Editor::updateControls {obj} {
    set itsUpdateInProgress 1

    set objtype [IO::type $obj]

    set controls $itsControlSets($objtype)

    $controls update $obj

    # need to force the controls to update before we draw the object
    update

    set itsUpdateInProgress 0
}

itcl::body Editor::viewingDist {val} {
    Toglet::setViewingDistance $itsToglet $val
}

itcl::body Editor::setAttrib {fname val} {
    debug "in setAttrib..."

    set objtype [curObjType]

    debug "objtype $objtype"

    set selection [getEditSelection]

    debug "selection $selection"

    set editobjs [dlist::select $selection [${objtype}::is $selection]]

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

itcl::body Editor::showFieldControls {} {

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
	    [FieldControlSet [::AUTO] $itsPanes $objtype \
		 [itcl::code $this setAttrib]]

    }

    $itsPanes show $objtype

    $itsPanes fraction $::PANE_FRACTION [expr 100-$::PANE_FRACTION]
}

itcl::body Editor::runCmd {} {
    set cmd [$itsButtons.runcmd get]
    catch { eval $cmd } result
    set ::statusInfo "\[$cmd\] --> $result"
    $itsButtons.runcmd clear
}

itcl::body Editor::init {parent objtype } {
    set itsPanes [iwidgets::panedwindow $parent.panes \
		      -width $::MAINPANE_WIDTH -height $::MAINPANE_HEIGHT]

    $itsPanes add controls

    set itsControls [$itsPanes childsite controls]

    #
    # Set up controls
    #

    scale $itsControls.viewingdist -showvalue false \
	-from 1 -to 200 -orient horizontal \
	-width $::SCALE_WIDTH \
	-command [itcl::code $this viewingDist]
    $itsControls.viewingdist set 60
    pack $itsControls.viewingdist -side top -fill x

    set itsButtons [frame $itsControls.buttons]

    iwidgets::optionmenu $itsButtons.objtypes -labeltext "Object type:" \
	-command [itcl::code $this showFieldControls] \
	-font $::FONT
    $itsButtons.objtypes insert 0 \
	Face \
	Fish \
	Gabor \
	Gtext \
	GxCylinder \
	GxDrawStyle \
	GxColor \
	GxLighting \
	GxLine \
	GxMaterial \
	GxSeparator \
	GxSphere \
	House \
	MaskHatch \
	MorphyFace \
	Position
    $itsButtons.objtypes sort ascending
    setObjType $objtype
    pack $itsButtons.objtypes -side top -anchor nw

    button $itsButtons.new -text "New Object" -relief raised \
	-command [itcl::code $this addNewObject] \
	-font $::FONT
    pack $itsButtons.new -side top -anchor nw

    button $itsButtons.preview -text "Make Preview" -relief raised \
	-command [itcl::code $this makePreviewObj] \
	-font $::FONT
    pack $itsButtons.preview -side top -anchor nw

    button $itsButtons.redraw -text "Redraw" -relief raised \
	-command [itcl::code $this requestDraw] \
	-font $::FONT
    pack $itsButtons.redraw -side top -anchor nw

    button $itsButtons.refreshlist -text "Refresh list" -relief raised \
	-command [itcl::code $this refreshLists] \
	-font $::FONT
    pack $itsButtons.refreshlist -side top -anchor nw

    iwidgets::entryfield $itsButtons.runcmd \
	-labeltext "Run command" -labelpos n \
	-command [itcl::code $this runCmd] \
	-labelfont $::FONT
    pack $itsButtons.runcmd -side top -anchor nw

    pack $itsButtons -side left -anchor nw

    iwidgets::scrolledlistbox $itsControls.editobjlist \
	-labeltext "Edit objects:" -hscrollmode dynamic \
	-selectmode extended -exportselection false \
	-selectioncommand [itcl::code $this onEditObjSelect] \
	-labelfont $::FONT \
	-textfont $::FONT
    pack $itsControls.editobjlist -side left -anchor nw

    iwidgets::scrolledlistbox $itsControls.viewobjlist \
	-labeltext "View object: " -hscrollmode dynamic \
	-selectmode browse -exportselection false \
	-selectioncommand [itcl::code $this onViewObjSelect] \
	-labelfont $::FONT \
	-textfont $::FONT
    pack $itsControls.viewobjlist -side left -anchor nw

    #
    # Set up attribs
    #

    Toglet::defaultParent $parent
    set itsToglet [new Toglet]
    Toglet::width $itsToglet $::TOGLET_WIDTH
    Toglet::currentToglet $itsToglet

    showFieldControls

    pack $itsPanes -side left -fill both -expand yes
}

itcl::body Editor::loadObjects {filename} {
    set objs [ObjDb::loadObjects $filename]

    set all [GxNode::findAll]

    setListAll $all
    standardSettings $all

    return [llength $objs]
}

itcl::body Editor::loadExpt {filename} {
    Expt::load $filename

    set all [GxNode::findAll]

    setListAll $all

    standardSettings $all
}

itcl::body Editor::saveObjects {filename} {
    set objs [getEditSelection] 
    ObjDb::saveObjects $objs $filename no
    return [llength $objs]
}

itcl::body Editor::saveBitmaps {basename} {
    set objs [getEditSelection]
    set grobjs [dlist::select $objs [GrObj::is $objs]]
    foreach obj $grobjs {
	GrObj::renderMode $obj $GrObj::GL_BITMAP_CACHE
	GrObj::saveBitmapCache $obj "${basename}${obj}.pbm"
	GrObj::renderMode $obj $GrObj::DIRECT_RENDER
    }
    return [llength $objs]
}

#
# Menuapp class definition
#

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

	set itsHelpEntry [label .ef -textvariable statusInfo \
		-relief sunken -anchor w -foreground darkgreen \
		-font $::FONT]

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

	set itsEditor [Editor [::AUTO] $itsFrame]
    }
}

wm withdraw .

# get rid of the default widget
Toglet::destroy [Toglet::currentToglet]

set app [Menuapp [::AUTO]]

update idletasks

wm deiconify .
