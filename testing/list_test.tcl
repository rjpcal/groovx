##############################################################################
###
### list_test.tcl
### Rob Peters
### Oct-1999
### $Id$
###
##############################################################################

# Multiple inclusion guard
if { [info exists List::TEST_DEFINED] } return;

namespace eval List {

set TEST_DEFINED 1

proc testList { packagename listname baseclass subclass1 subclass2 } {
    set testObj(packagename) $packagename
    set testObj(listname) $listname
    set testObj(baseclass) $baseclass
    set testObj(subclass1) $subclass1
    set testObj(subclass2) $subclass2

    testResetCmd testObj
    testCountCmd testObj
	 testRemoveCmd testObj
	 testGetValidIdsCmd testObj
	 testIsValidIdCmd testObj

    testStringifyCmd testObj
	 testDestringifyCmd testObj
}

proc testResetCmd { objname } {
    upvar $objname this

    set cmdname "${this(listname)}::reset"
    set usage "wrong \# args: should be \"$cmdname\""
    set testname "${this(packagename)}-${cmdname}"

    eval ::test $testname {"too many args"} {"
            $cmdname junk
    "} {$usage}
    eval ::test $testname {"check number of objects -> 0"} {"
        ${this(subclass1)}::${this(subclass1)}
        ${this(subclass2)}::${this(subclass2)}
        $cmdname
        ${this(listname)}::count
    "} {"^0$"}
    eval ::test $testname {"check first vacant -> 0"} {"
        ${this(subclass1)}::${this(subclass1)}
        ${this(subclass2)}::${this(subclass2)}
        $cmdname       
        ${this(subclass1)}::${this(subclass1)}
    "} {"^0$"}
}

proc testCountCmd { objname } {
    upvar $objname this

    set cmdname "${this(listname)}::count"
    set usage "wrong \# args: should be \"$cmdname\""
    set testname "${this(packagename)}-${cmdname}"

    eval ::test $testname {"too many args"} {"
        $cmdname junk
    "} {$usage}
    eval ::test $testname {"normal use"} {"
        ${this(listname)}::reset
        ${this(subclass1)}::${this(subclass1)}
        ${this(subclass2)}::${this(subclass2)}
        $cmdname
    "} {"^2$"}
} 

proc testRemoveCmd { objname } {
    upvar $objname this

    set cmdname "${this(listname)}::remove"
    set usage "wrong \# args: should be \"$cmdname item_id\""
    set testname "${this(packagename)}-${cmdname}"

	 eval ::test $testname {"too few args"} {"
	     $cmdname
	 "} {$usage}

	 eval ::test $testname {"too many args"} {"
	     $cmdname junk junk
	 "} {$usage}

	 eval ::test $testname {"normal use"} {"
        set id \[${this(subclass1)}::${this(subclass1)}\]
	     $cmdname \$id
	     ${this(listname)}::isValidId \$id
 	 "} {"^0$"}
}

proc testGetValidIdsCmd { objname } {
    upvar $objname this

    set cmdname "${this(listname)}::getValidIds"
    set usage "wrong \# args: should be \"$cmdname\""
    set testname "${this(packagename)}-${cmdname}"

	 eval ::test $testname {"too many args"} {"
	     $cmdname junk
	 "} {$usage}

	 eval ::test $testname {"normal use on filled list"} {"
	     ${this(listname)}::reset
        ${this(subclass1)}::${this(subclass1)}
        ${this(subclass1)}::${this(subclass1)}
        ${this(subclass2)}::${this(subclass2)}
		  ${this(listname)}::remove 1
		  $cmdname
	 "} {"^0 2$"}

	 eval ::test $testname {"normal use on empty list"} {"
	     ${this(listname)}::reset
		  $cmdname
	 "} {"^$"}
}

proc testIsValidIdCmd { objname } {
    upvar $objname this

    set cmdname "${this(listname)}::isValidId"
    set usage "wrong \# args: should be \"$cmdname item_id\""
    set testname "${this(packagename)}-${cmdname}"

	 eval ::test $testname {"too few args"} {"
	     $cmdname
	 "} {$usage}

	 eval ::test $testname {"too many args"} {"
	     $cmdname 0 junk
	 "} {$usage}

	 eval ::test $testname {"normal use on valid id"} {"
        set id \[${this(subclass1)}::${this(subclass1)}\]
	     $cmdname \$id
	 "} {"^1$"}

	 eval ::test $testname {"normal use on valid id"} {"
        set id \[${this(subclass1)}::${this(subclass1)}\]
	     ${this(listname)}::remove \$id
	     $cmdname \$id
	 "} {"^0$"}
}

proc testStringifyCmd { objname } {
    upvar $objname this

    set stringify "${this(listname)}::stringify"
    set destringify "${this(listname)}::destringify"
    set usage "wrong \# args: should be \"$stringify\""
    set testname "${this(packagename)}-${stringify}"

    eval ::test $testname {"too many args"} {"
        $stringify junk
    "} {$usage}
    eval ::test $testname {"use on empty list"} {"
        ${this(listname)}::reset
        set str \[$stringify\]
        ${this(subclass1)}::${this(subclass1)}
        ${this(subclass2)}::${this(subclass2)}
        $destringify \$str
        ${this(listname)}::count
    "} {"^0$"}
    eval ::test $testname {"use on filled list"} {"
        ${this(listname)}::reset
        set id1 \[${this(subclass1)}::${this(subclass1)}\]
        set id2 \[${this(subclass2)}::${this(subclass2)}\]
        set type1_before \[${this(baseclass)}::type \$id1\]
        set type2_before \[${this(baseclass)}::type \$id2\]
        set str \[$stringify\]
        ${this(listname)}::reset
        $destringify \$str
        set type1_after \[${this(baseclass)}::type \$id1\]
        set type2_after \[${this(baseclass)}::type \$id2\]
	     set equal1 \[string equal \$type1_before \$type1_after\]
	     set equal2 \[string equal \$type2_before \$type2_after\]
        return \"\[${this(listname)}::count\] \$equal1 \$equal2\"
    "} {"^2 1 1$"}

}

proc testDestringifyCmd { objname } {
    upvar $objname this

    set stringify "${this(listname)}::stringify"
    set destringify "${this(listname)}::destringify"
    set usage "wrong \# args: should be \"$destringify string\""
    set testname "${this(packagename)}-${destringify}"

    eval ::test $testname {"too few args"} {"
        $destringify
    "} {$usage}

    eval ::test $testname {"too many args"} {"
        $destringify junk junk
    "} {$usage}

    eval ::test $testname {"error on incomplete input"} {"
        $destringify ${this(listname)}
    "} {"${destringify}: InputError: ${this(listname)}"}

    eval ::test $testname {"error on bad input"} {"
        $destringify {this is a bunch of bs}
    "} {"${destringify}: InputError: ${this(listname)}"}
}

namespace export testList

}
