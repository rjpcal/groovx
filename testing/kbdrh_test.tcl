##############################################################################
###
### KbdRhTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### KbdRh::kbdResponseHdlrCmd ###
test "KbdRhTcl-KbdRh::kbdResponseHdlr" "too many args" {
    KbdRh::kbdResponseHdlr junk
} {^wrong \# args: should be "KbdRh::kbdResponseHdlr"$}
test "KbdRhTcl-KbdRh::kbdResponseHdlr" "normal use" {
	 catch {KbdRh::kbdResponseHdlr}
} {^0$}

### KbdRh::useFeedbackCmd ###
RhList::reset
set rhid [KbdRh::kbdResponseHdlr]
test "KbdRhTcl-KbdRh::useFeedback" "too few args" {
	 KbdRh::useFeedback
} {^wrong \# args: should be "KbdRh::useFeedback item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::useFeedback" "too many args" {
	 KbdRh::useFeedback junk junk junk
} {^wrong \# args: should be "KbdRh::useFeedback item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use" {
	 KbdRh::useFeedback $::rhid true
	 KbdRh::useFeedback $::rhid
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use" {
	 KbdRh::useFeedback $::rhid yes
	 KbdRh::useFeedback $::rhid
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use" {
	 KbdRh::useFeedback $::rhid 1
	 KbdRh::useFeedback $::rhid
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use" {
	 KbdRh::useFeedback $::rhid -1.5
	 KbdRh::useFeedback $::rhid
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use" {
	 KbdRh::useFeedback $::rhid false
	 KbdRh::useFeedback $::rhid
} {^0$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use" {
	 KbdRh::useFeedback $::rhid no
	 KbdRh::useFeedback $::rhid
} {^0$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use" {
	 KbdRh::useFeedback $::rhid 0
	 KbdRh::useFeedback $::rhid
} {^0$}
test "KbdRhTcl-KbdRh::useFeedback" "error on non-boolean input" {
	 KbdRh::useFeedback $::rhid FLASE
} {^expected boolean value but got "FLASE"$}
test "KbdRhTcl-KbdRh::useFeedback" "error on bad rhid" {
	 KbdRh::useFeedback -1
} {^KbdRh::useFeedback: invalid response handler id$}

### KbdRh::keyRespPairs ###
test "KbdRhTcl-KbdRh::keyRespPairs" "too few args" {
	 KbdRh::keyRespPairs
} {^wrong \# args: should be "KbdRh::keyRespPairs item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::keyRespPairs" "too many args" {
	 KbdRh::keyRespPairs junk junk junk
} {^wrong \# args: should be "KbdRh::keyRespPairs item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::keyRespPairs" "error on bad rhid" {
	 KbdRh::keyRespPairs -1
} {^KbdRh::keyRespPairs: invalid response handler id$}
test "KbdRhTcl-KbdRh::keyRespPairs" "normal use" {
	 KbdRh::keyRespPairs $::rhid { {{^[aA]$} 0} {{^[lL]$} 1} }
	 KbdRh::keyRespPairs $::rhid
} {^ \{\{\^\[aA\]\$\} 0\} \{\{\^\[lL\]\$\} 1\} $}
