##############################################################################
###
### dlink_list
### Rob Peters
### 10-Sep-2001
### $Id$
###
##############################################################################

package require Dlinktest

foreach cmd [info commands Dlinktest::test*] {
    ::test "dlink_list" "$cmd" $cmd {^$}
}
