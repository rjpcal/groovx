##############################################################################
###
### GrobjTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Objlist
package require Grobj
package require Face
package require Fixpt
package require Objtogl

if { ![Togl::inited] } { Togl::init "-rgba false"; update }

set FACE [Face::face]
set FIXPT [Fixpt::fixpt]

### GrObj::typeCmd ###
test "GrobjTcl-GrObj::type" "too few args" {
    GrObj::type
} {wrong \# args: should be "GrObj::type objid"}
test "GrobjTcl-GrObj::type" "too many args" {
    GrObj::type j u
} {wrong \# args: should be "GrObj::type objid"}
test "GrobjTcl-GrObj::type" "normal use on Face" {
	 GrObj::type $::FACE
} {Face}
test "GrobjTcl-GrObj::type" "normal use on FixPt" {
	 GrObj::type $::FIXPT
} {FixPt}
test "GrobjTcl-GrObj::type" "error from bad objid" {
	 GrObj::type -1
} {GrObj::type: objid out of range}
test "GrobjTcl-GrObj::type" "error from too large objid" {
	 GrObj::type 10000
} {GrObj::type: objid out of range}

### GrObj::stringifyCmd ###
test "GrobjTcl-GrObj::stringify" "too few args" {
	 GrObj::stringify
} {^wrong \# args: should be "GrObj::stringify item_id"$}
test "GrobjTcl-GrObj::stringify" "too many args" {
	 GrObj::stringify $::FACE junk
} {^wrong \# args: should be "GrObj::stringify item_id"$}
test "GrobjTcl-GrObj::stringify" "normal use" {
	 catch {GrObj::stringify $::FACE}
} {^0$}
test "GrobjTcl-GrObj::stringify" "error" {} {^$} $no_test

### GrObj::destringifyCmd ###
test "GrobjTcl-GrObj::destringify" "too few args" {
	 GrObj::destringify
} {^wrong \# args: should be "GrObj::destringify item_id string"$}
test "GrobjTcl-GrObj::destringify" "too many args" {
	 GrObj::destringify $::FACE junk junk
} {^wrong \# args: should be "GrObj::destringify item_id string"$}
test "GrobjTcl-GrObj::destringify" "normal use" {
	 catch {GrObj::destringify $::FACE [GrObj::stringify $::FACE]}
} {^0$}
test "GrobjTcl-GrObj::destringify" "error" {
	 catch {GrObj::destringify $::FACE junk}
} {^1$}

### GrObj::categoryCmd ###
test "GrobjTcl-GrObj::category" "too few args" {
	 GrObj::category
} {^wrong \# args: should be "GrObj::category item_id\(s\) \?new_value\(s\)\?"$}
test "GrobjTcl-GrObj::category" "too many args" {
	 GrObj::category 0 0 junk
} {^wrong \# args: should be "GrObj::category item_id\(s\) \?new_value\(s\)\?"$}
test "GrobjTcl-GrObj::category" "normal use get" {
	 GrObj::category $::FACE
} "^$INT$"
test "GrobjTcl-GrObj::category" "normal use set" {
	 GrObj::category $::FACE 34
	 GrObj::category $::FACE
} {^34$}
test "GrobjTcl-GrObj::category" "normal use vector get" {
	 GrObj::category "$::FACE $::FIXPT"
} "^${INT}${SP}${INT}$"
test "GrobjTcl-GrObj::category" "normal use vector set with one value" {
	 GrObj::category "$::FACE $::FIXPT" 49
	 GrObj::category "$::FACE $::FIXPT"
} "^${INT}${SP}${INT}$"
test "GrobjTcl-GrObj::category" "normal use vector set with many values" {
	 GrObj::category "$::FACE $::FIXPT" "3 7 11"
	 GrObj::category "$::FACE $::FIXPT"
} "^${INT}${SP}${INT}$"
test "GrobjTcl-GrObj::category" "error" {} {^$} $no_test

### GrObj::usingCompileCmd ###
test "GrobjTcl-GrObj::usingCompile" "too few args" {
	 GrObj::usingCompile
} {^wrong \# args: should be "GrObj::usingCompile item_id\(s\) \?new_value\(s\)\?"$}
test "GrobjTcl-GrObj::usingCompile" "too many args" {
	 GrObj::usingCompile 0 0 junk
} {^wrong \# args: should be "GrObj::usingCompile item_id\(s\) \?new_value\(s\)\?"$}
test "GrobjTcl-GrObj::usingCompile" "normal use vector get" {
	 GrObj::usingCompile "$::FACE $::FIXPT"
} "^${INT}${SP}${INT}$"
test "GrobjTcl-GrObj::usingCompile" "error" {} {^$} $no_test

### GrObj::updateCmd ###
test "GrobjTcl-GrObj::update" "too few args" {
	 GrObj::update
} {^wrong \# args: should be "GrObj::update item_id\(s\)"$}
test "GrobjTcl-GrObj::update" "too many args" {
	 GrObj::update $::FACE junk
} {^wrong \# args: should be "GrObj::update item_id\(s\)"$}
test "GrobjTcl-GrObj::update" "normal use" {
	 catch {GrObj::update $::FACE}
} {^0$}
test "GrobjTcl-GrObj::update" "error" {} {^$} $no_test


### Cleanup
unset FACE
unset FIXPT