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

    proc testClass { bc sc1 sc2 } {
        -> [Toglet::current] setVisible 0
        ObjDb::clear

	# class::removeAll
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

	# class::countAll
        eval ::test ${bc}::countAll {"too many args"} {"
            ${bc}::countAll junk
        "} {"wrong \# args: should be"}

        eval ::test ${bc}::countAll {"normal use"} {"
            set before_count \[${bc}::countAll\]
            Obj::new ${sc1}
            Obj::new ${sc2}
            set after_count \[${bc}::countAll\]
            return \[expr \$after_count - \$before_count\]
        "} {"^2$"}

	# Obj::delete
        eval ::test Obj::delete {"too few args"} {"
            Obj::delete
        "} {"wrong \# args: should be"}

        eval ::test Obj::delete {"too many args"} {"
            Obj::delete junk junk
        "} {"wrong \# args: should be"}

        eval ::test Obj::delete {"normal use"} {"
            set id \[Obj::new ${sc1}\]
            Obj::delete \$id
            IO::is \$id
         "} {"^0$"}

	# class::findAll
        eval ::test ${bc}::findAll {"too many args"} {"
            ${bc}::findAll junk
        "} {"wrong \# args: should be"}

        eval ::test ${bc}::findAll {"normal use on filled list"} {"
            Obj::new ${sc1}
            set remove_me \[Obj::new ${sc1}\]
            Obj::new ${sc2}
            Obj::delete \$remove_me
            set count \[${bc}::countAll\]
            set num_ids \[llength \[${bc}::findAll\]\]
            set removed_id \[lsearch -exact \[${bc}::findAll\] \$remove_me\]
            return \"\[expr \$count - \$num_ids\] \$removed_id\"
        "} {"^0 -1$"}

	# class::is
        eval ::test ${bc}::is {"too few args"} {"
            ${bc}::is
        "} {"wrong \# args: should be"}

        eval ::test ${bc}::is {"too many args"} {"
            ${bc}::is 0 junk
        "} {"wrong \# args: should be"}

        eval ::test ${bc}::is {"normal use on valid id"} {"
            set id \[Obj::new ${sc1}\]
            ${bc}::is \$id
        "} {"^1$"}

        eval ::test ${bc}::is {"normal use on valid id"} {"
            set id \[Obj::new ${sc1}\]
            Obj::delete \$id
            ${bc}::is \$id
        "} {"^0$"}
    }

    namespace export testClass

}

Objtest::testClass Block Block Block
Objtest::testClass GxShapeKit Face Fish
Objtest::testClass GxTransform GxTransform Jitter
Objtest::testClass ResponseHandler KbdResponseHdlr NullResponseHdlr
Objtest::testClass TimingHdlr TimingHdlr TimingHandler
Objtest::testClass Trial Trial Trial
