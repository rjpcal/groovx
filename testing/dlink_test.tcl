##############################################################################
###
### dlink_list
### Rob Peters
### 10-Sep-2001
### $Id$
###
##############################################################################

load obj/$::env(ARCH)/tests/dlinktest.tso

foreach cmd [info commands Dlinktest::test_*] {
	 if { [regsub test_ $cmd expect_ expected] } {
		  ::test "dlink_list" "$cmd" $cmd [$expected]
	 }
}
