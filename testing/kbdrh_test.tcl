##############################################################################
###
### KbdRhTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

set ::KBDRH [Obj::new KbdResponseHdlr]
KbdRh::keyRespPairs $::KBDRH { {{^[aA]$} 0} {{^[lL]$} 1} }

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd KbdRhTcl IO 1 $::KBDRH
IO::testDestringifyCmd KbdRhTcl IO 1 $::KBDRH
IO::testWriteCmd KbdRhTcl IO 1 $::KBDRH
IO::testReadCmd KbdRhTcl IO 1 $::KBDRH

### Obj::new KbdRh ###
test "KbdRhTcl-Obj::new KbdRh" "too many args" {
    Obj::new KbdResponseHdlr junk junk
} {^wrong \# args: should be}
test "KbdRhTcl-Obj::new KbdRh" "normal use" {
	 catch {Obj::new KbdResponseHdlr}
} {^0$}

### KbdRh::useFeedbackCmd ###
test "KbdRhTcl-KbdRh::useFeedback" "too few args" {
	 KbdRh::useFeedback
} {^wrong \# args: should be}
test "KbdRhTcl-KbdRh::useFeedback" "too many args" {
	 KbdRh::useFeedback junk junk junk
} {^wrong \# args: should be}
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
test "KbdRhTcl-KbdRh::keyRespPairs" "normal use" {
	 KbdRh::keyRespPairs $::KBDRH { {{^[aA]$} 0} {{^[lL]$} 1} }
	 EventRh::responseProc $::KBDRH
} {\{\{\^\[aA\]\$\} 0\} \{\{\^\[lL\]\$\} 1\}}

### KbdRh::feedbackPairs ###
test "KbdRhTcl-KbdRh::feedbackPairs" "too few args" {
	 KbdRh::feedbackPairs
} {^wrong \# args: should be}
test "KbdRhTcl-KbdRh::feedbackPairs" "too many args" {
	 KbdRh::feedbackPairs junk junk junk
} {^wrong \# args: should be}
test "KbdRhTcl-KbdRh::feedbackPairs" "error on bad rhid" {
	 KbdRh::feedbackPairs -1
} {^EventRh::feedbackMap: .*$}
test "KbdRhTcl-KbdRh::feedbackPairs" "normal use" {
	 KbdRh::feedbackPairs $::KBDRH { {x 5} {z 9} }
	 KbdRh::feedbackPairs $::KBDRH
} {^ \{x 5\} \{z 9\} $}

