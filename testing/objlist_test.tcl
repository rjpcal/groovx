##############################################################################
###
### ObjlistTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################
	 
source ${::TEST_DIR}/list_test.tcl
List::testList ObjlistTcl ObjList GrObj Face Fish

### ObjList::loadObjectsCmd ###
test "ObjList-ObjList::loadObjects" "too few args" { 
	 ObjList::loadObjects
} {wrong \# args: should be "ObjList::loadObjects\
		  filename \?num_to_read=-1\?"}
test "ObjList-ObjList::loadObjects" "too many args" { 
	 ObjList::loadObjects j u n k y
} {wrong \# args: should be "ObjList::loadObjects\
		  filename \?num_to_read=-1\?"}
test "ObjList-ObjList::loadObjects" "normal file read with no comments" {
	 ObjList::reset
	 set objids [ObjList::loadObjects $::TEST_DIR/faces_file_no_comments]
	 set num_read [llength $objids]
	 set result "$num_read [IO::type [lindex $objids 0]]"
	 return $result
} {^10 Face$}
test "ObjList-ObjList::loadObjects" "normal file read" {
	 ObjList::reset
	 set objids [ObjList::loadObjects $::TEST_DIR/faces_file]
	 set num_read [llength $objids]
	 set result "$num_read [IO::type [lindex $objids 0]]"
	 return $result
} {^10 Face$}
test "ObjList-ObjList::loadObjects" "normal file read with limit on # to read" {
	 ObjList::reset
	 set objids [ObjList::loadObjects $::TEST_DIR/faces_file 5]
	 set num_read [llength $objids]
	 set result "$num_read [IO::type [lindex $objids 0]]"
	 return $result
} {^5 Face$}
test "ObjList-ObjList::loadObjects" "file read with virtual constructor" {
	 ObjList::reset
	 set objids [ObjList::loadObjects $::TEST_DIR/nos_faces_s50_c3]
	 return "[llength $objids] \
				[IO::type [lindex $objids 0]] [IO::type [lindex $objids 10]]"
} {^20 *Face *CloneFace$}
test "ObjList-ObjList::loadObjects" "empty file read" {
	 ObjList::reset
	 set before_count [ObjList::count]
	 set objids [ObjList::loadObjects $::TEST_DIR/empty_file]
	 set num_read [llength $objids]
	 set after_count [ObjList::count]
	 return "$num_read [expr $after_count - $before_count]"
} {^0 0$}
test "ObjList-ObjList::loadObjects" "empty file read with limit on # to read" {
	 ObjList::reset
	 set before_count [ObjList::count]
	 set objids [ObjList::loadObjects $::TEST_DIR/empty_file 5]
	 set num_read [llength $objids]
	 set after_count [ObjList::count]
	 return "$num_read [expr $after_count - $before_count]"
} {^0 0$}
# On bad input, the error may be detected by either Face or IoMgr 
# depending on which constructor got called.
test "ObjList-ObjList::loadObjects" "error on non-existent file" {
	 exec rm -rf $::TEST_DIR/nonexistent_file
	 ObjList::loadObjects $::TEST_DIR/nonexistent_file
} {^ObjList::loadObjects: unable to open file$}
test "ObjList-ObjList::loadObjects" "error from junk text file" {
	 ObjList::loadObjects $::TEST_DIR/junk_text_file
} {^ObjList::loadObjects: (IO::InputError|unable to create object of type).*$}
test "ObjList-ObjList::loadObjects" "error from junk binary file" {
	 ObjList::loadObjects $::TEST_DIR/junk_bin_file
} {ObjList::loadObjects: (IO::InputError|unable to create object of type).*$} \
  [ expr [string equal $env(ARCH) "irix6"] ? $skip_known_bug : $normal_test]
