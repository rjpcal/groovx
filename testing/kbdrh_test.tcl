##############################################################################
###
### KbdRhTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

source ${::TEST_DIR}/io_test.tcl

RhList::reset
set ::KBDRH [IO::new KbdResponseHdlr]
KbdRh::keyRespPairs $::KBDRH { {{^[aA]$} 0} {{^[lL]$} 1} }

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd KbdRhTcl KbdRh 1 $::KBDRH
IO::testDestringifyCmd KbdRhTcl KbdRh 1 $::KBDRH
IO::testWriteCmd KbdRhTcl KbdRh 1 $::KBDRH
IO::testReadCmd KbdRhTcl KbdRh 1 $::KBDRH

### IO::new KbdRh ###
test "KbdRhTcl-IO::new KbdRh" "too many args" {
    IO::new KbdResponseHdlr junk
} {^wrong \# args: should be "IO::new typename"$}
test "KbdRhTcl-IO::new KbdRh" "normal use" {
	 catch {IO::new KbdResponseHdlr}
} {^0$}

### KbdRh::useFeedbackCmd ###
test "KbdRhTcl-KbdRh::useFeedback" "too few args" {
	 KbdRh::useFeedback
} {^wrong \# args: should be "EventRh::useFeedback item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::useFeedback" "too many args" {
	 KbdRh::useFeedback junk junk junk
} {^wrong \# args: should be "EventRh::useFeedback item_id\(s\) \?new_value\(s\)\?"$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 1" {
	 KbdRh::useFeedback $::KBDRH true
	 KbdRh::useFeedback $::KBDRH
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 2" {
	 KbdRh::useFeedback $::KBDRH yes
	 KbdRh::useFeedback $::KBDRH
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 3" {
	 KbdRh::useFeedback $::KBDRH 1
	 KbdRh::useFeedback $::KBDRH
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 4" {
	 KbdRh::useFeedback $::KBDRH -1.5
	 KbdRh::useFeedback $::KBDRH
} {^1$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 5" {
	 KbdRh::useFeedback $::KBDRH false
	 KbdRh::useFeedback $::KBDRH
} {^0$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 6" {
	 KbdRh::useFeedback $::KBDRH no
	 KbdRh::useFeedback $::KBDRH
} {^0$}
test "KbdRhTcl-KbdRh::useFeedback" "normal use 7" {
	 KbdRh::useFeedback $::KBDRH 0
	 KbdRh::useFeedback $::KBDRH
} {^0$}
test "KbdRhTcl-KbdRh::useFeedback" "error on non-boolean input" {
	 KbdRh::useFeedback $::KBDRH FLASE
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
	 KbdRh::keyRespPairs $::KBDRH { {{^[aA]$} 0} {{^[lL]$} 1} }
	 KbdRh::keyRespPairs $::KBDRH
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
	 KbdRh::feedbackPairs $::KBDRH { {x 5} {z 9} }
	 KbdRh::feedbackPairs $::KBDRH
} {^ \{x 5\} \{z 9\} $}

