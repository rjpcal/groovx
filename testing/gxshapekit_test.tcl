##############################################################################
###
### GxShapeKit
### Rob Peters
### $Id$
###
##############################################################################

# Multiple inclusion guard
if { [info exists GxShapeKit::TEST_DEFINED] } return;

package require Gxshapekit

source ${::TEST_DIR}/io_test.tcl


namespace eval GxShapeKit {

variable TEST_DEFINED 1
variable BASE_CLASS_TESTED 0

namespace export testSubclass
proc testSubclass { package {subclass "GxShapeKit"} {objid -1} } {

    variable BASE_CLASS_TESTED

    set testObj(testbase) [expr !$BASE_CLASS_TESTED]
    set testObj(testsubclass) [expr ![string equal $subclass "GxShapeKit"]]
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
    if { $testObj(objid) > 0 } {
	testWriteLGX testObj
	testReadLGX testObj
	testReadASW testObj
	testWriteASW testObj
    }
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

    set cmdname "Obj::type"
    set usage "wrong \# args: should be \"$cmdname objref\\(s\\)\""
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
        "} {"signed/unsigned.*"}
    }

    if { $this(testsubclass) } {
        eval ::test $testname {"normal use on $this(subclass)"} {"
            $cmdname $this(objid)
        "} {$this(subclass)}
    }
}

proc testWriteLGX { objname } {
    upvar $objname this

    IO::testWriteLGX $this(package) $this(objid)
}

proc testReadLGX { objname } {
    upvar $objname this

    IO::testReadLGX $this(package) $this(objid)
}

proc testWriteASW { objname } {
    upvar $objname this

    IO::testWriteASW $this(package) $this(objid)
}

proc testReadASW { objname } {
    upvar $objname this

    IO::testReadASW $this(package) $this(objid)
}

proc testCategoryCmd { objname } {
    upvar $objname this

    set cmdname "GxShapeKit::category"
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
            GxShapeKit::category $this(objid)
        "} {"^$::INT$"}
        eval ::test $testname {"normal use set"} {"
            GxShapeKit::category $this(objid) 34
            GxShapeKit::category $this(objid)
        "} {^34$}
        eval ::test $testname {"normal use vector get"} {"
            GxShapeKit::category \"$this(objid) $this(objid)\"
        "} {"^${::INT}${::SP}${::INT}$"}
        eval ::test $testname {"normal use vector set with one value"} {"
            GxShapeKit::category \"$this(objid) $this(objid)\" 49
            GxShapeKit::category \"$this(objid) $this(objid)\"
        "} {"^${::INT}${::SP}${::INT}$"}
        eval ::test $testname {"normal use vector set with many values"} {"
            GxShapeKit::category \"$this(objid) $this(objid)\" \"3 7 11\"
            GxShapeKit::category \"$this(objid) $this(objid)\" 
        "} {"^${::INT}${::SP}${::INT}$"}
    }
}

proc testDrawCmd { objname } {
    upvar $objname this

    set testname "${this(package)}-draw"

    if { $this(testsubclass) } {
        package require Toglet

        eval ::test $testname {"normal use"} {"
            glClearColor 0 0 0 0
            glColor 1 1 1 1
            clearscreen
            set pix1 \[pixelCheckSum\]
            see $this(objid)
            set pix2 \[pixelCheckSum\]
            -> \[Toglet::current\] setVisible false
            return \"\[expr \$pix1 == \$pix2\] \$pix1 \$pix2\"
         "} {"^0 "}
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
# end namespace GxShapeKit

GxShapeKit::testSubclass Gxshapekit

