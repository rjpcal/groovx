# Created 3-Nov-2000
# $Id$

set files {expt215302Aug1999.asw.gz \
		  expt215012Jan2000.asw.gz \
		  expt232423May2000.asw.gz \
		  expt080905Oct2000.asw.gz \
		  train_2_fishes_or.asw.gz \
		  pairs_mfaces_s50.asw.gz}

set idx [lsearch -exact $argv -output]
if { $idx < 0 } { error "can't find argument for output file" }

set outfile [lindex $argv [expr $idx + 1]]

set o [open $outfile a]

proc print {args} {
	 eval puts -nonewline $args
	 eval puts -nonewline $::o $args
}

proc print_time {msg} {
	 set usec [lindex $msg 0]
	 set msec [expr $usec/1000.0]
	 print "${msec} msec"
	 return $msec
}

print "\n-----------------------------------------------------------------"

print "\n[clock format [clock seconds]]"
print "\n[info nameofexecutable]\n"

set LOAD_TIME 0
set SAVE_TIME 0

foreach f $files {
	 Expt::clear
	 IO::clear
	 print "\nloading testing/${f}... "
	 set t [time {Expt::load testing/$f}]
	 set msec [print_time $t]
	 set ::LOAD_TIME [expr $::LOAD_TIME + $msec]

	 print "\nsaving... "
	 set t [time {Expt::save temp.asw}]
	 exec rm temp.asw
	 set msec [print_time $t]
	 set ::SAVE_TIME [expr $::SAVE_TIME + $msec]
}

print "\n\nTOTAL LOAD TIME $::LOAD_TIME msec"
print "\nTOTAL SAVE TIME $::SAVE_TIME msec"

print "\n\n"

close $o

exit