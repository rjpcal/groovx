##############################################################################
###
### DlistTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### dlist::choose ###
test "DlistTcl-dlist::choose" "args" {
	 dlist::choose junk
} {wrong \# args: should be "dlist::choose source_list index_list"}
test "DlistTcl-dlist::choose" "norm1" { 
	 catch {dlist::choose {} {}}
} {^0$}
test "DlistTcl-dlist::choose" "norm2" { 
	 dlist::choose { 3.1 5 7 } { 2 0 1 }
} {7 3\.1 5}
test "DlistTcl-dlist::choose" "err1" {
     dlist::choose {1 2} {-1 0} 
} {dlist::choose: signed/unsigned conversion failed}
test "DlistTcl-dlist::choose" "err2" {
     dlist::choose {1 2} {1.5 0} 
} {expected integer but got "1\.5"}

### dlist::not ###
test "DlistTcl-dlist::not" "args" {
    dlist::not
} {wrong \# args: should be "dlist::not source_list"}
test "DlistTcl-dlist::not" "norm1" { 
	 catch {dlist::not {}}
} {^0$}
test "DlistTcl-dlist::not" "norm2" { 
	 dlist::not { -1 0 1 349 }
} {0 1 0 0}
test "DlistTcl-dlist::not" "error" {
     dlist::not { -1.5 0.3 }
} {expected integer but got "-1\.5"}

### dlist::ones ###
test "DlistTcl-dlist::ones" "args" {
    dlist::ones
} {wrong \# args: should be "dlist::ones num_ones"}
test "DlistTcl-dlist::ones" "norm1" { 
	 catch {dlist::ones 0}
} {^0$}
test "DlistTcl-dlist::ones" "norm2" { 
	 dlist::ones 5
} {1 1 1 1 1}
test "DlistTcl-dlist::ones" "err1" {
     dlist::ones 4.5
} {expected integer but got "4\.5"}
test "DlistTcl-dlist::ones" "err2" {
	 dlist::ones -1
} {dlist::ones: signed/unsigned conversion failed}

### dlist::pickone ###
test "DlistTcl-dlist::pickone" "args" {
    dlist::pickone
} {wrong \# args: should be "dlist::pickone source_list"}
test "DlistTcl-dlist::pickone" "norm1" { 
	 dlist::pickone {1 3 5 7 9}
} {[13579]}
test "DlistTcl-dlist::pickone" "norm2" {
     dlist::pickone {0 3.4 -2.6 str {list list}}
} {0|3\.4|-2\.6|str|list list}
test "DlistTcl-dlist::pickone" "error" {
     dlist::pickone {}
} {dlist::pickone: source_list is empty}

### dlist::range ###
test "DlistTcl-dlist::range" "args" {
    dlist::range
} {wrong \# args: should be "dlist::range begin end"}
test "DlistTcl-dlist::range" "norm1" { 
	 dlist::range 0 0
} {^0$}
test "DlistTcl-dlist::range" "norm2" { 
	 dlist::range 0 5
} {0 1 2 3 4 5}
test "DlistTcl-dlist::range" "norm3" { dlist::range 0 -1} {^$}
test "DlistTcl-dlist::range" "err1" {
    dlist::range 0.5 6
} {expected integer but got "0\.5"}

### dlist::repeat ###
test "DlistTcl-dlist::repeat" "args" {
    dlist::repeat
} {wrong \# args: should be "dlist::repeat source_list times_list"}
test "DlistTcl-dlist::repeat" "norm1" {
	 catch {dlist::repeat {} {}}
} {^0$}
test "DlistTcl-dlist::repeat" "norm2" { 
	 catch {dlist::repeat { 4 5 6 } { 0 0 0 }}
} {^0$}
test "DlistTcl-dlist::repeat" "norm3" { 
	 dlist::repeat { 4 5 6 } { 1 2 3 }
} {4 5 5 6 6 6}
test "DlistTcl-dlist::repeat" "err4" { 
	 dlist::repeat { 4 5 6 } { -1 2 3 }
} {dlist::repeat: signed/unsigned conversion failed}
test "DlistTcl-dlist::repeat" "err2" {
     dlist::repeat { 4 5 6 } { 1.5 2 3 }
} {expected integer but got "1\.5"}

### dlist::select ###
test "DlistTcl-dlist::select" "args" {
    dlist::select
} {wrong \# args: should be "dlist::select source_list flags_list"}
test "DlistTcl-dlist::select" "norm1" { 
	 dlist::select {1 2 3 4 5} {1 0 0 1 1}
} {1 4 5}
test "DlistTcl-dlist::select" "norm2" { 
	 dlist::select {1 2.5 str {L L}} {0 1 1 1}
} {2\.5 str \{L L\}}
test "DlistTcl-dlist::select" "error" {
     dlist::select {1 2 3} {1.5 0.2 0.8}
} {expected integer but got "1\.5"}

### dlist::sum ###
test "DlistTcl-dlist::sum" "args" {
    dlist::sum
} {wrong \# args: should be "dlist::sum source_list"}
test "DlistTcl-dlist::sum" "norm int" {
	 dlist::sum { -3 6 0 9 17} 
} {^29$}
test "DlistTcl-dlist::sum" "norm float" {
     dlist::sum { 1.6 0.0 -0.2 12.9 }
} {^14\.3$}
test "DlistTcl-dlist::sum" "norm mixed" {
     dlist::sum { -4 2 -0.1 4.5 }
} {^2\.4$}
test "DlistTcl-dlist::sum" "error" {
     dlist::sum { 1 junk }
} {expected floating-point number but got "junk"}

### dlist::zeros ###
test "DlistTcl-dlist::zeros" "args" {
	 dlist::zeros
} {wrong \# args: should be "dlist::zeros num_zeros"}
test "DlistTcl-dlist::zeros" "norm1" {
	 catch {dlist::zeros 0}
} {^0$}
test "DlistTcl-dlist::zeros" "norm2" {
	 dlist::zeros 5
} {0 0 0 0 0}
test "DlistTcl-dlist::zeros" "err1" { 
	 dlist::zeros 3.5
} {expected integer but got "3\.5"}


