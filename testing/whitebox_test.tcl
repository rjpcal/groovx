##############################################################################
###
### blackbox tests
### Rob Peters
### 24-Jun-2002
### $Id$
###
##############################################################################

set pkgs {
    Dlinktest
    Fstringtest
}

foreach pkg $pkgs {

    package require $pkg

    foreach cmd [info commands ${pkg}::test*] {
	::test "$pkg" "$cmd" $cmd {^$}
    }
}
