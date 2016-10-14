##############################################################################
###
### EventRhTcl
### Rob Peters
### Nov-1999
###
##############################################################################

set ::EVENTRH [Obj::new EventResponseHdlr]

source ${::TEST_DIR}/io_test.tcl

::testReadWrite EventRhTcl $::EVENTRH

### Obj::new EventRh ###
test "EventRhTcl-Obj::new EventRh" "too many args" {
    Obj::new EventResponseHdlr junk junk
} {^wrong \# args: should be}
test "EventRhTcl-Obj::new EventRh" "normal use" {
	 catch {Obj::new EventResponseHdlr}
} {^0$}

### EventRh::useFeedbackCmd ###
test "EventRhTcl-EventRh::useFeedback" "too few args" {
	 EventRh::useFeedback
} {^wrong \# args: should be}
test "EventRhTcl-EventRh::useFeedback" "too many args" {
	 EventRh::useFeedback junk junk junk
} {^wrong \# args: should be}
test "EventRhTcl-EventRh::useFeedback" "normal use 1" {
	 EventRh::useFeedback $::EVENTRH true
	 EventRh::useFeedback $::EVENTRH
} {^1$}
test "EventRhTcl-EventRh::useFeedback" "normal use 2" {
	 EventRh::useFeedback $::EVENTRH yes
	 EventRh::useFeedback $::EVENTRH
} {^1$}
test "EventRhTcl-EventRh::useFeedback" "normal use 3" {
	 EventRh::useFeedback $::EVENTRH 1
	 EventRh::useFeedback $::EVENTRH
} {^1$}
test "EventRhTcl-EventRh::useFeedback" "normal use 4" {
	 EventRh::useFeedback $::EVENTRH -1.5
	 EventRh::useFeedback $::EVENTRH
} {^1$}
test "EventRhTcl-EventRh::useFeedback" "normal use 5" {
	 EventRh::useFeedback $::EVENTRH false
	 EventRh::useFeedback $::EVENTRH
} {^0$}
test "EventRhTcl-EventRh::useFeedback" "normal use 6" {
	 EventRh::useFeedback $::EVENTRH no
	 EventRh::useFeedback $::EVENTRH
} {^0$}
test "EventRhTcl-EventRh::useFeedback" "normal use 7" {
	 EventRh::useFeedback $::EVENTRH 0
	 EventRh::useFeedback $::EVENTRH
} {^0$}
test "EventRhTcl-EventRh::useFeedback" "error on non-boolean input" {
	 EventRh::useFeedback $::EVENTRH FLASE
} {expected.*but got}
test "EventRhTcl-EventRh::useFeedback" "error on bad rhid" {
	 EventRh::useFeedback -1
} {useFeedback: .*$}

### EventRh::inputResponseMap ###
test "EventRhTcl-EventRh::inputResponseMap" "too few args" {
	 EventRh::inputResponseMap
} {^wrong \# args: should be}
test "EventRhTcl-EventRh::inputResponseMap" "too many args" {
	 EventRh::inputResponseMap junk junk junk
} {^wrong \# args: should be}
test "EventRhTcl-EventRh::inputResponseMap" "normal use" {
	 EventRh::inputResponseMap $::EVENTRH { {{^[aA]$} 0} {{^[lL]$} 1} }
	 EventRh::responseProc $::EVENTRH
} {\{\{\^\[aA\]\$\} 0\} \{\{\^\[lL\]\$\} 1\}}

### EventRh::feedbackMap ###
test "EventRhTcl-EventRh::feedbackMap" "too few args" {
	 EventRh::feedbackMap
} {^wrong \# args: should be}
test "EventRhTcl-EventRh::feedbackMap" "too many args" {
	 EventRh::feedbackMap junk junk junk
} {^wrong \# args: should be}
test "EventRhTcl-EventRh::feedbackMap" "error on bad rhid" {
	 EventRh::feedbackMap -1
} {feedbackMap: .*$}
test "EventRhTcl-EventRh::feedbackMap" "normal use" {
	 EventRh::feedbackMap $::EVENTRH { {x 5} {z 9} }
	 EventRh::feedbackMap $::EVENTRH
} {^ \{x 5\} \{z 9\} $}

