##############################################################################
###
### KbdRhTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### KbdRh::KbdRhCmd ###
test "KbdRhTcl-KbdRh::KbdRh" "too many args" {
    KbdRh::KbdRh junk
} {^wrong \# args: should be "KbdRh::KbdRh"$}
test "KbdRhTcl-KbdRh::KbdRh" "normal use" {
	 catch {KbdRh::KbdRh}
} {^0$}

### KbdRh::useFeedbackCmd ###
RhList::reset
set rhid [KbdRh::KbdRh]
test "KbdRhTcl-KbdRh::useFeedback" "too few args" {
	 KbdRh::useFeedback
} {^wrong \# args: should be "EventRh::useFeedback item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::useFeedback" "too many args" {
	 KbdRh::useFeedback junk junk junk
} {^wrong \# args: should be "EventRh::useFeedback item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 1" {
	 KbdRh::useFeedback $::rhid true
	 KbdRh::useFeedback $::rhid
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 2" {
	 KbdRh::useFeedback $::rhid yes
	 KbdRh::useFeedback $::rhid
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 3" {
	 KbdRh::useFeedback $::rhid 1
	 KbdRh::useFeedback $::rhid
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 4" {
	 KbdRh::useFeedback $::rhid -1.5
	 KbdRh::useFeedback $::rhid
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 5" {
	 KbdRh::useFeedback $::rhid false
	 KbdRh::useFeedback $::rhid
} {^0$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 6" {
	 KbdRh::useFeedback $::rhid no
	 KbdRh::useFeedback $::rhid
} {^0$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 7" {
	 KbdRh::useFeedback $::rhid 0
	 KbdRh::useFeedback $::rhid
} {^0$}
test "KbdRhTcl-KbdRh::useFeedback" "error on non-boolean input" {
	 KbdRh::useFeedback $::rhid FLASE
} {expected boolean value but got "FLASE"}
test "KbdRhTcl-KbdRh::useFeedback" "error on bad rhid" {
	 KbdRh::useFeedback -1
} {^EventRh::useFeedback: .*$}

### KbdRh::keyRespPairs ###
test "KbdRhTcl-KbdRh::keyRespPairs" "too few args" {
	 KbdRh::keyRespPairs
} {^wrong \# args: should be "EventRh::inputResponseMap item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::keyRespPairs" "too many args" {
	 KbdRh::keyRespPairs junk junk junk
} {^wrong \# args: should be "EventRh::inputResponseMap item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::keyRespPairs" "error on bad rhid" {
	 KbdRh::keyRespPairs -1
} {^EventRh::inputResponseMap: .*$}
test "KbdRhTcl-KbdRh::keyRespPairs" "normal use" {
	 KbdRh::keyRespPairs $::rhid { {{^[aA]$} 0} {{^[lL]$} 1} }
	 KbdRh::keyRespPairs $::rhid
} {^ \{\{\^\[aA\]\$\} 0\} \{\{\^\[lL\]\$\} 1\} $}

### KbdRh::feedbackPairs ###
test "KbdRhTcl-KbdRh::feedbackPairs" "too few args" {
	 KbdRh::feedbackPairs
} {^wrong \# args: should be "EventRh::feedbackMap item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::feedbackPairs" "too many args" {
	 KbdRh::feedbackPairs junk junk junk
} {^wrong \# args: should be "EventRh::feedbackMap item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::feedbackPairs" "error on bad rhid" {
	 KbdRh::feedbackPairs -1
} {^EventRh::feedbackMap: .*$}
test "KbdRhTcl-KbdRh::feedbackPairs" "normal use" {
	 KbdRh::feedbackPairs $::rhid { {x 5} {z 9} }
	 KbdRh::feedbackPairs $::rhid
} {^ \{x 5\} \{z 9\} $}

