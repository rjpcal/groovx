set fd [open "aswfiles" r]
set files [split [read $fd] "\n"]
close $fd

set failures [open "failed" w]
set log [open "rechecklog" w]

set i 0

foreach fname $files {
    if { [string length $fname] == 0 } { continue }

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

    puts -nonewline $log "[incr i] ${dest}..."

    set code [catch {set xp [IO::loadXML $dest]} result]

    if { $code == 0 } {
	puts $log "recheck ok"
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
