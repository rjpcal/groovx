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
package require Toglet

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
            set objid [Obj::new $subclass]
        } else {
            set objid $objid
        }
    } else {
        set objid -1
    }

    if { $objid > 0 } {
        ::testReadWrite $package $objid
    }

    # class::type
    if { $testbase } {
        ::test ${subclass}::type "too few args" {
            Obj::type
	} {wrong \# args: should be}

        ::test ${subclass}::type "too many args" {
            Obj::type 0 junk
	} {wrong \# args: should be}

        ::test ${subclass}::type "error from bad objid" {
            Obj::type -1
        } {expected.*but got}
    }

    if { $testsubclass } {
        ::test ${subclass}::type "normal use on $subclass" [format {
            Obj::type %s
        } $objid] $subclass
    }


    if { $testbase } {
	# classname::category
        ::test ${subclass}::category "too few args" {
            GxShapeKit::category
	} {wrong \# args: should be}
        ::test ${subclass}::category "too many args" {
            GxShapeKit::category 0 0 junk
	} {wrong \# args: should be}
    }

    if { $testsubclass } {

	# classname::category
        ::test ${subclass}::category "normal use get" [format {
            GxShapeKit::category %s
        } $objid] "^$::INT$"

        ::test ${subclass}::category "normal use set" [format {
            GxShapeKit::category %s 34
            GxShapeKit::category %s
        } $objid $objid] {^34$}

        ::test ${subclass}::category "normal use vector get" [format {
            GxShapeKit::category "%s %s"
        } $objid $objid] "^${::INT}${::SP}${::INT}$"

        ::test ${subclass}::category "normal use vector set with one value" [format {
            GxShapeKit::category "%s %s" 49
            GxShapeKit::category "%s %s"
        } $objid $objid $objid $objid] "^${::INT}${::SP}${::INT}$"

        ::test ${subclass}::category "normal use vector set with many values" [format {
            GxShapeKit::category "%s %s" "3 7 11"
            GxShapeKit::category "%s %s"
        } $objid $objid $objid $objid] "^${::INT}${::SP}${::INT}$"

	# class::draw
        ::test ${subclass}::draw "normal use" [format {
            glClearColor 0 0 0 0
            glColor 1 1 1 1
            clearscreen
            set pix1 [pixelCheckSum]
            see %s
            set pix2 [pixelCheckSum]
            -> [Toglet::current] setVisible false
            return "[expr $pix1 == $pix2] $pix1 $pix2"
	} $objid] {^0 }
    }

    set BASE_CLASS_TESTED 1
}

}
# end namespace GxShapeKit

GxShapeKit::testSubclass Gxshapekit

