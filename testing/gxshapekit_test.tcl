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

    set this(testbase) [expr !$BASE_CLASS_TESTED]
    set this(testsubclass) [expr ![string equal $subclass "GxShapeKit"]]
    set this(package) $package
    set this(subclass) $subclass
    if { $this(testsubclass) } {
        if { $objid == -1 } {
            set this(objid) [eval Obj::new $subclass ]
        } else {
            set this(objid) $objid
        }
    } else {
        set this(objid) -1
    }

    if { $this(objid) > 0 } {
	::testReadWrite $this(package) $this(objid)
    }

    # class::type
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
        "} {"expected.*but got"}
    }

    if { $this(testsubclass) } {
        eval ::test $testname {"normal use on $this(subclass)"} {"
            $cmdname $this(objid)
        "} {$this(subclass)}
    }


    # classname::category
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


    # class::draw
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

    set BASE_CLASS_TESTED 1
}

}
# end namespace GxShapeKit

GxShapeKit::testSubclass Gxshapekit

