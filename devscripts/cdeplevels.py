#!/usr/bin/env python2

# This is a quick script to build a dependency file suitable for
# inclusion into a Makefile, by scanning for source files ('.cc'
# files) within a source tree passed as a command-line argument.

# $Id$

import cppdeps, os, sys

import profile

paths = [os.path.normpath(sys.argv[1])]

for arg in sys.argv[2:]:
    if arg.startswith("-I"):
        paths.append(arg[2:])

def go(n):
    for i in range(n):
        b = cppdeps.DepBuilder(paths)
        b.buildDepTree()
        b.printCdepLevels(sys.stdout)

#profile.run("go(10)")
go(1)
