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

    set testbase [expr !$BASE_CLASS_TESTED]
    set testsubclass [expr ![string equal $subclass "GxShapeKit"]]

    if { $testsubclass } {
        if { $objid == -1 } {
            set this(objid) [eval Obj::new $subclass ]
        } else {
            set this(objid) $objid
        }
    } else {
        set this(objid) -1
    }

    if { $this(objid) > 0 } {
        ::testReadWrite $package $this(objid)
    }

    # class::type
    if { $testbase } {
        eval ::test ${subclass}::type {"too few args"} {"
            Obj::type
        "} {"wrong \# args: should be"}
        eval ::test ${subclass}::type {"too many args"} {"
            Obj::type 0 junk
        "} {"wrong \# args: should be"}
        eval ::test ${subclass}::type {"error from bad objid"} {"
            Obj::type -1
        "} {"expected.*but got"}
    }

    if { $testsubclass } {
        eval ::test ${subclass}::type {"normal use on $subclass"} {"
            Obj::type $this(objid)
        "} {$subclass}
    }


    # classname::category
    if { $testbase } {
        eval ::test ${subclass}::category {"too few args"} {"
            GxShapeKit::category
        "} {"wrong \# args: should be"}
        eval ::test ${subclass}::category {"too many args"} {"
            GxShapeKit::category 0 0 junk
        "} {"wrong \# args: should be"}
    }

    if { $testsubclass } {
        eval ::test ${subclass}::category {"normal use get"} {"
            GxShapeKit::category $this(objid)
        "} {"^$::INT$"}
        eval ::test ${subclass}::category {"normal use set"} {"
            GxShapeKit::category $this(objid) 34
            GxShapeKit::category $this(objid)
        "} {^34$}
        eval ::test ${subclass}::category {"normal use vector get"} {"
            GxShapeKit::category \"$this(objid) $this(objid)\"
        "} {"^${::INT}${::SP}${::INT}$"}
        eval ::test ${subclass}::category {"normal use vector set with one value"} {"
            GxShapeKit::category \"$this(objid) $this(objid)\" 49
            GxShapeKit::category \"$this(objid) $this(objid)\"
        "} {"^${::INT}${::SP}${::INT}$"}
        eval ::test ${subclass}::category {"normal use vector set with many values"} {"
            GxShapeKit::category \"$this(objid) $this(objid)\" \"3 7 11\"
            GxShapeKit::category \"$this(objid) $this(objid)\"
        "} {"^${::INT}${::SP}${::INT}$"}
    }


    # class::draw
    if { $testsubclass } {
        package require Toglet

        eval ::test ${subclass}::draw {"normal use"} {"
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

