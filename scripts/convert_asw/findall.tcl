puts $::env(HOME)
exec find $::env(HOME)/ -print | grep {\.asw} > AllFiles
