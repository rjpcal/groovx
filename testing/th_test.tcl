##############################################################################
###
### ThTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### ThList::countCmd ###
test "ThTcl-SimpleTh::countCmd" "too many args" {
	 ThList::count junk
} {^wrong \# args: should be "ThList::count"$}
test "ThTcl-SimpleTh::countCmd" "normal use on empty list" {
	 ThList::reset
	 ThList::count
} {^0$}
test "ThTcl-SimpleTh::countCmd" "normal use on filled list" {
	 ThList::reset
	 SimpleTh::timingHandler
	 SimpleTh::timingHandler
	 ThList::count
} {^2$}

### ThList::resetCmd ###
test "ThTcl-SimpleTh::resetCmd" "too many args" {
	 ThList::reset junk
} {^wrong \# args: should be "ThList::reset"$}

### SimpleTh::timingHandlerCmd ###
test "ThTcl-SimpleTh::timingHandler" "too many args" {
    SimpleTh::timingHandler junk
} {^wrong \# args: should be "SimpleTh::timingHandler"$}
test "ThTcl-SimpleTh::timingHandler" "normal use" {
	 catch {SimpleTh::timingHandler}
} {^0$}

### ThList::stringifyCmd ###
### ThList::destringifyCmd ###
test "ThTcl-ThList::stringify" "too many args" {
	 ThList::stringify junk
} {^wrong \# args: should be "ThList::stringify"$}
test "ThTcl-ThList::destringify" "too few args" {
	 ThList::destringify
} {^wrong \# args: should be "ThList::destringify string"$}
test "ThTcl-ThList::destringify" "too many args" {
	 ThList::destringify junk junk
} {^wrong \# args: should be "ThList::destringify string"$}
test "ThTcl-ThList::stringify" "write, read, write and compare" {
	 ThList::reset
	 SimpleTh::timingHandler
	 SimpleTh::timingHandler
	 set str1 [ThList::stringify]
	 ThList::reset
	 ThList::destringify $str1
	 set str2 [ThList::stringify]
	 string equal $str1 $str2
} {^1$}


