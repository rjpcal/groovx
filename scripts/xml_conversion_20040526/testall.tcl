set f [open "aswfiles" r]
set files [split [read $f] "\n"]
close $f

set failures [open "failed" w]
set log [open "log" w]

set i 0

foreach filename $files {
    if { [string length $filename] == 0 } { continue }

    puts -nonewline $log "[incr i] ${filename}..."
    set code [catch {IO::retrieve $filename} result]
    if { $code == 0 } {
	puts $log "ok"
    } else {
	puts $log "failed: $result"
	puts $failures $filename
    }

    flush $log
    flush $failures

    ObjDb::purge
}

close $log
close $failures

exit
