##############################################################################
###
### io_test.tcl
### Rob Peters
### Oct-1999
### $Id$
###
##############################################################################

# Multiple inclusion guard
if { [info exists IO::TEST_DEFINED] } return;


namespace eval IO {

variable TEST_DEFINED 1

proc testStringifyCmd { packagename classname item_argn {item_id -1} } {
	 set cmdname "${classname}::stringify"
	 set testname "${packagename}-${cmdname}"

	 set usage ""

	 if { $item_argn == 1 } {
		  set usage "wrong \# args: should be \"$cmdname item_id\""
	 }

	 if { $item_argn == -1 } {
		  set usage "wrong \# args: should be \"$cmdname\""
	 }

	 if { $item_argn == 1 } {
		  eval ::test $testname {"too few args"} {"
				$cmdname
		  "} {$usage}
		  eval ::test $testname {"too many args"} {"
				$cmdname 0 junk
		  "} {$usage}
		  eval ::test $testname {"error from negative id"} {"
				$cmdname -1
		  "} {"${cmdname}: attempt to access invalid id in .*$"}
		  eval ::test $testname {"error from too large id"} {"
				$cmdname 10000
		  "} {"${cmdname}: attempt to access invalid id in .*$"}
		  eval ::test $testname {"error from non-integral id"} {"
				$cmdname 1.5
		  "} {"expected integer but got \"1\.5\""}

		  if { $item_id >= 0 } {
				eval ::test $testname {"normal use"} {"
		          catch {$cmdname $item_id}
            "} {^0$}
		  }
	 }
}

proc testDestringifyCmd { packagename classname item_argn {item_id -1} } {
	 set cmdname "${classname}::destringify"
	 set stringify "${classname}::stringify"
	 set testname "${packagename}-${cmdname}"

	 set usage ""

	 if { $item_argn == 1 } {
		  set usage "wrong \# args: should be \"$cmdname item_id string\""
	 }

	 if { $item_argn == -1 } {
		  set usage "wrong \# args: should be \"$cmdname string\""
	 }


	 if { $item_argn == 1 } {
		  eval ::test $testname {"too few args"} {"
		      $cmdname 0
		  "} {$usage}
		  eval ::test $testname {"too many args"} {"
		      $cmdname 0 string junk
		  "} {$usage}
		  eval ::test $testname {"error from negative id"} {"
		      $cmdname -1 junk
		  "} {"${cmdname}: attempt to access invalid id in .*$"}
		  eval ::test $testname {"error from too large id"} {"
		      $cmdname 10000 junk
		  "} {"${cmdname}: attempt to access invalid id in .*$"}
		  eval ::test $testname {"error from non-integral id"} {"
				$cmdname 1.5 junk
		  "} {"expected integer but got \"1\.5\""}

		  if { $item_id >= 0 } {
				eval ::test $testname {"normal use"} {"
		          set str \[$stringify $item_id\] 
		          catch {$cmdname $item_id \$str}
            "} {^0$}
				eval ::test $testname {"error on junk"} {"
		          catch {$cmdname $item_id junk}
				"} {^1$}
				
		  }
	 }
}

proc testWriteCmd { packagename classname item_argn {item_id -1} } {
	 set cmdname "${classname}::write"
	 set testname "${packagename}-${cmdname}"

	 set usage ""

	 if { $item_argn == 1 } {
		  set usage "wrong \# args: should be \"$cmdname item_id\""
	 }

	 if { $item_argn == -1 } {
		  set usage "wrong \# args: should be \"$cmdname\""
	 }

	 if { $item_argn == 1 } {
		  eval ::test $testname {"too few args"} {"
				$cmdname
		  "} {$usage}
		  eval ::test $testname {"too many args"} {"
				$cmdname 0 junk
		  "} {$usage}
		  eval ::test $testname {"error from negative id"} {"
				$cmdname -1
		  "} {"${cmdname}: attempt to access invalid id in .*$"}
		  eval ::test $testname {"error from too large id"} {"
				$cmdname 10000
		  "} {"${cmdname}: attempt to access invalid id in .*$"}
		  eval ::test $testname {"error from non-integral id"} {"
				$cmdname 1.5
		  "} {"expected integer but got \"1\.5\""}

		  if { $item_id >= 0 } {
				eval ::test $testname {"normal use"} {"
		          catch {$cmdname $item_id}
            "} {^0$}
		  }
	 }
}

proc testReadCmd { packagename classname item_argn {item_id -1} } {
	 set readcmd "${classname}::read"
	 set writecmd "${classname}::write"
	 set testname "${packagename}-${readcmd}"

	 set usage ""

	 if { $item_argn == 1 } {
		  set usage "wrong \# args: should be \"$readcmd item_id string\""
	 }

	 if { $item_argn == -1 } {
		  set usage "wrong \# args: should be \"$readcmd string\""
	 }


	 if { $item_argn == 1 } {
		  eval ::test $testname {"too few args"} {"
		      $readcmd 0
		  "} {$usage}
		  eval ::test $testname {"too many args"} {"
		      $readcmd 0 string junk
		  "} {$usage}
		  eval ::test $testname {"error from negative id"} {"
		      $readcmd -1 junk
		  "} {"${readcmd}: attempt to access invalid id in .*$"}
		  eval ::test $testname {"error from too large id"} {"
		      $readcmd 10000 junk
		  "} {"${readcmd}: attempt to access invalid id in .*$"}
		  eval ::test $testname {"error from non-integral id"} {"
				$readcmd 1.5 junk
		  "} {"expected integer but got \"1\.5\""}

		  if { $item_id >= 0 } {
				eval ::test $testname {"normal use"} {"
		          set str \[$writecmd $item_id\] 
		          catch {$readcmd $item_id \$str}
            "} {^0$}
				eval ::test $testname {"error on junk"} {"
		          catch {$readcmd $item_id junk}
				"} {^1$}
				
		  }
	 }
}

}
# end namespace eval IO
