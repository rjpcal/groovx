foreach xx [dlist::linspace -1.0 1.0 100] {
    lappend data $xx [expr sin($xx * 5)] 0.0
}

Togl::width 650
Togl::height 650
Togl::pixelsPerUnit 1

set pts [new GxPointSet]
-> $pts points $data
-> $pts style 2

set plot [new GxSeparator]
-> $plot debugMode 1
#-> $plot addChildren $pts
-> $plot addChildren [new GxLine {start {-1 0 0} stop {1 0 0}}]
-> $plot addChildren [new GxLine {start {0 -1 0} stop {0 1 0}}]

set xticks [dlist::linspace -1.0 1.0 9]
set yticks [dlist::linspace -1.0 1.0 9]

foreach x 0.0 {
#    set s [new GxSeparator]
#    -> $s debugMode 1
#    set k [new GxLine [list start [list $x -0.025 0] stop [list $x 0.025 0]]]
#    -> $s addChildren $k
#    set p [new Position [list translation [list $x 0 0]]]
#    -> $s addChildren $p
    set t [new Gtext [list text $x font {vector} strokeWidth 1]]
    GrObj::scalingMode $t 2
    GrObj::height $t 1.0
    GrObj::alignmentMode $t 4
    GrObj::centerX $t 0.0
    GrObj::centerY $t 0.0
    GrObj::bbVisibility $t 1
#    -> $s addChildren $t
    dbglevel 2
    puts [-> $t widthFactor]
    puts [-> $t heightFactor]
    puts [GrObj::boundingBox $t]
#    puts [-> $s boundingBox]
    dbglevel 0
    -> $plot addChildren $t
}

set scaler [new GxScaler]
-> $scaler child $plot
-> $scaler mode 2
-> $scaler maxDim 600

see $scaler
