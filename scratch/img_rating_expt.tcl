#!/usr/bin/env groovx

######################################################################
# Tcl/Groovx Code for Comparison of Images
# By Daniel Cleary, for Koch Lab Rotation Project (Fall 04-05)
# Modified from Sample Experiment Tcl Script by Rob Peters
#
# The program displays individual images from a database and retrieves
# user response. Specifically, it takes a training directory and uses
# those images for initial trials, and the data is discarded.
# After the initial trial, it displays each image from the database
# 3 times in random order and stores the response from the user.
######################################################################

# Note to self: change from debug_toglet to setup_toglet
# Note to self: change image time to 1.5 seconds
# Note: a reponse of 0 indicates no response from the user

proc setup_debug_toglet {} {

    wm geometry . 1300x490+0+0

    return [Toglet::current]
}

proc setup_toglet {} {

    return [new TopToglet]
}

proc make_spacebar_responsehdlr {} {

    set rh [new EventResponseHdlr]

    -> $rh bindingSubstitution "%A"
    -> $rh eventSequence <KeyPress>

    -> $rh responseProc {k} {
        switch $k {
            " " { return 0 }
            default { return -3 }
        }
    }

    return $rh
}

proc make_text_trial { msg {rgba {1 1 1 1}} } {

    set col [new GxColor]
    -> $col rgba $rgba

    set text [new GxText]
    -> $text text $msg
    -> $text font {courier 23 b}

    set th [new TimingHdlr]
    -> $th addStartEvent [new ClearBufferEvent {delay 50}]
    -> $th addStartEvent [new RenderEvent {delay 100}]
    -> $th addStartEvent [new SwapBuffersEvent {delay 150}]

    -> $th addResponseEvent [new ClearBufferEvent {delay 1}]
    -> $th addResponseEvent [new SwapBuffersEvent {delay 20}]
    -> $th addResponseEvent [new EndTrialEvent {delay 1000}]

    set trial [new Trial]

    -> $trial info "text message '$msg'"

    -> $trial addNode [new GxSeparator [list addChildren "$col $text"]]

    -> $trial timingHdlr $th
    -> $trial responseHdlr [make_spacebar_responsehdlr]

    return $trial
}

# Deprecated code, use numbered_responses now
proc make_response_nodes {} {

    set text [new Gtext {text "How salient was the image? 1-9"}]

    set response_col [new GxColor {rgba {1 1 1 1}}]

    set response_nodes [list]

    set node [new GxSeparator [list addChildren [list $response_col $text ]]]
    lappend response_nodes $node
    return $response_nodes
}

proc numbered_responses {img_num} {
    set text [new Gtext "text {How salient was the image? 1-9\
                \n\n\nImage $img_num of 335}"]

    set response_col [new GxColor {rgba {1 1 1 1}}]

    set response_nodes [list]

    set node [new GxSeparator [list addChildren [list $response_col $text ]]]
    lappend response_nodes $node
    return $response_nodes
}


proc get_file_list { src } {

    set files [list]

    if { [file isdirectory $src] } {
        foreach ext { pgm ppm png jpg } {
            set files [concat $files [glob -nocomplain ${src}/*.${ext}]]
        }
    } elseif { [string bytelength $src] == 0 || $src == "/TRAINING"} {
        set b [file dirname [info script]]
        if { [file isdirectory ${b}/../images] } {
            set d ${b}/../images/
        } elseif { [file isdirectory ${b}/../share/images] } {
            set d ${b}/../share/images/
        } else {
            error "couldn't find images/ directory for [info script]"
        }
                foreach ext { pgm ppm png jpg } {
                set files [concat $files [glob -nocomplain $d${src}/*.${ext}]]
                }

    } else {
        error "couldn't get image files from $src"
    }

    Log::log "found [llength $files] images in '$src'"

    return $files
}

proc make_trials { image_files response_num } {

    set trials [list]
    set tri_image [list]

    for { set i 1 } { $i < 4 } { incr i } {
     for { set j 0  } { $j < [ llength $image_files ] } { incr j } {
        lappend tri_image [ lindex $image_files $j ]
        }
     }

    set image_files [dlist::shuffle $tri_image [clock clicks]]
    foreach image_file $image_files {
        set bmap [new GxPixmap]
        -> $bmap queueImage $image_file
        -> $bmap purgeable 1

        set trial [new Trial]
        -> $trial info "image: $image_file"
        -> $trial addNode $bmap
        -> $trial addNode [numbered_responses $response_num ]

        incr response_num
        lappend trials $trial
    }
    return $trials
}

proc make_training_trials { image_files } {

    set trials [list]
    set image_files [dlist::shuffle $image_files [clock clicks]]
    set response_num 1

    foreach image_file $image_files {
        set bmap [new GxPixmap]
        -> $bmap queueImage $image_file
        -> $bmap purgeable 1

        set trial [new Trial]
        -> $trial info "TRAINING IMAGE: $image_file"
        -> $trial addNode $bmap
        -> $trial addNode [numbered_responses $response_num ]
        incr response_num
        lappend trials $trial
    }

    return $trials
}


proc make_responsehdlr {} {

    set rh [new EventResponseHdlr]

    -> $rh bindingSubstitution "%A"
    -> $rh eventSequence <KeyPress>
    -> $rh responseProc {k} {
	set objs [Obj::findAll]
	Log::log "number of objects: [llength $objs]"
	Log::log "last 10 objects: [lrange $objs end-10 end]"
	Log::log "last 10 objtypes: [Obj::type [lrange $objs end-10 end]]"
	Log::log "process status:\n[exec cat /proc/[pid]/status]"
        switch $k {
            1 {return 1}
            2 {return 2}
            3 {return 3}
            4 {return 4}
            5 {return 5}
            6 {return 6}
            7 {return 7}
            8 {return 8}
            9 {return 9}
            default {
                bell
                return -3
	    }
        }
    }

    return $rh
}



proc make_main_timinghdlr { image_duration } {

    set th [new TimingHdlr]

    -> $th addStartEvent [new DenyResponsesEvent "delay 1"]
    -> $th addStartEvent [new RenderBackEvent "delay 25"]

    # start with the image
    -> $th addStartEvent [new RenderEvent "delay 50"]
    set sb [new MultiEvent "delay 100"]
    -> $sb addEvents [list \
                          [new SwapBuffersEvent] \
                          [new FinishDrawingEvent]]
    -> $th addStartEvent $sb

    -> $th addStartEvent [new NextNodeEvent "delay 300"]
    -> $th addStartEvent [new ClearBufferEvent "delay 500"]
    -> $th addStartEvent [new RenderEvent "delay 600"]

    #this is the reponse...
    set sb [new MultiEvent "delay [expr $image_duration + 600]"]

    -> $sb addEvents [list \
                          [new SwapBuffersEvent] \
                          [new FinishDrawingEvent] \
                          [new AllowResponsesEvent]]
    -> $th addStartEvent $sb

    set sb [new MultiEvent "delay 0"]
    -> $sb addEvents [list \
                          [new ClearBufferEvent] \
                          [new SwapBuffersEvent] \
                          [new FinishDrawingEvent] \
                          [new EndTrialEvent]]
    -> $th addResponseEvent $sb

    #Adds a bell and skips the response if the user times out
    set snd [new GenericEvent]
    -> $snd callback "bell"

    set end [new MultiEvent "delay 90000"]
    -> $end addEvents [list \
                            $snd \
                            [new EndTrialEvent]]
    -> $th addStartEvent $end

    return $th
}

#Deprecated procedure.
proc make_training_block {image_files {image_duration 600} } {

    set response_nodes [make_response_nodes]

    set trials [make_training_trials $image_files $response_nodes]

    set rh [make_responsehdlr]

    -> $trials responseHdlr $rh

    set th [make_main_timinghdlr $image_duration]

    -> $trials timingHdlr $th

    set block [new Block]

    #set trials [dlist::shuffle $trials [clock clicks]]

    -> $block addElements $trials

    return $block
}

# Makes a block consisting of both training images and experimental images
proc make_image_block { image_files training_files {image_duration 600} } {

    set response_nodes [make_response_nodes]

    set training [make_training_trials $training_files]

    set trials [make_trials $image_files [expr [llength $training_files] + 1]]

    set rh [make_responsehdlr]

    -> $training responseHdlr $rh
    -> $trials responseHdlr $rh

    set th [make_main_timinghdlr $image_duration]

    -> $training timingHdlr $th
    -> $trials timingHdlr $th

    set block [new Block]

    #set trials [dlist::shuffle $trials [clock clicks]]

    -> $block addElements $training
    -> $block addElements $trials

    return $block
}

proc make_instructions {} {

    set block [new Block]

    set instructions \
"LEVEL OF SALIENCE IMAGE SEQUENCE:
        Please rate the following images on how visually salient
        each image is on a scale from 1 to 9, where 1 is completely
        uninteresting and 9 is extremely noticable.

\[press the spacebar to continue\]"

    -> $block addElements [make_text_trial $instructions]

    return $block
}

proc make_separator {} {

    set block [new Block]

    set instructions \
"BREAK TIME:
        You now have a 5 minute break between image sets.

\[press the spacebar to continue\]"
                                                                                         -> $block addElements [make_text_trial $instructions]

    return $block
}


proc main { argv } {

    # Change the toglet settings based on debug/expt mode
    #set toglet [setup_debug_toglet]
    set toglet [setup_toglet]

    # Not entirely sure what this does... delete?
    -> [-> $toglet camera] pixelsPerUnit 1.0

    glClearColor 0 0 0 0
    -> $toglet clearscreen

    update

    # Based on arguments, get lists of files and training files
    set image_files1 [get_file_list [lindex $argv 0]]
    set image_files2 [get_file_list [lindex $argv 1]]
    set image_files3 [get_file_list [lindex $argv 2]]
    set training1 [get_file_list [lindex $argv 0]/TRAINING]
    set training2 [get_file_list [lindex $argv 1]/TRAINING]
    set training3 [get_file_list [lindex $argv 2]/TRAINING]
    set expt [new ExptDriver]

    -> $expt widget $toglet
    -> $expt addElement [make_instructions]
    -> $expt addElement [make_image_block $image_files1 $training1]

    if  { [lindex $argv 1] != "" } {
        -> $expt addElement [make_separator]
        -> $expt addElement [make_image_block $image_files2 $training2]
        if { [lindex $argv 2] != "" } {
            -> $expt addElement [make_separator]
            -> $expt addElement [make_image_block $image_files3 $training3]
        }
    }

    -> $expt doWhenComplete "exit"

    -> $toglet maximize

    -> $toglet cursor {left_ptr \#555}
    -> $toglet warpPointer [-> $toglet width] [-> $toglet height]

    -> $expt waitStartKey <KeyPress-space>
}


catch {bgerror}
rename bgerror orig_bgerror

proc bgerror {args} {
    -> [Toglet::current] minimize
    eval orig_bgerror $args
}

proc make_keypress {} {
    event generate [-> [Toglet::current] pathname] <KeyPress-space>
    event generate [-> [Toglet::current] pathname] <KeyPress-5>
}

proc schedule_keypress {} {
    ::make_keypress
    after 250 ::schedule_keypress
}

after 250 ::schedule_keypress

main $argv

