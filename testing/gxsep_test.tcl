##############################################################################
###
### GxSepTcl
### Rob Peters
### 2-Nov-1999
### $Id$
###
##############################################################################

package require Gxseparator
package require Face

### Obj::new GxSeparator ###
test "GxSepTcl-Obj::new GxSeparator" "normal use" {
	 set gxsep [Obj::new GxSeparator]
	 set is_gxnode [GxNode::is $gxsep]
	 set is_gxsep [GxSeparator::is $gxsep]
	 return "$gxsep $is_gxnode $is_gxsep"
} "^${INT} 1 1$"

### GxSeparator::addChild ###
test "GxSepTcl-GxSeparator::addChild" "normal use" {
	 set gxsep [Obj::new GxSeparator]
	 set face [Obj::new Face]
	 set child_id1 [GxSeparator::addChild $gxsep $face]
	 set child_id2 [GxSeparator::addChild $gxsep $face]
	 return "$child_id1 $child_id2"
} {^0 1$}

test "GxSepTcl-GxSeparator::addChild" "try to add self" {
	 set gxsep [Obj::new GxSeparator]
	 set face [Obj::new Face]
	 GxSeparator::addChild $gxsep $face
	 set code [catch {GxSeparator::addChild $gxsep $gxsep} result]
	 return "$code $result"
} {^1 GxSeparator::addChild}

test "GxSepTcl-GxSeparator::addChild" "try to add node containing self" {
	 set gxsep1 [Obj::new GxSeparator]
	 set face [Obj::new Face]
	 GxSeparator::addChild $gxsep1 $face

	 set gxsep2 [Obj::new GxSeparator]

	 GxSeparator::addChild $gxsep2 $gxsep1

	 set gxsep3 [Obj::new GxSeparator]

	 GxSeparator::addChild $gxsep3 $gxsep2

	 set code [catch {GxSeparator::addChild $gxsep1 $gxsep3} result]
	 return "$code $result"
} {^1 GxSeparator::addChild}

### GxSeparator::contains ###
test "GxSepTcl-GxSeparator::contains" "test unrelated object" {
	 set gxsep [Obj::new GxSeparator]
	 set face [Obj::new Face]
	 GxNode::contains $gxsep $face
} {^0$}

test "GxSepTcl-GxSeparator::contains" "test self" {
	 set gxsep [Obj::new GxSeparator]
	 GxNode::contains $gxsep $gxsep
} {^1$}

test "GxSepTcl-GxSeparator::contains" "check contains containee" {
	 set gxsep1 [Obj::new GxSeparator]
	 set gxsep2 [Obj::new GxSeparator]
	 GxSeparator::addChild $gxsep1 $gxsep2

	 set 1_contains_2 [GxNode::contains $gxsep1 $gxsep2]
	 set 2_contains_1 [GxNode::contains $gxsep2 $gxsep1]

	 return "$1_contains_2 $2_contains_1"
} {^1 0$}

test "GxSepTcl-GxSeparator::contains" "check recursive contains" {
	 set gxsep1 [Obj::new GxSeparator]

	 set gxsep2 [Obj::new GxSeparator]

	 GxSeparator::addChild $gxsep1 $gxsep2

	 set gxsep3 [Obj::new GxSeparator]

	 GxSeparator::addChild $gxsep2 $gxsep3

	 set face [Obj::new Face]
	 GxSeparator::addChild $gxsep3 $face

	 GxNode::contains $gxsep1 $face
} {^1$}

### GxSeparator::numChildren ###
test "GxSepTcl-GxSeparator::numChildren" "use when empty" {
	 set gxsep [Obj::new GxSeparator]
	 GxSeparator::numChildren $gxsep
} {^0$}

test "GxSepTcl-GxSeparator::numChildren" "use with some children" {
	 set gxsep [Obj::new GxSeparator]
	 set face [Obj::new Face]
	 set child_id1 [GxSeparator::addChild $gxsep $face]
	 set child_id2 [GxSeparator::addChild $gxsep $face]
	 GxSeparator::numChildren $gxsep
} {^2$}

test "GxSepTcl-GxSeparator::numChildren" "use after a remove" {
	 set gxsep [Obj::new GxSeparator]
	 set face [Obj::new Face]
	 set child_id1 [GxSeparator::addChild $gxsep $face]
	 set child_id2 [GxSeparator::addChild $gxsep $face]
	 GxSeparator::removeChildAt $gxsep $child_id1
	 GxSeparator::numChildren $gxsep
} {^1$}

### GxSeparator::removeChildAt ###
test "GxSepTcl-GxSeparator::removeChildAt" "id too small" {
	 set gxsep [Obj::new GxSeparator]
	 GxSeparator::removeChildAt $gxsep -1
	 GxSeparator::numChildren $gxsep
} {^GxSeparator::removeChildAt: signed/unsigned conversion failed}

test "GxSepTcl-GxSeparator::removeChildAt" "use when empty" {
	 set gxsep [Obj::new GxSeparator]
	 GxSeparator::removeChildAt $gxsep 0
	 GxSeparator::numChildren $gxsep
} {^0$}

test "GxSepTcl-GxSeparator::removeChildAt" "use when filled" {
	 set gxsep [Obj::new GxSeparator]
	 set face [Obj::new Face]
	 GxSeparator::addChild $gxsep $face
	 GxSeparator::addChild $gxsep $face
	 GxSeparator::removeChildAt $gxsep 0
	 GxSeparator::numChildren $gxsep
} {^1$}

test "GxSepTcl-GxSeparator::removeChildAt" "id too large" {
	 set gxsep [Obj::new GxSeparator]
	 set face [Obj::new Face]
	 GxSeparator::addChild $gxsep $face
	 GxSeparator::addChild $gxsep $face
	 GxSeparator::removeChildAt $gxsep 2
	 GxSeparator::numChildren $gxsep
} {^2$}


### GxSeparator::removeChild ###
test "GxSepTcl-GxSeparator::removeChild" "try non-contained item" {
	 set gxsep [Obj::new GxSeparator]
	 set face [Obj::new Face]
	 GxSeparator::removeChild $gxsep $face
	 GxSeparator::numChildren $gxsep
} {^0$}

test "GxSepTcl-GxSeparator::removeChild" "use when filled" {
	 set gxsep [Obj::new GxSeparator]
	 set pos [Obj::new GxTransform]
	 set face [Obj::new Face]
	 GxSeparator::addChild $gxsep $pos
	 GxSeparator::addChild $gxsep $face
	 GxSeparator::removeChild $gxsep $face
	 return "[GxSeparator::numChildren $gxsep] [GxNode::contains $gxsep $face]"
} {^1 0$}

### GxSeparator::bugfix ###
test "GxSepTcl-GxSeparator::bugfixes" "destroy after attach" {
	 set gxsep [Obj::new GxSeparator]
	 set fixpt [Obj::new FixPt]
	 GxSeparator::addChild $gxsep $fixpt
	 see $gxsep
	 FixPt::length $fixpt 0.4
	 clearscreen
	 delete $gxsep
	 see $fixpt
	 FixPt::length $fixpt 0.2
	 delete $fixpt
} {}

source ${::TEST_DIR}/io_test.tcl

set ::GXSEP [Obj::new GxSeparator]
set ::FACE [Obj::new Face]
set ::POS [Obj::new GxTransform]

GxSeparator::addChild $::GXSEP $::POS
GxSeparator::addChild $::GXSEP $::FACE

IO::testWriteLGX GxSepTcl $::GXSEP
IO::testReadLGX GxSepTcl $::GXSEP
IO::testWriteASW GxSepTcl $::GXSEP
IO::testReadASW GxSepTcl $::GXSEP
