set f [open "AllFiles" r]

set failures [open "FailedFiles" w]

while { [gets $f filename] != -1 } {
    puts -nonewline "${filename}..."
    set code [catch {Expt::load $filename} result]
    if { $code == 0 } {
	puts "ok"
    } else {
	puts "failed: $result"
	puts $failures $filename
    }
    Expt::clear
    ObjDb::clear
}

close $failures
close $f

exit
