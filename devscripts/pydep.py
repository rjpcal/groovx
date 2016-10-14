#!/usr/bin/env python2

# This is a quick script to build a dependency file suitable for
# inclusion into a Makefile, by scanning for source files ('.cc'
# files) within a source tree passed as a command-line argument.


import cppdeps, os, sys

import profile

paths = [os.path.normpath(sys.argv[1])]

objdir = '';

for i in range(2, len(sys.argv)):
    if sys.argv[i].startswith("-I"):
        paths.append(sys.argv[i][2:])
    if sys.argv[i].startswith("--objdir"):
        objdir = sys.argv[i+1]

def go(n):
    for i in range(n):
        b = cppdeps.DepBuilder(paths, objdir)
        b.buildDepTree()
        b.printMakeDeps(sys.stdout)

#profile.run("go(10)")
go(1)
