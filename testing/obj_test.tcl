##############################################################################
###
### ObjTcl
### Rob Peters
### Jun-2001
### $Id$
###
##############################################################################

package require Toglet

namespace eval Objtest {

    variable TEST_DEFINED 1

    proc purgeAll {} {
        -> [Toglet::current] setVisible 0
        ObjDb::clear
    }

    proc testResetCmd { bc sc1 sc2 } {

        ::test "${bc}::removeAll" "too many args" [format {
            %s::removeAll junk
        } $bc] {wrong \# args: should be}

        ::test "${bc}::removeAll" "check number of objects" [format {
	    set bc %s
            ${bc}::removeAll
            set before_count [${bc}::countAll]
            Obj::new %s
            Obj::new %s
            ${bc}::removeAll
            set after_count [${bc}::countAll]
            return [expr $before_count - $after_count]
        } $bc $sc1 $sc2] {^0$}
    }

    proc testCountAllCmd { objname } {
        upvar $objname this

        set cmdname "${this(baseclass)}::countAll"
        set usage "wrong \# args: should be \"$cmdname\""

        eval ::test $cmdname {"too many args"} {"
            $cmdname junk
        "} {$usage}
        eval ::test $cmdname {"normal use"} {"
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
        set usage "wrong \# args: should be \"$cmdname objref\\(s\\)\""

        eval ::test $cmdname {"too few args"} {"
            $cmdname
        "} {$usage}

        eval ::test $cmdname {"too many args"} {"
            $cmdname junk junk
        "} {$usage}

        eval ::test $cmdname {"normal use"} {"
            set id \[Obj::new ${this(subclass1)}\]
            $cmdname \$id
            IO::is \$id
         "} {"^0$"}
    }

    proc testFindAllCmd { objname } {
        upvar $objname this

        set cmdname "${this(baseclass)}::findAll"
        set usage "wrong \# args: should be \"$cmdname\""

        eval ::test $cmdname {"too many args"} {"
            $cmdname junk
        "} {$usage}

        eval ::test $cmdname {"normal use on filled list"} {"
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

    proc testIsCmd { objname } {
        upvar $objname this

        set cmdname "${this(baseclass)}::is"
        set usage "wrong \# args: should be "

        eval ::test $cmdname {"too few args"} {"
            $cmdname
        "} {$usage}

        eval ::test $cmdname {"too many args"} {"
            $cmdname 0 junk
        "} {$usage}

        eval ::test $cmdname {"normal use on valid id"} {"
            set id \[Obj::new ${this(subclass1)}\]
            $cmdname \$id
        "} {"^1$"}

        eval ::test $cmdname {"normal use on valid id"} {"
            set id \[Obj::new ${this(subclass1)}\]
            Obj::delete \$id
            $cmdname \$id
        "} {"^0$"}
    }

    proc testClass { baseclass subclass1 subclass2 } {
        set testObj(packagename) Obj
        set testObj(baseclass) $baseclass
        set testObj(subclass1) $subclass1
        set testObj(subclass2) $subclass2

        purgeAll

        testResetCmd $baseclass $subclass1 $subclass2
        testCountAllCmd testObj
        testDeleteCmd testObj
        testFindAllCmd testObj
        testIsCmd testObj
    }

    namespace export testClass

}

Objtest::testClass Block Block Block
Objtest::testClass GxShapeKit Face Fish
Objtest::testClass GxTransform GxTransform Jitter
Objtest::testClass ResponseHandler KbdResponseHdlr NullResponseHdlr
Objtest::testClass TimingHdlr TimingHdlr TimingHandler
Objtest::testClass Trial Trial Trial
