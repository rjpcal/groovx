##############################################################################
###
### objectdb
### Rob Peters
### Dec-2000
### $Id$
###
##############################################################################

### IO::loadObjectsCmd ###
test "IO::loadObjects" "too few args" {
    IO::loadObjects
} {wrong \# args: should be}
test "IO::loadObjects" "too many args" {
    IO::loadObjects j u n k y
} {wrong \# args: should be}
test "IO::loadObjects" "normal file read with no comments" {
    set objids [IO::loadObjects $::TEST_DIR/faces_file_no_comments]
    set num_read [llength $objids]
    set result "$num_read [IO::type [lindex $objids 0]]"
    return $result
} {^10 Face$}
test "IO::loadObjects" "normal file read" {
    set objids [IO::loadObjects $::TEST_DIR/faces_file]
    set num_read [llength $objids]
    set result "$num_read [IO::type [lindex $objids 0]]"
    return $result
} {^10 Face$}
test "IO::loadObjects" "normal file read with limit on # to read" {
    set objids [IO::loadObjects $::TEST_DIR/faces_file 5]
    set num_read [llength $objids]
    set result "$num_read [IO::type [lindex $objids 0]]"
    return $result
} {^5 Face$}
test "IO::loadObjects" "file read with virtual constructor" {
    set objids [IO::loadObjects $::TEST_DIR/nos_faces_s50_c3]
    return "[llength $objids] \
            [IO::type [lindex $objids 0]] [IO::type [lindex $objids 10]]"
} {^20 *Face *CloneFace$}
test "IO::loadObjects" "empty file read" {
    set before_count [GxShapeKit::count_all]
    set objids [IO::loadObjects $::TEST_DIR/empty_file]
    set num_read [llength $objids]
    set after_count [GxShapeKit::count_all]
    return "$num_read [expr $after_count - $before_count]"
} {^0 0$}
test "IO::loadObjects" "empty file read with limit on # to read" {
    set before_count [GxShapeKit::count_all]
    set objids [IO::loadObjects $::TEST_DIR/empty_file 5]
    set num_read [llength $objids]
    set after_count [GxShapeKit::count_all]
    return "$num_read [expr $after_count - $before_count]"
} {^0 0$}
# On bad input, the error may be detected by either Face or IoMgr
# depending on which constructor got called.
test "IO::loadObjects" "error on non-existent file" {
    exec rm -rf $::TEST_DIR/nonexistent_file
    IO::loadObjects $::TEST_DIR/nonexistent_file
} {unable to open file}
test "IO::loadObjects" "error from junk text file" {
    IO::loadObjects $::TEST_DIR/junk_text_file
} {unknown object type}
test "IO::loadObjects" "error from junk binary file" {
    IO::loadObjects $::TEST_DIR/junk_bin_file
} {IO::loadObjects:}

### Obj::is ###
test "Obj::is" "don't inherit over-eagerly" {
    set f [new Face]
    set s [new GxScaler]
    set result0 [GxScaler::is $f]
    set result1 [GxScaler::is $s]
    delete $s
    delete $f
    return "$result0 $result1"
} {^0 1$}
