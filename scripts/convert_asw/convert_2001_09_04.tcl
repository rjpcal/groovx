set f [open "AllFiles" r]

set count 0

while { [gets $f filename] != -1 } {
    if { [string length $filename] == 0 } { continue }

    Expt::clear
    ObjDb::clear

    puts "Obj::counts ---> [Obj::countAll]"

    incr ::count

    puts "-------------------------------------------------------------"
    puts "$count"
    puts -nonewline "loading ${filename}..."

    #
    # Load the old file
    #

    set code [catch {Expt::load $filename} result]

    if { $code } {
	puts "failed: $result"
	continue
    } else {
	puts "ok"
    }

    #
    # Save the new file
    #

    set temp_filename ./temp.asw.gz

    puts -nonewline "saving ${temp_filename}..."

    set code [catch {Expt::save $temp_filename} result]

    if { $code } {
	puts "failed: $result"
	continue
    } else {
	puts "ok"
    }

    #
    # Try loading the new file
    #

    puts -nonewline "loading ${temp_filename}..."

    set code [catch {Expt::load $temp_filename} result]

    if { $code } {
	puts "failed: $result"
	continue
    } else {
	puts "ok"
    }

    #
    # Move the old file to a trash location
    #

    set parts [split $filename /]
    set trash_filename /home/rjpeters2/trash2/[join $parts |]

    exec chmod -w $filename
    exec chmod -w $temp_filename
    exec mv $filename $trash_filename
    exec mv $temp_filename $filename
}

exit
