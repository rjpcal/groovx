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

    # class::remove_all
    ::test "${bc}::remove_all" "too many args" [format {
	%s::remove_all junk
    } $bc] {wrong \# args: should be}

    ::test "${bc}::remove_all" "check number of objects" [format {
	set bc %s
	${bc}::remove_all
	set before_count [${bc}::count_all]
	Obj::new %s
	Obj::new %s
	${bc}::remove_all
	set after_count [${bc}::count_all]
	return [expr $before_count - $after_count]
    } $bc $sc1 $sc2] {^0$}

    # class::count_all
    ::test ${bc}::count_all "too many args" [format {
	%s::count_all junk
    } $bc] {wrong \# args: should be}

    ::test ${bc}::count_all "normal use" [format {
	set before_count [%s::count_all]
	Obj::new %s
	Obj::new %s
	set after_count [%s::count_all]
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

    # class::find_all
    ::test ${bc}::find_all "too many args" [format {
	%s::find_all junk
    } $bc] {wrong \# args: should be}

    ::test ${bc}::find_all "normal use on filled list" [format {
	Obj::new %s
	set remove_me [Obj::new %s]
	Obj::new %s
	Obj::delete $remove_me
	set count [%s::count_all]
	set num_ids [llength [%s::find_all]]
	set removed_id [lsearch -exact [%s::find_all] $remove_me]
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
