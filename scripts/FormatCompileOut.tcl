proc format_compile_out {filename} {
    set inf [open $filename r]
    set outf [open ${filename}.fmt w]

    set nextline ""

    set NAME {[%a-zA-Z/]*\.cc}
    set FLOAT {[0-9]*\.[0-9]*}
    set INT {[0-9]*}
    while { [gets $inf nextline] != -1 } {
	set exp "($NAME) *($FLOAT) *($FLOAT) *($FLOAT) *($INT) *($INT) *($FLOAT) *($FLOAT) *($FLOAT)"
	regexp $exp $nextline fullmatch source r1 u1 s1 wc1 wc2 r2 u2 s2
	set blank ""
	set line [format "%21s %3.1f %3.1f %3.1f %7d %7d %3.1f %3.1f %3.1f" $blank $r1 $u1 $s1 $wc1 $wc2 $r2 $u2 $s2]
	puts $outf "%$source"
	puts $outf $line
    }

    close $inf
    close $outf
}
