##############################################################################
###
### SubjectTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### Obj::new Subject ###
test "SubjectTcl-Obj::new Subject" "norm1" {
	 new Subject
} {^[0-9]+$}

### Subject::nameCmd ###
test "SubjectTcl-Subject::name" "normal" {
	 set s [new Subject]
	 Subject::name $s foobar
	 Subject::name $s
} {foobar}

### Subject::directoryCmd ###
test "SubjectTcl-Subject::directory" "normal" {
	 set s [new Subject]
	 Subject::directory $s /foobar/
	 Subject::directory $s
} {/foobar/}
