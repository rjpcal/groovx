##############################################################################
###
### DlistTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### dlist_chooseCmd ###
test "DlistTcl-dlist_choose" "args" {
	 dlist_choose dlist_choose
} {wrong \# args: should be "::dlist_choose source_list index_list"}
test "DlistTcl-dlist_choose" "norm1" { 
	 catch {dlist_choose {} {}}
} {^0$}
test "DlistTcl-dlist_choose" "norm2" { 
	 dlist_choose { 3.1 5 7 } { 2 0 1 }
} {7 3\.1 5}
test "DlistTcl-dlist_choose" "err1" {
     dlist_choose {1 2} {-1 0} 
} {dlist_choose: signed/unsigned conversion failed}
test "DlistTcl-dlist_choose" "err2" {
     dlist_choose {1 2} {1.5 0} 
} {expected integer but got "1\.5"}

### dlist_notCmd ###
test "DlistTcl-dlist_not" "args" {
    dlist_not
} {wrong \# args: should be "::dlist_not source_list"}
test "DlistTcl-dlist_not" "norm1" { 
	 catch {dlist_not {}}
} {^0$}
test "DlistTcl-dlist_not" "norm2" { 
	 dlist_not { -1 0 1 349 }
} {0 1 0 0}
test "DlistTcl-dlist_not" "error" {
     dlist_not { -1.5 0.3 }
} {expected integer but got "-1\.5"}

### dlist_onesCmd ###
test "DlistTcl-dlist_ones" "args" {
    dlist_ones
} {wrong \# args: should be "::dlist_ones num_ones"}
test "DlistTcl-dlist_ones" "norm1" { 
	 catch {dlist_ones 0}
} {^0$}
test "DlistTcl-dlist_ones" "norm2" { 
	 dlist_ones 5
} {1 1 1 1 1}
test "DlistTcl-dlist_ones" "err1" {
     dlist_ones 4.5
} {expected integer but got "4\.5"}
test "DlistTcl-dlist_ones" "err2" {
	 dlist_ones -1
} {dlist_ones: signed/unsigned conversion failed}

### dlist_pickoneCmd ###
test "DlistTcl-dlist_pickone" "args" {
    dlist_pickone
} {wrong \# args: should be "::dlist_pickone source_list"}
test "DlistTcl-dlist_pickone" "norm1" { 
	 dlist_pickone {1 3 5 7 9}
} {[13579]}
test "DlistTcl-dlist_pickone" "norm2" {
     dlist_pickone {0 3.4 -2.6 str {list list}}
} {0|3\.4|-2\.6|str|list list}
test "DlistTcl-dlist_pickone" "error" {
     dlist_pickone {}
} {dlist_pickone: source_list is empty}

### dlist_rangeCmd ###
test "DlistTcl-dlist_range" "args" {
    dlist_range
} {wrong \# args: should be "::dlist_range begin end"}
test "DlistTcl-dlist_range" "norm1" { 
	 dlist_range 0 0
} {^0$}
test "DlistTcl-dlist_range" "norm2" { 
	 dlist_range 0 5
} {0 1 2 3 4 5}
test "DlistTcl-dlist_range" "norm3" { dlist_range 0 -1} {^$}
test "DlistTcl-dlist_range" "err1" {
    dlist_range 0.5 6
} {expected integer but got "0\.5"}

### dlist_repeatCmd ###
test "DlistTcl-dlist_repeat" "args" {
    dlist_repeat
} {wrong \# args: should be "::dlist_repeat source_list times_list"}
test "DlistTcl-dlist_repeat" "norm1" {
	 catch {dlist_repeat {} {}}
} {^0$}
test "DlistTcl-dlist_repeat" "norm2" { 
	 catch {dlist_repeat { 4 5 6 } { 0 0 0 }}
} {^0$}
test "DlistTcl-dlist_repeat" "norm3" { 
	 dlist_repeat { 4 5 6 } { 1 2 3 }
} {4 5 5 6 6 6}
test "DlistTcl-dlist_repeat" "err4" { 
	 dlist_repeat { 4 5 6 } { -1 2 3 }
} {dlist_repeat: signed/unsigned conversion failed}
test "DlistTcl-dlist_repeat" "err2" {
     dlist_repeat { 4 5 6 } { 1.5 2 3 }
} {expected integer but got "1\.5"}

### dlist_selectCmd ###
test "DlistTcl-dlist_select" "args" {
    dlist_select
} {wrong \# args: should be "::dlist_select source_list flags_list"}
test "DlistTcl-dlist_select" "norm1" { 
	 dlist_select {1 2 3 4 5} {1 0 0 1 1}
} {1 4 5}
test "DlistTcl-dlist_select" "norm2" { 
	 dlist_select {1 2.5 str {L L}} {0 1 1 1}
} {2\.5 str \{L L\}}
test "DlistTcl-dlist_select" "error" {
     dlist_select {1 2 3} {1.5 0.2 0.8}
} {expected integer but got "1\.5"}

### dlist_sumCmd ###
test "DlistTcl-dlist_sum" "args" {
    dlist_sum
} {wrong \# args: should be "::dlist_sum source_list"}
test "DlistTcl-dlist_sum" "norm int" {
	 dlist_sum { -3 6 0 9 17} 
} {^29$}
test "DlistTcl-dlist_sum" "norm float" {
     dlist_sum { 1.6 0.0 -0.2 12.9 }
} {^14\.3$}
test "DlistTcl-dlist_sum" "norm mixed" {
     dlist_sum { -4 2 -0.1 4.5 }
} {^2\.4$}
test "DlistTcl-dlist_sum" "error" {
     dlist_sum { 1 junk }
} {expected floating-point number but got "junk"}

### dlist_zerosCmd ###
test "DlistTcl-dlist_zeros" "args" {
	 dlist_zeros
} {wrong \# args: should be "::dlist_zeros num_zeros"}
test "DlistTcl-dlist_zeros" "norm1" {
	 catch {dlist_zeros 0}
} {^0$}
test "DlistTcl-dlist_zeros" "norm2" {
	 dlist_zeros 5
} {0 0 0 0 0}
test "DlistTcl-dlist_zeros" "err1" { 
	 dlist_zeros 3.5
} {expected integer but got "3\.5"}


