#!/usr/bin/env python2

# This is a quick script to build a dependency file suitable for
# inclusion into a Makefile, by scanning for source files ('.cc'
# files) within a source tree passed as a command-line argument.

# $Id$

import cppdeps, os, sys

b = cppdeps.DepBuilder(os.path.normpath(sys.argv[1]))
b.buildDepTree()
b.printMakeDeps(sys.stdout)
