set fd [open "aswfiles" r]
set files [split [read $fd] "\n"]
close $fd

foreach fname $files {
    if { [string length $fname] == 0 } { continue }

    if { ![file exists $fname] } {
	set alt /cit/rjpeters/oldasw/$fname
	if { ![file exists $alt] } {
	    error "can't find $fname"
	} else {
	    file rename $alt $fname
	    puts "retrieved $alt"
	}
    }
}

exit
