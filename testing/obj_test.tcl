##############################################################################
###
### ObjTcl
### Rob Peters
### Jun-2001
### $Id$
###
##############################################################################

package require Toglet

proc testClassCmds { bc sc1 sc2 } {
    -> [Toglet::current] setVisible 0
    objectdb::clear

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
    ::test ${bc}::countAll "too many args" [format {
	%s::countAll junk
    } $bc] {wrong \# args: should be}

    ::test ${bc}::countAll "normal use" [format {
	set before_count [%s::countAll]
	Obj::new %s
	Obj::new %s
	set after_count [%s::countAll]
	return [expr $after_count - $before_count]
    } $bc $sc1 $sc2 $bc] {^2$}

    # Obj::delete
    ::test Obj::delete "too few args" {
	Obj::delete
    } {wrong \# args: should be}

    ::test Obj::delete "too many args" {
	Obj::delete junk junk
    } {wrong \# args: should be}

    ::test Obj::delete "normal use" [format {
	set id [Obj::new %s]
	Obj::delete $id
	Obj::is $id
    } $sc1] {^0$}

    # class::findAll
    ::test ${bc}::findAll "too many args" [format {
	%s::findAll junk
    } $bc] {wrong \# args: should be}

    ::test ${bc}::findAll "normal use on filled list" [format {
	Obj::new %s
	set remove_me [Obj::new %s]
	Obj::new %s
	Obj::delete $remove_me
	set count [%s::countAll]
	set num_ids [llength [%s::findAll]]
	set removed_id [lsearch -exact [%s::findAll] $remove_me]
	return "[expr $count - $num_ids] $removed_id"
    } $sc1 $sc1 $sc2 $bc $bc $bc] {^0 -1$}

    # class::is
    ::test ${bc}::is "too few args" [format {
	%s::is
    } $bc] {wrong \# args: should be}

    ::test ${bc}::is "too many args" [format {
	%s::is 0 junk
    } $bc] {wrong \# args: should be}

    ::test ${bc}::is "normal use on valid id" [format {
	set id [Obj::new %s]
	%s::is $id
    } $sc1 $bc] {^1$}

    ::test ${bc}::is "normal use on valid id" [format {
	set id [Obj::new %s]
	Obj::delete $id
	%s::is $id
    } $sc1 $bc] {^0$}
}

::testClassCmds Block Block Block
::testClassCmds GxShapeKit Face Fish
::testClassCmds GxTransform GxTransform Jitter
::testClassCmds ResponseHandler KbdResponseHdlr NullResponseHdlr
::testClassCmds TimingHdlr TimingHdlr TimingHandler
::testClassCmds Trial Trial Trial
