##############################################################################
###
### IoDb
### Rob Peters
### Dec-2000
### $Id$
###
##############################################################################

### IoDb::loadObjectsCmd ###
test "IoDb-IoDb::loadObjects" "too few args" { 
	 IoDb::loadObjects
} {wrong \# args: should be "IoDb::loadObjects\
		  filename \?num_to_read=-1\?"}
test "IoDb-IoDb::loadObjects" "too many args" { 
	 IoDb::loadObjects j u n k y
} {wrong \# args: should be "IoDb::loadObjects\
		  filename \?num_to_read=-1\?"}
test "IoDb-IoDb::loadObjects" "normal file read with no comments" {
	 set objids [IoDb::loadObjects $::TEST_DIR/faces_file_no_comments]
	 set num_read [llength $objids]
	 set result "$num_read [IO::type [lindex $objids 0]]"
	 return $result
} {^10 Face$}
test "IoDb-IoDb::loadObjects" "normal file read" {
	 set objids [IoDb::loadObjects $::TEST_DIR/faces_file]
	 set num_read [llength $objids]
	 set result "$num_read [IO::type [lindex $objids 0]]"
	 return $result
} {^10 Face$}
test "IoDb-IoDb::loadObjects" "normal file read with limit on # to read" {
	 set objids [IoDb::loadObjects $::TEST_DIR/faces_file 5]
	 set num_read [llength $objids]
	 set result "$num_read [IO::type [lindex $objids 0]]"
	 return $result
} {^5 Face$}
test "IoDb-IoDb::loadObjects" "file read with virtual constructor" {
	 set objids [IoDb::loadObjects $::TEST_DIR/nos_faces_s50_c3]
	 return "[llength $objids] \
				[IO::type [lindex $objids 0]] [IO::type [lindex $objids 10]]"
} {^20 *Face *CloneFace$}
test "IoDb-IoDb::loadObjects" "empty file read" {
	 set before_count [GrObj::countAll]
	 set objids [IoDb::loadObjects $::TEST_DIR/empty_file]
	 set num_read [llength $objids]
	 set after_count [GrObj::countAll]
	 return "$num_read [expr $after_count - $before_count]"
} {^0 0$}
test "IoDb-IoDb::loadObjects" "empty file read with limit on # to read" {
	 set before_count [GrObj::countAll]
	 set objids [IoDb::loadObjects $::TEST_DIR/empty_file 5]
	 set num_read [llength $objids]
	 set after_count [GrObj::countAll]
	 return "$num_read [expr $after_count - $before_count]"
} {^0 0$}
# On bad input, the error may be detected by either Face or IoMgr 
# depending on which constructor got called.
test "IoDb-IoDb::loadObjects" "error on non-existent file" {
	 exec rm -rf $::TEST_DIR/nonexistent_file
	 IoDb::loadObjects $::TEST_DIR/nonexistent_file
} {^IoDb::loadObjects: unable to open file$}
test "IoDb-IoDb::loadObjects" "error from junk text file" {
	 IoDb::loadObjects $::TEST_DIR/junk_text_file
} {^IoDb::loadObjects: (IO::InputError|unable to create object of type).*$}
test "IoDb-IoDb::loadObjects" "error from junk binary file" {
	 IoDb::loadObjects $::TEST_DIR/junk_bin_file
} {IoDb::loadObjects: (IO::InputError|unable to create object of type).*$} \
  [ expr [string equal $env(ARCH) "irix6"] ? $skip_known_bug : $normal_test]
