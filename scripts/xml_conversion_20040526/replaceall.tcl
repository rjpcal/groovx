set fd [open "aswfiles" r]
set files [split [read $fd] "\n"]
close $fd

set failures [open "failed" w]
set log [open "log" w]

set i 0

foreach fname $files {
    if { [string length $fname] == 0 } { continue }

    unset -nocomplain xp

    puts -nonewline $log "[incr i] ${fname}..."
    set code [catch {set xp [IO::retrieve $fname]} result]
    if { $code == 0 } {
	puts $log "read ok"

	set extension ".gvx"

	if { [string equal ".gz" [file extension $fname]] } {
	    set gzipped 1
	    set extension ".gvx.gz"
	} else {
	    set finfo [exec file $fname]
	    if { [string first "gzip" $finfo] > 0 } {
		set gzipped 1
		set extension ".gvx.gz"
	    }
	}

	set dirname [file dirname $fname]

	set stem [file tail $fname]

	set stem [file rootname [file rootname $stem]]

	set dest ${dirname}/${stem}${extension}

	puts -nonewline $log "$i ${dest}..."

	IO::saveXML $xp $dest

	puts $log "save ok"

	puts -nonewline $log "$i ${dest}..."

	set code [catch {set xp2 [IO::loadXML $dest]} result]

	if { $code == 0 } {
	    puts $log "reread ok"
	    puts -nonewline $log "$i ${dest}..."
	    IO::saveXML $xp2 cmpcheck.gvx.gz
	    exec zdiff $dest cmpcheck.gvx.gz
	    puts $log "cmpcheck ok"
	} else {
	    puts $log "reread failed: $result"
	}

	set aswdir /cit/rjpeters/oldasw/$dirname
	file mkdir $aswdir

	set aswdest /cit/rjpeters/oldasw/$fname

	puts -nonewline $log "$i ${aswdest}..."

	file rename $fname $aswdest

	puts $log "rename ok"

    } else {
	puts $log "failed: $result"
	puts $failures $fname
    }

    flush $log
    flush $failures

    ObjDb::purge
}

close $log
close $failures

exit
