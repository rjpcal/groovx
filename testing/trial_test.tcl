##############################################################################
###
### TrialTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Trial

set TRIAL [Trial::Trial]

### Trial::responseHdlrCmd ###
test "TrialTcl-Trial::responseHdlr" "too few args" {
	 Trial::responseHdlr
} {^wrong \# args: should be "Trial::responseHdlr item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::responseHdlr" "too many args" {
	 Trial::responseHdlr $::TRIAL 0 junk
} {^wrong \# args: should be "Trial::responseHdlr item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::responseHdlr" "normal use" {
	 Trial::responseHdlr $::TRIAL 0
	 Trial::responseHdlr $::TRIAL
} {^0$}
test "TrialTcl-Trial::responseHdlr" "error" {
	 Trial::responseHdlr $::TRIAL -1
} {^$} $skip_known_bug

### Trial::timingHdlrCmd ###
test "TrialTcl-Trial::timingHdlr" "too few args" {
	 Trial::timingHdlr
} {^wrong \# args: should be "Trial::timingHdlr item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::timingHdlr" "too many args" {
	 Trial::timingHdlr $::TRIAL 0 junk
} {^wrong \# args: should be "Trial::timingHdlr item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::timingHdlr" "normal use" {
	 Trial::timingHdlr $::TRIAL 0
	 Trial::timingHdlr $::TRIAL
} {^0$}
test "TrialTcl-Trial::timingHdlr" "error" {
	 Trial::timingHdlr $::TRIAL -1
} {^$} $skip_known_bug

### Trial::typeCmd ###
test "TrialTcl-Trial::type" "too few args" {
	 Trial::type
} {^wrong \# args: should be "Trial::type item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::type" "too many args" {
	 Trial::type $::TRIAL 0 junk 
} {^wrong \# args: should be "Trial::type item_id\(s\) \?new_value\(s\)\?"$}
test "TrialTcl-Trial::type" "normal use" {
	 Trial::type $::TRIAL 0
	 Trial::type $::TRIAL
} {^0$}

