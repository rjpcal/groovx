##############################################################################
###
### Dlist
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### dlist::choose ###
test "Dlist-dlist::choose" "args" {
    dlist::choose junk
} {wrong \# args: should be "dlist::choose source_list index_list"}
test "Dlist-dlist::choose" "norm1" {
    catch {dlist::choose {} {}}
} {^0$}
test "Dlist-dlist::choose" "norm2" {
    dlist::choose { 3.1 5 7 } { 2 0 1 }
} {7 3\.1 5}
test "Dlist-dlist::choose" "err1" {
     dlist::choose {1 2} {-1 0}
} {signed/unsigned conversion failed}
test "Dlist-dlist::choose" "err2" {
     dlist::choose {1 2} {1.5 0}
} {expected integer but got "1\.5"}

### dlist::index ###
test "Dlist-dlist::index" "args" {
    dlist::index
} {wrong \# args: should be "dlist::index list index"}
test "Dlist-dlist::index" "norm" {
    set a { 3 5 9 }
    dlist::index $a 1
} {^5$}
test "Dlist-dlist::index" "err" {
    set a {3 5 9}
    dlist::index $a 3
} {^dlist::index: }

### dlist::not ###
test "Dlist-dlist::not" "args" {
    dlist::not
} {wrong \# args: should be "dlist::not list"}
test "Dlist-dlist::not" "norm1" {
    catch {dlist::not {}}
} {^0$}
test "Dlist-dlist::not" "norm2" {
    dlist::not { -1 0 1 349 }
} {0 1 0 0}
test "Dlist-dlist::not" "error" {
     dlist::not { -1.5 0.3 }
} {expected integer but got "-1\.5"}

### dlist::ones ###
test "Dlist-dlist::ones" "args" {
    dlist::ones
} {wrong \# args: should be "dlist::ones num_ones"}
test "Dlist-dlist::ones" "norm1" {
    catch {dlist::ones 0}
} {^0$}
test "Dlist-dlist::ones" "norm2" {
    dlist::ones 5
} {1 1 1 1 1}
test "Dlist-dlist::ones" "err1" {
     dlist::ones 4.5
} {expected integer but got "4\.5"}
test "Dlist-dlist::ones" "err2" {
    dlist::ones -1
} {signed/unsigned conversion failed}

### dlist::pickone ###
test "Dlist-dlist::pickone" "args" {
    dlist::pickone
} {wrong \# args: should be "dlist::pickone list"}
test "Dlist-dlist::pickone" "norm1" {
    dlist::pickone {1 3 5 7 9}
} {[13579]}
test "Dlist-dlist::pickone" "norm2" {
     dlist::pickone {0 3.4 -2.6 str {list list}}
} {0|3\.4|-2\.6|str|list list}
test "Dlist-dlist::pickone" "error" {
     dlist::pickone {}
} {source_list is empty}

### dlist::range ###
test "Dlist-dlist::range" "args" {
    dlist::range
} {wrong \# args: should be one of}
test "Dlist-dlist::range" "norm1" { dlist::range 0  0}   {^0$}
test "Dlist-dlist::range" "norm2" { dlist::range 0  5}   {0 1 2 3 4 5}
test "Dlist-dlist::range" "norm3" { dlist::range 0 -1}   {^$}
test "Dlist-dlist::range" "step0" { dlist::range 5 10 0} {^$}
test "Dlist-dlist::range" "empty1" { dlist::range 5 10 -1} {^$}
test "Dlist-dlist::range" "empty1" { dlist::range 10 5  1} {^$}
test "Dlist-dlist::range" "step+" { dlist::range 2 11  3} {^2 5 8 11$}
test "Dlist-dlist::range" "step-" { dlist::range 12 1 -3} {^12 9 6 3$}
test "Dlist-dlist::range" "err1" {
    dlist::range 0.5 6
} {expected integer but got "0\.5"}

### dlist::repeat ###
test "Dlist-dlist::repeat" "args" {
    dlist::repeat
} {wrong \# args: should be "dlist::repeat source_list times_list"}
test "Dlist-dlist::repeat" "norm1" {
    catch {dlist::repeat {} {}}
} {^0$}
test "Dlist-dlist::repeat" "norm2" {
    catch {dlist::repeat { 4 5 6 } { 0 0 0 }}
} {^0$}
test "Dlist-dlist::repeat" "norm3" {
    dlist::repeat { 4 5 6 } { 1 2 3 }
} {4 5 5 6 6 6}
test "Dlist-dlist::repeat" "err4" {
    dlist::repeat { 4 5 6 } { -1 2 3 }
} {signed/unsigned conversion failed}
test "Dlist-dlist::repeat" "err2" {
     dlist::repeat { 4 5 6 } { 1.5 2 3 }
} {expected integer but got "1\.5"}

### dlist::select ###
test "Dlist-dlist::select" "args" {
    dlist::select
} {wrong \# args: should be "dlist::select source_list flags_list"}
test "Dlist-dlist::select" "norm1" {
    dlist::select {1 2 3 4 5} {1 0 0 1 1}
} {1 4 5}
test "Dlist-dlist::select" "norm2" {
    dlist::select {1 2.5 str {L L}} {0 1 1 1}
} {2\.5 str \{L L\}}
test "Dlist-dlist::select" "error" {
     dlist::select {1 2 3} {1.5 0.2 0.8}
} {expected integer but got "1\.5"}

### dlist::slice ###
test "Dlist-dlist::slice" "args" {
    dlist::slice
} {wrong \# args: should be "dlist::slice list n"}
test "Dlist-dlist::slice" "norm1" {
    dlist::slice {{1 2 3} {4 5 6} {7 8 9}} 1
} {^2 5 8$}
test "Dlist-dlist::slice" "norm2" {
    dlist::slice {{1 2 3} {4 5 6} {7 8}} 0
} {^1 4 7}
test "Dlist-dlist::slice" "err1" {
    dlist::slice {{1 2 3} {4 5 6} {7 8 9}} 3
} {^dlist::slice: }
test "Dlist-dlist::slice" "err2" {
    dlist::slice {{1 2 3} {4 5} {7 8 9}} 2
} {^dlist::slice: }

### dlist::sum ###
test "Dlist-dlist::sum" "args" {
    dlist::sum
} {wrong \# args: should be "dlist::sum list"}
test "Dlist-dlist::sum" "norm int" {
    dlist::sum { -3 6 0 9 17}
} {^29$}
test "Dlist-dlist::sum" "norm float" {
     dlist::sum { 1.6 0.0 -0.2 12.9 }
} {^14\.3$}
test "Dlist-dlist::sum" "norm mixed" {
     dlist::sum { -4 2 -0.1 4.5 }
} {^2\.4$}
test "Dlist-dlist::sum" "error" {
     dlist::sum { 1 junk }
} {expected floating-point number but got "junk"}

### dlist::zeros ###
test "Dlist-dlist::zeros" "args" {
    dlist::zeros
} {wrong \# args: should be "dlist::zeros num_zeros"}
test "Dlist-dlist::zeros" "norm1" {
    catch {dlist::zeros 0}
} {^0$}
test "Dlist-dlist::zeros" "norm2" {
    dlist::zeros 5
} {0 0 0 0 0}
test "Dlist-dlist::zeros" "err1" {
    dlist::zeros 3.5
} {expected integer but got "3\.5"}


