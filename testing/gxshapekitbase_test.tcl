##############################################################################
###
### GxShapeKit
### Rob Peters
### Oct-2004
###
##############################################################################

# class::type
::test GxShapeKit::type "too few args" {
    Obj::type
} {wrong \# args: should be}

::test GxShapeKit::type "too many args" {
    Obj::type 0 junk
} {wrong \# args: should be}

::test GxShapeKit::type "error from bad objid" {
    Obj::type -1
} {expected.*but got}

# classname::category
::test GxShapeKit::category "too few args" {
    GxShapeKit::category
} {wrong \# args: should be}
::test GxShapeKit::category "too many args" {
    GxShapeKit::category 0 0 junk
} {wrong \# args: should be}
