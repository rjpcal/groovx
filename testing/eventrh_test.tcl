##############################################################################
###
### EventRhTcl
### Rob Peters
### Nov-1999
### $Id$
###
##############################################################################

### EventRh::EventRhCmd ###
test "EventRhTcl-EventRh::EventRh" "too many args" {
    EventRh::EventRh junk
} {^wrong \# args: should be "EventRh::EventRh"$}
test "EventRhTcl-EventRh::EventRh" "normal use" {
	 catch {EventRh::EventRh}
} {^0$}

### EventRh::useFeedbackCmd ###
RhList::reset
set rhid [EventRh::EventRh]
test "EventRhTcl-EventRh::useFeedback" "too few args" {
	 EventRh::useFeedback
} {^wrong \# args: should be "EventRh::useFeedback item_id\(s\) \?new_value\(s\)\?"$}
test "EventRhTcl-EventRh::useFeedback" "too many args" {
	 EventRh::useFeedback junk junk junk
} {^wrong \# args: should be "EventRh::useFeedback item_id\(s\) \?new_value\(s\)\?"$}
test "EventRhTcl-EventRh::useFeedback" "normal use 1" {
	 EventRh::useFeedback $::rhid true
	 EventRh::useFeedback $::rhid
} {^1$}
test "EventRhTcl-EventRh::useFeedback" "normal use 2" {
	 EventRh::useFeedback $::rhid yes
	 EventRh::useFeedback $::rhid
} {^1$}
test "EventRhTcl-EventRh::useFeedback" "normal use 3" {
	 EventRh::useFeedback $::rhid 1
	 EventRh::useFeedback $::rhid
} {^1$}
test "EventRhTcl-EventRh::useFeedback" "normal use 4" {
	 EventRh::useFeedback $::rhid -1.5
	 EventRh::useFeedback $::rhid
} {^1$}
test "EventRhTcl-EventRh::useFeedback" "normal use 5" {
	 EventRh::useFeedback $::rhid false
	 EventRh::useFeedback $::rhid
} {^0$}
test "EventRhTcl-EventRh::useFeedback" "normal use 6" {
	 EventRh::useFeedback $::rhid no
	 EventRh::useFeedback $::rhid
} {^0$}
test "EventRhTcl-EventRh::useFeedback" "normal use 7" {
	 EventRh::useFeedback $::rhid 0
	 EventRh::useFeedback $::rhid
} {^0$}
test "EventRhTcl-EventRh::useFeedback" "error on non-boolean input" {
	 EventRh::useFeedback $::rhid FLASE
} {expected boolean value but got "FLASE"}
test "EventRhTcl-EventRh::useFeedback" "error on bad rhid" {
	 EventRh::useFeedback -1
} {^EventRh::useFeedback: .*$}

### EventRh::inputResponseMap ###
test "EventRhTcl-EventRh::inputResponseMap" "too few args" {
	 EventRh::inputResponseMap
} {^wrong \# args: should be "EventRh::inputResponseMap item_id\(s\) \?new_value\(s\)\?"$}
test "EventRhTcl-EventRh::inputResponseMap" "too many args" {
	 EventRh::inputResponseMap junk junk junk
} {^wrong \# args: should be "EventRh::inputResponseMap item_id\(s\) \?new_value\(s\)\?"$}
test "EventRhTcl-EventRh::inputResponseMap" "error on bad rhid" {
	 EventRh::inputResponseMap -1
} {^EventRh::inputResponseMap: .*$}
test "EventRhTcl-EventRh::inputResponseMap" "normal use" {
	 EventRh::inputResponseMap $::rhid { {{^[aA]$} 0} {{^[lL]$} 1} }
	 EventRh::inputResponseMap $::rhid
} {^ \{\{\^\[aA\]\$\} 0\} \{\{\^\[lL\]\$\} 1\} $}

### EventRh::feedbackMap ###
test "EventRhTcl-EventRh::feedbackMap" "too few args" {
	 EventRh::feedbackMap
} {^wrong \# args: should be "EventRh::feedbackMap item_id\(s\) \?new_value\(s\)\?"$}
test "EventRhTcl-EventRh::feedbackMap" "too many args" {
	 EventRh::feedbackMap junk junk junk
} {^wrong \# args: should be "EventRh::feedbackMap item_id\(s\) \?new_value\(s\)\?"$}
test "EventRhTcl-EventRh::feedbackMap" "error on bad rhid" {
	 EventRh::feedbackMap -1
} {^EventRh::feedbackMap: .*$}
test "EventRhTcl-EventRh::feedbackMap" "normal use" {
	 EventRh::feedbackMap $::rhid { {x 5} {z 9} }
	 EventRh::feedbackMap $::rhid
} {^ \{x 5\} \{z 9\} $}

