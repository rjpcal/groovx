# $Id$

source $::env(HOME)/local/bin/backup_template.tcl

make_backup [pwd] {*.o *.do *.a *.sl a.out *~}
