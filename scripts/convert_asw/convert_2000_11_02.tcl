set f [open "ExptFiles2" r]

set count 0

while { [gets $f filename] != -1 } {
    if { [string length $filename] == 0 } { continue }

    Expt::clear
    IO::clear

    puts "counts ---> [BlockList::count] [Tlist::count] \
				[RhList::count] [ThList::count] [ObjList::count] [PosList::count]"

    incr ::count

    puts "-------------------------------------------------------------"
    puts "$count"
    puts -nonewline "loading ${filename}..."

    set code [catch {Expt::load $filename} result]

    set temp_filename ${filename}.temp.gz

    if { $code } {
	puts "failed: $result"
	continue
    } else {
	puts "ok"
    }

    puts -nonewline "saving ${temp_filename}..."

    set code [catch {Expt::save $temp_filename} result]

    if { $code } {
	puts "failed: $result"
	continue
    } else {
	puts "ok"
    }

    puts -nonewline "loading ${temp_filename}..."

    set code [catch {Expt::load $temp_filename} result]

    if { $code } {
	puts "failed: $result"
	continue
    } else {
	puts "ok"
    }

    set parts [split $filename /]
    set trash_filename ./old_expt_files/[join $parts |]

    exec chmod -w $filename
    exec chmod -w $temp_filename
    exec mv $filename $trash_filename
    exec mv $temp_filename $filename
}

exit
