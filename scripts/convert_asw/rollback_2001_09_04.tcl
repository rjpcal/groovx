set f [open "AllFiles" r]

set count 0

while { [gets $f filename] != -1 } {
    if { [string length $filename] == 0 } { continue }

    #
    # Move the old file to a trash location
    #

    set parts [split $filename /]
    set trash_filename /home/rjpeters2/trash/[join $parts |]

    set new_trash_filename /home/rjpeters2/trash2/[join $parts |]

    exec chmod -w $filename
    exec chmod -w $trash_filename

    puts "-------------------------------------------------------------"
    puts [exec ls $filename]
    puts [exec ls $trash_filename]
    puts $new_trash_filename

    exec mv $filename $new_trash_filename
    exec mv $trash_filename $filename
}

exit
