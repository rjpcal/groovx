##############################################################################
###
### GrobjTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

# Multiple inclusion guard
if { [info exists GrObj::TEST_DEFINED] } return;

package require Grobj

source ${::TEST_DIR}/io_test.tcl


namespace eval GrObj {

variable TEST_DEFINED 1
variable BASE_CLASS_TESTED 0

namespace export testSubclass
proc testSubclass { package {subclass "GrObj"} {objid -1} } {

	 variable BASE_CLASS_TESTED

	 set testObj(testbase) [expr !$BASE_CLASS_TESTED]
	 set testObj(testsubclass) [expr ![string equal $subclass "GrObj"]]
	 set testObj(package) $package
	 set testObj(subclass) $subclass
	 if { $testObj(testsubclass) } {
		  if { $objid == -1 } {
				set testObj(objid) [eval Obj::new $subclass ]
		  } else {
				set testObj(objid) $objid
		  }
	 } else {
		  set testObj(objid) -1
	 }

	 testTypeCmd testObj
	 testStringifyCmd testObj
	 testDestringifyCmd testObj
	 testReadCmd testObj
	 testWriteCmd testObj
	 testCategoryCmd testObj
	 testDrawCmd testObj
	 testBoundingBoxCmd testObj
	 testAlignmentModeCmd testObj
	 testAspectRatioCmd testObj
	 testBBVisibilityCmd testObj
	 testCenterXCmd testObj
	 testCenterYCmd testObj
	 testHeightCmd testObj
	 testMaxDimensionCmd testObj
	 testRenderModeCmd testObj
	 testScalingModeCmd testObj
	 testUnRenderModeCmd testObj
	 testWidthCmd testObj

	 set BASE_CLASS_TESTED 1
}

proc testTypeCmd { objname } {
	 upvar $objname this

	 set cmdname "IO::type"
	 set usage "wrong \# args: should be \"$cmdname item_id\\(s\\)\""
	 set testname "${this(package)}-${cmdname}"

	 if { $this(testbase) } {
		  eval ::test $testname {"too few args"} {"
		      $cmdname
		  "} {$usage}
		  eval ::test $testname {"too many args"} {"
		      $cmdname 0 junk
		  "} {$usage}
		  eval ::test $testname {"error from bad objid"} {"
		      $cmdname -1
		  "} {"${cmdname}: signed/unsigned.*"}
	 }

	 if { $this(testsubclass) } {
		  eval ::test $testname {"normal use on $this(subclass)"} {"
		      $cmdname $this(objid)
		  "} {$this(subclass)}
	 }
}

proc testStringifyCmd { objname } {
	 upvar $objname this

	 IO::testStringifyCmd $this(package) IO 1 $this(objid)
}

proc testDestringifyCmd { objname } {
	 upvar $objname this

	 IO::testDestringifyCmd $this(package) IO 1 $this(objid)
}

proc testWriteCmd { objname } {
	 upvar $objname this

	 IO::testWriteCmd $this(package) IO 1 $this(objid)
}

proc testReadCmd { objname } {
	 upvar $objname this

	 IO::testReadCmd $this(package) IO 1 $this(objid)
}

proc testCategoryCmd { objname } {
	 upvar $objname this

	 set cmdname "GrObj::category"
	 set usage "wrong \# args: should be"
	 set testname "${this(package)}-${cmdname}"

	 if { $this(testbase) } {
		  eval ::test $testname {"too few args"} {"
				$cmdname
		  "} {$usage}
		  eval ::test $testname {"too many args"} {"
		      $cmdname 0 0 junk
		  "} {$usage}
	 }

	 if { $this(testsubclass) } {
		  eval ::test $testname {"normal use get"} {"
				GrObj::category $this(objid)
		  "} {"^$::INT$"}
		  eval ::test $testname {"normal use set"} {"
				GrObj::category $this(objid) 34
		      GrObj::category $this(objid)
		  "} {^34$}
		  eval ::test $testname {"normal use vector get"} {"
		      GrObj::category \"$this(objid) $this(objid)\"
		  "} {"^${::INT}${::SP}${::INT}$"}
		  eval ::test $testname {"normal use vector set with one value"} {"
		      GrObj::category \"$this(objid) $this(objid)\" 49
		      GrObj::category \"$this(objid) $this(objid)\"
		  "} {"^${::INT}${::SP}${::INT}$"}
		  eval ::test $testname {"normal use vector set with many values"} {"
		      GrObj::category \"$this(objid) $this(objid)\" \"3 7 11\"
		      GrObj::category \"$this(objid) $this(objid)\" 
		  "} {"^${::INT}${::SP}${::INT}$"}
	 }
}

proc testDrawCmd { objname } {
	 upvar $objname this

	 set testname "${this(package)}-draw"

	 if { $this(testsubclass) } {
		  package require Toglet
		  if { ![Togl::inited] } { Togl::init; ::update }

		  eval ::test $testname {"normal use"} {"
		      clearscreen
		      set pix1 \[pixelCheckSum\]
		      see $this(objid)
		      set pix2 \[pixelCheckSum\]
		      Togl::setVisible false
		      expr \$pix1 == \$pix2
		  "} {"^0$"}
	 }
}

proc testBoundingBoxCmd { objname } {
	 upvar $objname this
}

proc testAlignmentModeCmd { objname } {
	 upvar $objname this
}

proc testAspectRatioCmd { objname } {
	 upvar $objname this
}

proc testBBVisibilityCmd { objname } {
	 upvar $objname this
}

proc testCenterXCmd { objname } {
	 upvar $objname this
}

proc testCenterYCmd { objname } {
	 upvar $objname this
}

proc testHeightCmd { objname } {
	 upvar $objname this
}

proc testMaxDimensionCmd { objname } {
	 upvar $objname this
}

proc testRenderModeCmd { objname } {
	 upvar $objname this
}

proc testScalingModeCmd { objname } {
	 upvar $objname this
}

proc testUnRenderModeCmd { objname } {
	 upvar $objname this
}

proc testWidthCmd { objname } {
	 upvar $objname this
}

} 
# end namespace GrObj

GrObj::testSubclass Grobj

