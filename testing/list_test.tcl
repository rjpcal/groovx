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

package require Objtogl
if { ![Togl::inited] } { Togl::init; ::update }

namespace eval List {

variable TEST_DEFINED 1

proc purgeAll {} {
	 Togl::setVisible 0
	 Expt::clear
	 IoDb::clear
}

proc testList { packagename listname baseclass subclass1 subclass2 } {
    set testObj(packagename) $packagename
    set testObj(listname) $listname
    set testObj(baseclass) $baseclass
    set testObj(subclass1) $subclass1
    set testObj(subclass2) $subclass2

	 purgeAll

    testResetCmd testObj
    testCountCmd testObj
	 testDeleteCmd testObj
	 testGetValidIdsCmd testObj
	 testIsValidIdCmd testObj
}

proc testResetCmd { objname } {
    upvar $objname this

    set cmdname "${this(baseclass)}::removeAll"
    set usage "wrong \# args: should be \"$cmdname\""
    set testname "${this(packagename)}-${cmdname}"

    eval ::test $testname {"too many args"} {"
            $cmdname junk
    "} {$usage}
    eval ::test $testname {"check number of objects"} {"
        $cmdname
        set before_count \[${this(baseclass)}::countAll\]
        Obj::new ${this(subclass1)}
        Obj::new ${this(subclass2)}
        $cmdname
        set after_count \[${this(baseclass)}::countAll\]
		  return \[expr \$before_count - \$after_count\]
    "} {"^0$"}
}

proc testCountCmd { objname } {
    upvar $objname this

    set cmdname "${this(baseclass)}::countAll"
    set usage "wrong \# args: should be \"$cmdname\""
    set testname "${this(packagename)}-${cmdname}"

    eval ::test $testname {"too many args"} {"
        $cmdname junk
    "} {$usage}
    eval ::test $testname {"normal use"} {"
	     set before_count \[$cmdname\]
        Obj::new ${this(subclass1)}
        Obj::new ${this(subclass2)}
        set after_count \[$cmdname\]
		  return \[expr \$after_count - \$before_count\]
    "} {"^2$"}
} 

proc testDeleteCmd { objname } {
    upvar $objname this

    set cmdname "Obj::delete"
    set usage "wrong \# args: should be \"$cmdname item_id\\(s\\)\""
    set testname "${this(packagename)}-${cmdname}"

	 eval ::test $testname {"too few args"} {"
	     $cmdname
	 "} {$usage}

	 eval ::test $testname {"too many args"} {"
	     $cmdname junk junk
	 "} {$usage}

	 eval ::test $testname {"normal use"} {"
        set id \[Obj::new ${this(subclass1)}\]
	     $cmdname \$id
	     IO::is \$id
 	 "} {"^0$"}
}

proc testGetValidIdsCmd { objname } {
    upvar $objname this

    set cmdname "${this(baseclass)}::findAll"
    set usage "wrong \# args: should be \"$cmdname\""
    set testname "${this(packagename)}-${cmdname}"

	 eval ::test $testname {"too many args"} {"
	     $cmdname junk
	 "} {$usage}

	 eval ::test $testname {"normal use on filled list"} {"
        Obj::new ${this(subclass1)}
        set remove_me \[Obj::new ${this(subclass1)}\]
        Obj::new ${this(subclass2)}
		  Obj::delete \$remove_me
		  set count \[${this(baseclass)}::countAll\]
		  set num_ids \[llength \[$cmdname\]\]
		  set removed_id \[lsearch -exact \[$cmdname\] \$remove_me\]
		  return \"\[expr \$count - \$num_ids\] \$removed_id\"
	 "} {"^0 -1$"}
}

proc testIsValidIdCmd { objname } {
    upvar $objname this

    set cmdname "IO::is"
    set usage "wrong \# args: should be \"$cmdname item_id\""
    set testname "${this(packagename)}-${cmdname}"

	 eval ::test $testname {"too few args"} {"
	     $cmdname
	 "} {$usage}

	 eval ::test $testname {"too many args"} {"
	     $cmdname 0 junk
	 "} {$usage}

	 eval ::test $testname {"normal use on valid id"} {"
        set id \[Obj::new ${this(subclass1)}\]
	     $cmdname \$id
	 "} {"^1$"}

	 eval ::test $testname {"normal use on valid id"} {"
        set id \[Obj::new ${this(subclass1)}\]
	     Obj::delete \$id
	     $cmdname \$id
	 "} {"^0$"}
}

namespace export testList

}
