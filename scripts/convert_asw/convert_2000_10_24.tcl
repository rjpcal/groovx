set f [open "AswFiles" r]

set count 0

while { [gets $f filename] != -1 } {
    if { [string length $filename] == 0 } { continue }

    if { ![regsub {\.asw} $filename ".asw_new" new_filename] } {
	continue
    }

    Expt::clear
    BlockList::reset
    Tlist::reset
    RhList::reset
    ThList::reset
    ObjList::reset
    PosList::reset

    puts "counts ---> [BlockList::count] [Tlist::count] \
				[RhList::count] [ThList::count] [ObjList::count] [PosList::count]"

    incr ::count

    puts "-------------------------------------------------------------"
    puts "$count"
    puts -nonewline "loading ${filename}..."

    set code [catch {Expt::load $filename} result]

    if { $code } {
	puts "failed: $result"
	continue
    } else {
	puts "ok"
    }

    puts -nonewline "saving ${new_filename}..."

    set code [catch {Expt::save $new_filename} result]

    if { $code } {
	puts "failed: $result"
	continue
    } else {
	puts "ok"
    }

    puts -nonewline "loading ${new_filename}..."

    set code [catch {Expt::load $new_filename} result]

    if { $code } {
	puts "failed: $result"
	continue
    } else {
	puts "ok"
    }
}

exit
