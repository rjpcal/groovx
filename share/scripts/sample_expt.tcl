#!/usr/bin/env groovx

######################################################################
# Sample Tcl code for psychophysics experiments. This script sets up
# and runs an experiment consisting of a sequence of free-viewing
# trials. Each trial consists of, in sequence, (1) a fixation cross,
# (2) an image, and (3) a response screen in which the user is asked
# to press either 1 or 2 indicating which of two regions they thought
# contained the most interesting point in the image.

# $Id$


######################################################################
# Make a (debug) OpenGL window for displaying stimuli during the experiment

proc setup_debug_toglet {} {

    wm geometry . 1300x1100+0+0

    return [Toglet::current]
}


######################################################################
# Make a fullscreen OpenGL window for displaying stimuli during the
# experiment
#
# WARNING: Calling this proc will pop a fullscreen window with no
# close/minimize button, so it will lock your screen, except for the
# following escape: at any time you can press
# Shift-Control-MouseButton1 and the fullscreen window will be
# minimized.

proc setup_toglet {} {

    return [new TopToglet]
}


######################################################################
# Make a response handler object that just listens for any keypress

proc make_spacebar_responsehdlr {} {

    set rh [new EventResponseHdlr]

    -> $rh bindingSubstitution "%A"
    -> $rh eventSequence <KeyPress>

    # Listen for spacebar presses, and ignore anything else
    -> $rh responseProc {k} {
	switch $k {
	    " " { return 0 }
	    default { return -3 }
	}
    }

    return $rh
}


######################################################################
# Make a trial object that displays a text message

proc make_text_trial { msg {rgba {1 1 1 1}} } {

    # Pick the color for the text message
    set col [new GxColor]
    -> $col rgba $rgba

    # Create the text object
    set text [new GxText]
    -> $text text $msg
    -> $text font {courier 28 b}

    # Create a timing-handler object that will wait indefinitely until
    # a user response is seen
    set th [new TimingHdlr]
    -> $th addStartEvent [new ClearBufferEvent {delay 50}]
    -> $th addStartEvent [new RenderEvent {delay 100}]
    -> $th addStartEvent [new SwapBuffersEvent {delay 150}]

    -> $th addResponseEvent [new ClearBufferEvent {delay 1}]
    -> $th addResponseEvent [new SwapBuffersEvent {delay 20}]
    -> $th addResponseEvent [new EndTrialEvent {delay 1000}]

    # Set up the trial object with the given text message, with the
    # given timing-handler object, and with a response-handler object
    # that just listens for spacebar-presses
    set trial [new Trial]

    -> $trial info "text message '$msg'"

    -> $trial addNode [new GxSeparator [list addChildren "$col $text"]]

    -> $trial timingHdlr $th
    -> $trial responseHdlr [make_spacebar_responsehdlr]

    return $trial
}


######################################################################
# Make graphic-node objects that represent the response screens

proc make_response_nodes {} {

    set text_1 [new Gtext {text "region (1)"}]
    set text_2 [new Gtext {text "region (2)"}]

    set response_divider \
	[new GxSeparator \
	     [list addChildren \
		  [list [new GxLine {start {0 -1000 0} stop {0 1000 0} width 1}] \
		       [new GxLine {start {-10 0 0} stop {10 0 0} width 1}]]]]

    set response_col [new GxColor {rgba {0.6 1.0 0.7 1}}]

    set angles { 0 30 60 90 120 150 }

    set response_nodes [list]

    foreach angle $angles {
	set rotation [new GxTransform [list rotationAngle $angle]]
	set node [new GxSeparator [list addChildren [list $rotation $response_col $response_divider]]]

	set text_pos_1 [new GxTransform {translation {-200 0 0} scaling {0.25 0.25 0.25}}]
	set text_pos_2 [new GxTransform {translation {200 0 0} scaling {0.25 0.25 0.25}}]

	set back_rotate [new GxTransform [list rotationAngle [expr -1 * $angle]]]

	if { $angle < 90 } {
	    -> $node addChildren \
		[new GxSeparator \
		     [list addChildren \
			  [list $text_pos_1 $back_rotate $text_1]]]
	    -> $node addChildren \
		[new GxSeparator \
		     [list addChildren \
			  [list $text_pos_2 $back_rotate $text_2]]]
	} else {
	    -> $node addChildren \
		[new GxSeparator \
		     [list addChildren \
			  [list $text_pos_2 $back_rotate $text_1]]]
	    -> $node addChildren \
		[new GxSeparator \
		     [list addChildren \
			  [list $text_pos_1 $back_rotate $text_2]]]
	}

	lappend response_nodes $node
    }

    return $response_nodes
}


######################################################################
# get a list of file names either by reading names from a file, or by
# globbing for image files in a directory

proc get_file_list { src } {

    set files [list]

    if { [file isdirectory $src] } {
	foreach ext {pgm ppm png jpg} {
	    set files [concat $files [glob -nocomplain ${src}/*.${ext}]]
	}
    } elseif { [file isfile $src] } {
	set chan [open $src r]
	set files [read $chan]
	close $chan
    } elseif { [string bytelength $src] == 0 } {
	set b [file dirname [info script]]
	if { [file isdirectory ${b}/../images] } {
	    set d ${b}/../images/
	} elseif { [file isdirectory ${b}/../share/images] } {
	    set d ${b}/../share/images/
	} else {
	    error "couldn't find images/ directory for [info script]"
	}
	set files [list \
		       ${d}nga01.jpg ${d}nga02.jpg ${d}nga03.jpg ${d}nga04.jpg \
		       ${d}nga05.jpg ${d}nga06.jpg ${d}nga07.jpg ${d}nga08.jpg \
		       ${d}nga09.jpg ${d}nga10.jpg]
    } else {
	error "couldn't get image files from $src"
    }

    Log::log "found [llength $files] images in '$src'"

    return $files
}


######################################################################
# make a set of free-viewing trial objects, with one trial for each of
# the images in the given list of image files; for each trial use one
# of the response screens in the given list of response nodes

proc make_trials { image_files response_nodes } {

    set trials [list]

    set fixptnode [new GxSeparator \
		       [list addChildren \
			    [list [new GxColor {rgba {0.6 0.6 0.8 1.0}}] \
				 [new FixPt {length 20 width 2}]]]]

    set c 0

    foreach image_file $image_files {
	set bmap [new GxPixmap]
	-> $bmap queueImage $image_file
	-> $bmap purgeable 1

	# change this to "if {1}" to get some debug information
	if {0} {
	    set firstnode [new GxSeparator \
			       [list addChildren \
				    [list $fixptnode \
					 [new GxTransform {translation {0 -50 0}}] \
					 [new GxColor {rgba {0.7 0.7 0.7 1.0}}] \
					 [new GxText "text $image_file font {helvetica 34}"] ]]]
	}

	set trial [new Trial]
	-> $trial info "image $image_file"
	-> $trial addNode $fixptnode
	#-> $trial addNode $firstnode
	-> $trial addNode $bmap
	-> $trial addNode [lindex $response_nodes [expr $c % [llength $response_nodes]]]

	incr c

	lappend trials $trial
    }

    return $trials
}


######################################################################
# make a response-handler object that just listens for keypresses of
# numerals either 1 or 2 -- and translate those into corresponding
# response codes

proc make_responsehdlr {} {

    set rh [new EventResponseHdlr]

    -> $rh bindingSubstitution "%A"
    -> $rh eventSequence <KeyPress>
    -> $rh responseProc {k} {
	switch $k {
	    1 {return 1}
	    2 {return 2}
	    default {return -3}
	}
    }

    return $rh
}


######################################################################
# make a timing-handler object for the main free-viewing trials

proc make_main_timinghdlr { image_duration } {

    set th [new TimingHdlr]

    -> $th addStartEvent [new DenyResponsesEvent "delay 1"]
    -> $th addStartEvent [new RenderBackEvent "delay 25"]

    # here's the fixation cross:
    -> $th addStartEvent [new RenderEvent "delay 50"]
    set sb [new MultiEvent "delay 100"]
    -> $sb addEvents [list \
			  [new SwapBuffersEvent] \
			  [new FinishDrawingEvent]]
    -> $th addStartEvent $sb

    # here's the image:
    -> $th addStartEvent [new NextNodeEvent "delay 300"]
    -> $th addStartEvent [new ClearBufferEvent "delay 500"]
    -> $th addStartEvent [new RenderEvent "delay 600"]
    set sb [new MultiEvent "delay 1000"]
    -> $sb addEvents [list \
			  [new SwapBuffersEvent] \
			  [new FinishDrawingEvent]]
    -> $th addStartEvent $sb

    # here's the response array:
    -> $th addStartEvent [new NextNodeEvent "delay 1100"]
    -> $th addStartEvent [new ClearBufferEvent "delay 1200"]
    -> $th addStartEvent [new RenderEvent "delay 1300"]
    set sb [new MultiEvent "delay [expr $image_duration + 1000]"]
    -> $sb addEvents [list \
			  [new SwapBuffersEvent] \
			  [new FinishDrawingEvent] \
			  [new AllowResponsesEvent]]
    -> $th addStartEvent $sb

    set sb [new MultiEvent "delay 0"]
    -> $sb addEvents [list \
			  [new ClearBufferEvent] \
			  [new SwapBuffersEvent] \
			  [new FinishDrawingEvent]]
    -> $th addResponseEvent $sb
    -> $th addResponseEvent [new EndTrialEvent "delay 500"]

    return $th
}


######################################################################
# make a "block" object consisting of an instruction screen followed
# by a series of free-viewing trials

proc make_main_block { image_files {image_duration 3000} } {

    set response_nodes [make_response_nodes]

    set trials [make_trials $image_files $response_nodes]

    set rh [make_responsehdlr]

    -> $trials responseHdlr $rh

    set th [make_main_timinghdlr $image_duration]

    -> $trials timingHdlr $th

    set block [new Block]

    set trials [dlist::shuffle $trials [clock clicks]]

    set instructions \
"FREE-VIEWING SEQUENCE

after each image:
  press '1' if the region labeled '1' contained the most interesting feature
  press '2' if the region labeled '2' contained the most interesting feature

\[press the spacebar to continue\]"

    -> $block addElements [make_text_trial $instructions]
    -> $block addElements $trials

    return $block
}


######################################################################
# here's the main procedure

proc main { argv } {


    ### Set up the main experiment window

    set toglet [setup_debug_toglet]

    # change the previous line to use 'setup_toglet' instead to get a
    # fullscreen window with no window-manager decorations -- BUT TAKE
    # NOTE of the warning near the definition of setup_toglet above
    # regarding the escape sequence Shift-Control-MouseButton1 to exit
    # fullscreen mode.

    # So that we have 1.0 OpenGL units == 1.0 screen pixels
    -> [-> $toglet camera] pixelsPerUnit 1.0

    glClearColor 0 0 0 0
    -> $toglet clearscreen

    update


    ### Get the list of image files to be used in the experiment
    set image_files [get_file_list [lindex $argv 0]]


    ### Set up the experiment object itself
    set expt [new ExptDriver]

    -> $expt widget $toglet

    -> $expt addElement [make_main_block $image_files]

    -> $expt doWhenComplete "exit"

    -> $toglet maximize

    -> $toglet cursor {left_ptr \#555}
    -> $toglet warpPointer [-> $toglet width] [-> $toglet height]


    ### Wait until somebody presses the spacebar
    -> $expt waitStartKey <KeyPress-space>
}


######################################################################
# We need to redefine the "bgerror" proc to first minimize the current
# Toglet, in case that Toglet is a frameless top-level widget... in
# that case, the window manager wouldn't be able to bring the
# "background error" window in front of the Toglet to receive keyboard
# events, so that application would appear to have locked up.
catch {bgerror}
rename bgerror orig_bgerror

proc bgerror {args} {
    -> [Toglet::current] minimize
    eval orig_bgerror $args
}

######################################################################
# here we call the main procedure

main $argv
