set fd [open "objfiles" r]
set files [split [read $fd] "\n"]
close $fd

set failures [open "failed" w]
set log [open "log" w]

set i 0

proc convert_file { fname dest } {
    set fd [open $fname r]
    set ofd [open $dest w]
    while { [gets $fd line] != -1 } {
	regsub -all {Face @1} $line {Face @2} line2
	regsub -all {GrObj} $line2 {GxShapeKit} line3
	regsub -all {Fish @3} $line3 {Fish @4} line4
	regsub -all {MorphyFace @1} $line4 {MorphyFace @2} line5
	if { [regexp {Fish @2} $line5] } {
	    regsub -all {Fish @2} $line5 {Fish @4} line6
	    set line5 $line6
	    regsub -all {0 0 1 1 GxShapeKit} $line5 {GxShapeKit} line6
	    set line5 $line6
	}
	regsub -all {House @2} $line5 {House @3} line6
	puts $ofd $line6
    }
    close $ofd
    close $fd
}

foreach fname $files {
    if { [string length $fname] == 0 } { continue }

    if { ![file exists $fname] } { continue }

    unset -nocomplain xp

    if { [catch {exec grep {[A-Z]} $fname}] == 0 } {
	puts -nonewline $log "[incr i] ${fname}..."
	set code [catch {ObjDb::loadObjects $fname} result]
	if { $code == 0 } {
	    puts $log "read ok"
	} else {
	    puts $log "read failed: $result"

	    convert_file $fname check.lgx

	    puts -nonewline $log "$f ${fname}..."
	    set code [catch {ObjDb::loadObjects check.lgx} result]
	    if { $code == 0 } {
		puts $log "conversion ok"

		set extension ".lgx"

		set dirname [file dirname $fname]

		set stem [file tail $fname]

		set stem [file rootname $stem]

		set dest ${dirname}/${stem}${extension}

		file rename check.lgx $dest

		set objdir /cit/rjpeters/oldobj/$dirname
		file mkdir $objdir

		set objdest /cit/rjpeters/oldobj/$fname

		puts -nonewline $log "$i ${objdest}..."

		file rename $fname $objdest

		puts $log "rename ok"
	    } else {
		puts $log "conversion failed: $result"
		puts $failures $fname
	    }
	}
    } else {
	# nothing
    }

    flush $log
    flush $failures

    ObjDb::purge
}

close $log
close $failures

exit
