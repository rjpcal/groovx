# $Id$

source $::env(HOME)/local/bin/backup_template.tcl

make_backup /cit/rjpeters/sorcery/grsh {*.o *.do *.a *.sl a.out *~}
