##############################################################################
###
### ObjDb
### Rob Peters
### Dec-2000
### $Id$
###
##############################################################################

### ObjDb::loadObjectsCmd ###
test "ObjDb-ObjDb::loadObjects" "too few args" { 
	 ObjDb::loadObjects
} {wrong \# args: should be}
test "ObjDb-ObjDb::loadObjects" "too many args" { 
	 ObjDb::loadObjects j u n k y
} {wrong \# args: should be}
test "ObjDb-ObjDb::loadObjects" "normal file read with no comments" {
	 set objids [ObjDb::loadObjects $::TEST_DIR/faces_file_no_comments]
	 set num_read [llength $objids]
	 set result "$num_read [IO::type [lindex $objids 0]]"
	 return $result
} {^10 Face$}
test "ObjDb-ObjDb::loadObjects" "normal file read" {
	 set objids [ObjDb::loadObjects $::TEST_DIR/faces_file]
	 set num_read [llength $objids]
	 set result "$num_read [IO::type [lindex $objids 0]]"
	 return $result
} {^10 Face$}
test "ObjDb-ObjDb::loadObjects" "normal file read with limit on # to read" {
	 set objids [ObjDb::loadObjects $::TEST_DIR/faces_file 5]
	 set num_read [llength $objids]
	 set result "$num_read [IO::type [lindex $objids 0]]"
	 return $result
} {^5 Face$}
test "ObjDb-ObjDb::loadObjects" "file read with virtual constructor" {
	 set objids [ObjDb::loadObjects $::TEST_DIR/nos_faces_s50_c3]
	 return "[llength $objids] \
				[IO::type [lindex $objids 0]] [IO::type [lindex $objids 10]]"
} {^20 *Face *CloneFace$}
test "ObjDb-ObjDb::loadObjects" "empty file read" {
	 set before_count [GxShapeKit::countAll]
	 set objids [ObjDb::loadObjects $::TEST_DIR/empty_file]
	 set num_read [llength $objids]
	 set after_count [GxShapeKit::countAll]
	 return "$num_read [expr $after_count - $before_count]"
} {^0 0$}
test "ObjDb-ObjDb::loadObjects" "empty file read with limit on # to read" {
	 set before_count [GxShapeKit::countAll]
	 set objids [ObjDb::loadObjects $::TEST_DIR/empty_file 5]
	 set num_read [llength $objids]
	 set after_count [GxShapeKit::countAll]
	 return "$num_read [expr $after_count - $before_count]"
} {^0 0$}
# On bad input, the error may be detected by either Face or IoMgr 
# depending on which constructor got called.
test "ObjDb-ObjDb::loadObjects" "error on non-existent file" {
	 exec rm -rf $::TEST_DIR/nonexistent_file
	 ObjDb::loadObjects $::TEST_DIR/nonexistent_file
} {^ObjDb::loadObjects: unable to open file}
test "ObjDb-ObjDb::loadObjects" "error from junk text file" {
	 ObjDb::loadObjects $::TEST_DIR/junk_text_file
} {^ObjDb::loadObjects: }
test "ObjDb-ObjDb::loadObjects" "error from junk binary file" {
	 ObjDb::loadObjects $::TEST_DIR/junk_bin_file
} {ObjDb::loadObjects: } \
  [ expr [string equal $env(ARCH) "irix6"] ? $skip_known_bug : $normal_test]
