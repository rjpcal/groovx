##############################################################################
###
### TlistTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Face
package require Pos
package require Tlist
package require Objtogl

source ${::TEST_DIR}/list_test.tcl
List::testList TlistTcl Tlist Trial Trial Trial

if { ![Togl::inited] } { Togl::init "-rgba false"; update }

### Tlist::loadObjidFileCmd ###
test "TlistTcl-Tlist::loadObjidFile" "too few args" {
    Tlist::loadObjidFile
} {wrong \# args: should be "Tlist::loadObjidFile\
		  objid_file num_lines \?offset\?"}
test "TlistTcl-Tlist::loadObjidFile" "too many args" {
    Tlist::loadObjidFile j u n k
} {wrong \# args: should be "Tlist::loadObjidFile\
		  objid_file num_lines \?offset\?"}
test "TlistTcl-Tlist::loadObjidFile" "normal read with no offset" {
	 ObjList::reset
	 PosList::reset
	 Tlist::loadObjidFile $::TEST_DIR/objid_file -1 0
	 Tlist::count
} {^3$} $skip_known_bug
test "TlistTcl-Tlist::loadObjidFile" "read with fixed # lines, and offset" {
	 ObjList::reset
	 PosList::reset
	 Tlist::loadObjidFile $::TEST_DIR/objid_file 2 1
	 Tlist::count
} {^2$} $skip_known_bug
test "TlistTcl-Tlist::loadObjidFile" "read empty file" {
	 ObjList::reset
	 PosList::reset
	 Tlist::loadObjidFile $::TEST_DIR/empty_file -1 0
	 Tlist::stringify
	 Tlist::count
} {^0$} $skip_known_bug
test "TlistTcl-Tlist::loadObjidFile" "error on junk text file" {
	 Tlist::loadObjidFile $::TEST_DIR/junk_text_file -1 0
} {Tlist::loadObjidFile: IO::InputError: Trial} $skip_known_bug
test "TlistTcl-Tlist::loadObjidFile" "error on junk binary file" {
	 Tlist::loadObjidFile $::TEST_DIR/junk_bin_file -1 0
} {Tlist::loadObjidFile: IO::InputError: Trial} $skip_known_bug

#  [ expr [string equal $env(ARCH) "irix6"] ? $skip_known_bug : $normal_test]


### Tlist::dealSinglesCmd ###
test "TlistTcl-Tlist::dealSingles" "too few args" {
    Tlist::dealSingles j
} {wrong \# args: should be "Tlist::dealSingles objid\(s\) posid"}
test "TlistTcl-Tlist::dealSingles" "too many args" {
    Tlist::dealSingles j u n
} {wrong \# args: should be "Tlist::dealSingles objid\(s\) posid"}
test "TlistTcl-Tlist::dealSingles" "normal use with several GrObj's" {
	 set f1 [IO::new Face]
	 set f2 [IO::new Face]
	 set f3 [IO::new Face]
	 set p [IO::new Position]
	 set trials [Tlist::dealSingles "$f1 $f2 $f3" $p]
	 llength $trials
} {^3$}
test "TlistTcl-Tlist::dealSingles" "normal use with empty ObjList" {
	 set p [IO::new Position]
	 set trials [Tlist::dealSingles "" $p]
	 llength $trials
} {^0$}

### Tlist::dealPairsCmd ###
test "TlistTcl-Tlist::dealPairs" "too few args" {
    Tlist::dealPairs j u n
} {wrong \# args: should be "Tlist::dealPairs objids1 objids2 posid1 posid2"}
test "TlistTcl-Tlist::dealPairs" "too many args" {
    Tlist::dealPairs j u n k y
} {wrong \# args: should be "Tlist::dealPairs objids1 objids2 posid1 posid2"}
test "TlistTcl-Tlist::dealPairs" "normal use on two GrObj's" {
	 set o1 [IO::new Face]
	 set o2 [IO::new Face]
	 set p1 [IO::new Position]
	 set p2 [IO::new Position]
	 set trials [Tlist::dealPairs "$o1 $o2" "$o1 $o2" $p1 $p2]
	 llength $trials
} {^4$}
test "TlistTcl-Tlist::dealPairs" "normal use with empty objids" {
	 set p1 [IO::new Position]
	 set p2 [IO::new Position]
	 set trials [Tlist::dealPairs "" "" $p1 $p2]
	 llength $trials
} {^0$}

### Tlist::dealTriadsCmd ###
test "TlistTcl-Tlist::dealTriads" "too few args" {
    Tlist::dealTriads a b c
} {wrong \# args: should be "Tlist::dealTriads objids posid1 posid2 posid3"}
test "TlistTcl-Tlist::dealTriads" "too many args" {
    Tlist::dealTriads a b c d e
} {wrong \# args: should be "Tlist::dealTriads objids posid1 posid2 posid3"}
test "TlistTcl-Tlist::dealTriads" "normal use on three GrObj's" {
	 set objs "[IO::new Face] [IO::new Face] [IO::new Face]"
	 set p1 [IO::new Position]
	 set p2 [IO::new Position]
	 set p3 [IO::new Position]
	 set trials [Tlist::dealTriads $objs $p1 $p2 $p3]
	 llength $trials
} {^18$}
test "TlistTcl-Tlist::dealTriads" "normal use on two GrObj's" {
	 set objs "[IO::new Face] [IO::new Face]"
	 set p1 [IO::new Position]
	 set p2 [IO::new Position]
	 set p3 [IO::new Position]
	 set trials [Tlist::dealTriads $objs $p1 $p2 $p3]
	 llength $trials
} {^0$}
test "TlistTcl-Tlist::dealTriads" "normal use on empty ObjList" {
	 set p1 [IO::new Position]
	 set trials [Tlist::dealTriads "" $p1 $p1 $p1]
	 llength $trials
} {^0$}

### Tlist::write_responsesCmd ###
test "TlistTcl-Tlist::write_responses" "too few args" {
    Tlist::write_responses
} {wrong \# args: should be "Tlist::write_responses filename"}
test "TlistTcl-Tlist::write_responses" "too many args" {
    Tlist::write_responses j u
} {wrong \# args: should be "Tlist::write_responses filename"}
