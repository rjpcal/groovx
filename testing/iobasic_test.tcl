##############################################################################
###
### io_test.tcl
### Rob Peters
### Oct-2004
### $Id$
###
##############################################################################

::test IO::writeLGX "too few args" {
    IO::writeLGX
} "wrong \# args: should be "

::test IO::writeLGX "too many args" {
    IO::writeLGX 0 junk
} "wrong \# args: should be "

::test IO::writeLGX "error from negative id" {
    IO::writeLGX -1
} {expected.*but got}

::test IO::writeLGX "error from too large id" {
    IO::writeLGX 10000000
} {attempted to access invalid object.*$}

::test IO::writeLGX "error from non-integral id" {
    IO::writeLGX 1.5
} {expected.*but got}

::test IO::readLGX "too few args" {
    IO::readLGX 0
} "wrong \# args: should be "

::test IO::readLGX "too many args" {
    IO::readLGX 0 string junk
} "wrong \# args: should be "

::test IO::readLGX "error from negative id" {
    IO::readLGX -1 junk
} {expected.*but got}

::test IO::readLGX "error from too large id" {
    IO::readLGX 10000000 junk
} {attempted to access invalid object.*$}

::test IO::readLGX "error from non-integral id" {
    IO::readLGX 1.5 junk
} {expected.*but got}

::test IO::writeASW "too few args" {
    IO::writeASW
} "wrong \# args: should be "

::test IO::writeASW "too many args" {
    IO::writeASW 0 junk
} "wrong \# args: should be "

::test IO::writeASW "error from negative id" {
    IO::writeASW -1
} {expected.*but got}

::test IO::writeASW "error from too large id" {
    IO::writeASW 10000000
} {attempted to access invalid object.*$}

::test IO::writeASW "error from non-integral id" {
    IO::writeASW 1.5
} {expected.*but got}

::test IO::readASW "too few args" {
    IO::readASW 0
} "wrong \# args: should be "

::test IO::readASW "too many args" {
    IO::readASW 0 string junk
} "wrong \# args: should be "

::test IO::readASW "error from negative id" {
    IO::readASW -1 junk
} {expected.*but got}

::test IO::readASW "error from too large id" {
    IO::readASW 10000000 junk
} {attempted to access invalid object.*$}

::test IO::readASW "error from non-integral id" {
    IO::readASW 1.5 junk
} {expected.*but got}
