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
if { ![Togl::inited] } { Togl::init "-rgba false"; ::update }

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

    testStringifyCmd testObj
	 testDestringifyCmd testObj
	 testWriteCmd testObj
	 testReadCmd testObj
}

proc testResetCmd { objname } {
    upvar $objname this

    set cmdname "${this(listname)}::reset"
    set usage "wrong \# args: should be \"$cmdname\""
    set testname "${this(packagename)}-${cmdname}"

    eval ::test $testname {"too many args"} {"
            $cmdname junk
    "} {$usage}
    eval ::test $testname {"check number of objects"} {"
        $cmdname
        set before_count \[${this(listname)}::count\]
        IO::new ${this(subclass1)}
        IO::new ${this(subclass2)}
        $cmdname
        set after_count \[${this(listname)}::count\]
		  return \[expr \$before_count - \$after_count\]
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
	     set before_count \[$cmdname\]
        IO::new ${this(subclass1)}
        IO::new ${this(subclass2)}
        set after_count \[$cmdname\]
		  return \[expr \$after_count - \$before_count\]
    "} {"^2$"}
} 

proc testDeleteCmd { objname } {
    upvar $objname this

    set cmdname "IO::delete"
    set usage "wrong \# args: should be \"$cmdname item_id\\(s\\)\""
    set testname "${this(packagename)}-${cmdname}"

	 eval ::test $testname {"too few args"} {"
	     $cmdname
	 "} {$usage}

	 eval ::test $testname {"too many args"} {"
	     $cmdname junk junk
	 "} {$usage}

	 eval ::test $testname {"normal use"} {"
        set id \[IO::new ${this(subclass1)}\]
	     $cmdname \$id
	     IO::is \$id
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
        IO::new ${this(subclass1)}
        set remove_me \[IO::new ${this(subclass1)}\]
        IO::new ${this(subclass2)}
		  IO::delete \$remove_me
		  set count \[${this(listname)}::count\]
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
        set id \[IO::new ${this(subclass1)}\]
	     $cmdname \$id
	 "} {"^1$"}

	 eval ::test $testname {"normal use on valid id"} {"
        set id \[IO::new ${this(subclass1)}\]
	     IO::delete \$id
	     $cmdname \$id
	 "} {"^0$"}
}

proc testStringifyCmd { objname } {
    upvar $objname this

	 purgeAll

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
        IO::new ${this(subclass1)}
        IO::new ${this(subclass2)}
        $destringify \$str
        ${this(listname)}::count
    "} {"^0$"}
    eval ::test $testname {"use on filled list"} {"
        ${this(listname)}::reset

        set id1 \[IO::new ${this(subclass1)}\]
        set id2 \[IO::new ${this(subclass2)}\]

	     set ids_before \[${this(listname)}::getValidIds\]
        set idx1 \[lsearch \$ids_before \$id1\]
        set idx2 \[lsearch \$ids_before \$id2\]

        set type1_before \[IO::type \$id1\]
        set type2_before \[IO::type \$id2\]

        set str \[$stringify\]

        ${this(listname)}::reset
        $destringify \$str

	     set ids_after \[${this(listname)}::getValidIds\]
        set id1_after \[lindex \$ids_after \$idx1\]
        set id2_after \[lindex \$ids_after \$idx2\]

        set type1_after \[IO::type \$id1_after\]
        set type2_after \[IO::type \$id2_after\]

	     set equal1 \[string equal \$type1_before \$type1_after\]
	     set equal2 \[string equal \$type2_before \$type2_after\]
        return \"\[${this(listname)}::count\] \$equal1 \$equal2\"
    "} {"^2 1 1$"}

}

proc testDestringifyCmd { objname } {
    upvar $objname this

	 purgeAll

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
    "} {"${destringify}: IO::InputError:"}

    eval ::test $testname {"error on bad input"} {"
        $destringify {this is a bunch of bs}
    "} {"${destringify}: IO::InputError: couldn't read typename for"}
}


proc testWriteCmd { objname } {
    upvar $objname this

	 purgeAll

    set writecmd "${this(listname)}::write"
    set readcmd "${this(listname)}::read"
    set usage "wrong \# args: should be \"$writecmd\""
    set testname "${this(packagename)}-${writecmd}"

    eval ::test $testname {"too many args"} {"
        $writecmd junk
    "} {$usage}
    eval ::test $testname {"use on empty list"} {"
        ${this(listname)}::reset
        set str \[$writecmd\]
        IO::new ${this(subclass1)}
        IO::new ${this(subclass2)}
        $readcmd \$str
        ${this(listname)}::count
    "} {"^0$"}
    eval ::test $testname {"use on filled list"} {"
        ${this(listname)}::reset

        set id1 \[IO::new ${this(subclass1)}\]
        set id2 \[IO::new ${this(subclass2)}\]

	     set ids_before \[${this(listname)}::getValidIds\]
        set idx1 \[lsearch \$ids_before \$id1\]
        set idx2 \[lsearch \$ids_before \$id2\]

        set type1_before \[IO::type \$id1\]
        set type2_before \[IO::type \$id2\]

        set str \[$writecmd\]

        ${this(listname)}::reset
        $readcmd \$str

	     set ids_after \[${this(listname)}::getValidIds\]
        set id1_after \[lindex \$ids_after \$idx1\]
        set id2_after \[lindex \$ids_after \$idx2\]

        set type1_after \[IO::type \$id1_after\]
        set type2_after \[IO::type \$id2_after\]

	     set equal1 \[string equal \$type1_before \$type1_after\]
	     set equal2 \[string equal \$type2_before \$type2_after\]
        return \"\[${this(listname)}::count\] \$equal1 \$equal2\"
    "} {"^2 1 1$"}

}

proc testReadCmd { objname } {
    upvar $objname this

	 purgeAll

    set writecmd "${this(listname)}::write"
    set readcmd "${this(listname)}::read"
    set usage "wrong \# args: should be \"$readcmd string\""
    set testname "${this(packagename)}-${readcmd}"

    eval ::test $testname {"too few args"} {"
        $readcmd
    "} {$usage}

    eval ::test $testname {"too many args"} {"
        $readcmd junk junk
    "} {$usage}

    eval ::test $testname {"error on incomplete input"} {"
        $readcmd ${this(listname)}
    "} {"${readcmd}: IO::ReadError: input failed"}

    eval ::test $testname {"error on bad input"} {"
        $readcmd {this is a bunch of bs}
    "} {"${readcmd}: IO::ReadError: input failed"}
}

namespace export testList

}
