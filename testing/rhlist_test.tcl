##############################################################################
###
### RhListTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### RhList::countCmd ###
test "RhList-KbdRh::countCmd" "too many args" {
	 RhList::count junk
} {^wrong \# args: should be "RhList::count"$}
test "RhList-KbdRh::countCmd" "normal use on empty list" {
	 RhList::reset
	 RhList::count
} {^0$}
test "RhList-KbdRh::countCmd" "normal use on filled list" {
	 RhList::reset
	 KbdRh::KbdRh
	 KbdRh::KbdRh
	 RhList::count
} {^2$}

### RhList::resetCmd ###
test "RhList-KbdRh::resetCmd" "too many args" {
	 RhList::reset junk
} {^wrong \# args: should be "RhList::reset"$}

### RhList::stringifyCmd ###
### RhList::destringifyCmd ###
test "RhListTcl-RhList::stringify" "too many args" {
	 RhList::stringify junk
} {^wrong \# args: should be "RhList::stringify"$}
test "RhListTcl-RhList::destringify" "too few args" {
	 RhList::destringify
} {^wrong \# args: should be "RhList::destringify string"$}
test "RhListTcl-RhList::destringify" "too many args" {
	 RhList::destringify junk junk
} {^wrong \# args: should be "RhList::destringify string"$}
test "RhListTcl-RhList::stringify" "write, read, write and compare" {
	 RhList::reset
	 KbdRh::KbdRh
	 KbdRh::KbdRh
	 set str1 [RhList::stringify]
	 RhList::reset
	 RhList::destringify $str1
	 set str2 [RhList::stringify]
	 string equal $str1 $str2
} {^1$}


