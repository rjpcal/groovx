##############################################################################
###
### TlistTcl
### Rob Peters
### Jul-1999
###
##############################################################################

package require Face
package require Gxtransform
package require Tlist
package require Toglet

### Tlist::loadObjidFileCmd ###
test "TlistTcl-Tlist::loadObjidFile" "too few args" {
    Tlist::loadObjidFile
} {wrong \# args: should be}
test "TlistTcl-Tlist::loadObjidFile" "too many args" {
    Tlist::loadObjidFile j u n k y
} {wrong \# args: should be}
test "TlistTcl-Tlist::loadObjidFile" "normal read" {
    set objids [newarr Face 11]
    set posids [newarr GxTransform 4]
    set trials [Tlist::loadObjidFile $::TEST_DIR/objid_file $objids $posids -1]
    objectdb::clear
    llength $trials
} {^3$}
test "TlistTcl-Tlist::loadObjidFile" "read with fixed # lines" {
    set objids [newarr Face 11]
    set posids [newarr GxTransform 4]
    set trials [Tlist::loadObjidFile $::TEST_DIR/objid_file $objids $posids 2]
    objectdb::clear
    llength $trials
} {^2$}
test "TlistTcl-Tlist::loadObjidFile" "read empty file" {
    set objids [newarr Face 11]
    set posids [newarr GxTransform 4]
    set trials [Tlist::loadObjidFile $::TEST_DIR/empty_file $objids $posids -1]
    objectdb::clear
    llength $trials
} {^0$}
test "TlistTcl-Tlist::loadObjidFile" "error on junk text file" {
    set o [newarr Face 11]
    set p [newarr GxTransform 4]
    set trials [Tlist::loadObjidFile $::TEST_DIR/junk_text_file $o $p -1]
    objectdb::clear
    llength $trials
} {Tlist::loadObjidFile:}
test "TlistTcl-Tlist::loadObjidFile" "error on junk binary file" {
    set o [newarr Face 11]
    set p [newarr GxTransform 4]
    set before [Trial::count_all]
    catch {Tlist::loadObjidFile $::TEST_DIR/junk_bin_file $o $p -1} trials
    set result "[llength $trials] [expr [Trial::count_all]-$before]"
    objectdb::clear
    return $result
} {^0 0$}


### Tlist::dealSinglesCmd ###
test "TlistTcl-Tlist::dealSingles" "too few args" {
    Tlist::dealSingles j
} {wrong \# args: should be}
test "TlistTcl-Tlist::dealSingles" "too many args" {
    Tlist::dealSingles j u n
} {wrong \# args: should be}
test "TlistTcl-Tlist::dealSingles" "normal use with several GxShapeKit's" {
    set f1 [Obj::new Face]
    set f2 [Obj::new Face]
    set f3 [Obj::new Face]
    set p [Obj::new GxTransform]
    set trials [Tlist::dealSingles "$f1 $f2 $f3" $p]
    llength $trials
} {^3$}
test "TlistTcl-Tlist::dealSingles" "normal use with empty ObjList" {
    set p [Obj::new GxTransform]
    set trials [Tlist::dealSingles "" $p]
    llength $trials
} {^0$}

### Tlist::dealPairsCmd ###
test "TlistTcl-Tlist::dealPairs" "too few args" {
    Tlist::dealPairs j u n
} {wrong \# args: should be}
test "TlistTcl-Tlist::dealPairs" "too many args" {
    Tlist::dealPairs j u n k y
} {wrong \# args: should be}
test "TlistTcl-Tlist::dealPairs" "normal use on two GxShapeKit's" {
    set o1 [Obj::new Face]
    set o2 [Obj::new Face]
    set p1 [Obj::new GxTransform]
    set p2 [Obj::new GxTransform]
    set trials [Tlist::dealPairs "$o1 $o2" "$o1 $o2" $p1 $p2]
    llength $trials
} {^4$}
test "TlistTcl-Tlist::dealPairs" "normal use with empty objids" {
    set p1 [Obj::new GxTransform]
    set p2 [Obj::new GxTransform]
    set trials [Tlist::dealPairs "" "" $p1 $p2]
    llength $trials
} {^0$}

### Tlist::dealTriadsCmd ###
test "TlistTcl-Tlist::dealTriads" "too few args" {
    Tlist::dealTriads a b c
} {wrong \# args: should be}
test "TlistTcl-Tlist::dealTriads" "too many args" {
    Tlist::dealTriads a b c d e
} {wrong \# args: should be}
test "TlistTcl-Tlist::dealTriads" "normal use on three GxShapeKit's" {
    set objs "[Obj::new Face] [Obj::new Face] [Obj::new Face]"
    set p1 [Obj::new GxTransform]
    set p2 [Obj::new GxTransform]
    set p3 [Obj::new GxTransform]
    set trials [Tlist::dealTriads $objs $p1 $p2 $p3]
    llength $trials
} {^18$}
test "TlistTcl-Tlist::dealTriads" "normal use on two GxShapeKit's" {
    set objs "[Obj::new Face] [Obj::new Face]"
    set p1 [Obj::new GxTransform]
    set p2 [Obj::new GxTransform]
    set p3 [Obj::new GxTransform]
    set trials [Tlist::dealTriads $objs $p1 $p2 $p3]
    llength $trials
} {^0$}
test "TlistTcl-Tlist::dealTriads" "normal use on empty ObjList" {
    set p1 [Obj::new GxTransform]
    set trials [Tlist::dealTriads "" $p1 $p1 $p1]
    llength $trials
} {^0$}

### Tlist::write_responsesCmd ###
test "TlistTcl-Tlist::write_responses" "too few args" {
    Tlist::write_responses
} {wrong \# args: should be}
test "TlistTcl-Tlist::write_responses" "too many args" {
    Tlist::write_responses j u
} {wrong \# args: should be}
