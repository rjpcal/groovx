##############################################################################
###
### whitebox tests
### Rob Peters
### 24-Jun-2002
###
##############################################################################

set pkgs {
    Algotest
    Basesixfourtest
    Fstringtest
    Geomtest
    Mtxtest
    Numtest
    Signaltest
    Tcltimertest
    Vectwotest
}

foreach pkg $pkgs {

    package require $pkg

    foreach cmd [info commands ${pkg}::test*] {
	::test "$pkg" "$cmd" $cmd {^$}
    }
}
