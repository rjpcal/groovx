##############################################################################
###
### SubjectTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

### subjectCmd ###
test "SubjectTcl-subject" "args" {
    subject
} {wrong \# args: should be "subject subcommand \?args\.\.\.\?"}
test "SubjectTcl-subject" "normal use" {} $BLANK $no_test
test "SubjectTcl-subject" "error" {
    subject junk
} {subject: invalid command} 

### newSubjectCmd ###
test "SubjectTcl-newSubject" "args" {
    subject new
} {wrong \# args: should be "subject new subjectKey \?subjectName\? \?subjectDir\?"}
test "SubjectTcl-newSubject" "norm1" {
	 catch {subject new rjp1}
} {^0$}
test "SubjectTcl-newSubject" "norm2" {
	 catch {subject new rjp2 Rob}
} {^0$}
test "SubjectTcl-newSubject" "norm3" {
	 catch {subject new rjp3 Rob ~/mydir/}
} {^0$}
test "SubjectTcl-newSubject" "error" {
    subject new rep
	 subject new rep
} {subject: subject already exists}

### subjectCmd ###
test "SubjectTcl-clear" "args" {
    subject
} {wrong \# args: should be "subject subcommand \?args\.\.\.\?"}
test "SubjectTcl-clear" "normal use" { 
	 subject new s1
	 subject new s2
	 subject clear
	 subject list
} {^$}
test "SubjectTcl-clear" "no error" {} $BLANK $no_test

### subjectNameCmd ###
subject new nt name_test
test "SubjectTcl-subjectName" "args" {
    subject name
} {wrong \# args: should be "subject name subjectKey \?subjectName\?"}
test "SubjectTcl-subjectName" "norm1" {
	 subject name nt
} {name_test}
test "SubjectTcl-subjectName" "norm2" {
	 subject name nt new_name
	 subject name nt
} {new_name}
test "SubjectTcl-subjectName" "error" {
    subject name junk
} {subject: no such subject exists}

### subjectDirCmd ###
subject new dt dt_name /dir_test/
test "SubjectTcl-subjectDir" "args" {
    subject dir
} {wrong \# args: should be "subject dir subjectKey \?subjectDir\?"}
test "SubjectTcl-subjectDir" "norm1" {
	 subject dir dt
} {/dir_test/}
test "SubjectTcl-subjectDir" "norm2" {
	 subject dir dt /new_dir/
	 subject dir dt
} {/new_dir/}
test "SubjectTcl-subjectDir" "error" {
	 subject dir junk
} {subject: no such subject exists}

### listSubjectsCmd ###
test "SubjectTcl-listSubjects" "args" {
    subject list junk
} {wrong \# args: should be "subject list"}
test "SubjectTcl-listSubjects" "normal use" {
	 subject clear
	 subject new s1 subj1 /dir1/
	 subject new s2 subj2 /dir2/
	 subject list
} {s1[ \t]+subj1[ \t]+/dir1/
s2[ \t]+subj2[ \t]+/dir2/}
test "SubjectTcl-listSubjects" "no error" {} $BLANK $no_test
	 

