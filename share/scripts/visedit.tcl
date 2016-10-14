#!/bin/sh

# We sneak the pathname to the 'bin' directory into the tcl code via
# an environment variable, GROOVX_BINDIR. We need this because it's
# possible (e.g. on Mac OS X) that the groovx executable is itself a
# script that execs a different file (in order to get at an executable
# nestled inside Groovx.app/Contents/MacOS, for example). When that
# happens, we'll have lost track of the original 'bin' directory,
# except that we've saved it in the GROOVX_BINDIR environment
# variable.

# The next two lines are seen first by /bin/sh as a comment followed
# by a line of shell script. That line of shell script execs the
# groovx executable with this file as its first argument. When groovx
# (i.e. Tcl) re-reads this file, it ignores the next two lines since
# it sees them as a single comment because of the backslash in the
# comment line.

# \
    export GROOVX_BINDIR=`dirname $0`; exec ${GROOVX_BINDIR}/groovx "$0" "$@"


# Copyright (c) 2004-2005 Rob Peters <rjpeters at klab dot caltech dot edu>

# A wrapper script to run the code in editor.tcl using the neighboring
# groovx executable. (This way, for example, there can be multiple
# groovx installations on a given system, and each installation's
# visedit.tcl will be run using the groovx from the same
# installation.)

if { [info exists ::env(GROOVX_BINDIR)] } {
    source $::env(GROOVX_BINDIR)/editor.tcl
} else {
    source [file dirname [info script]]/editor.tcl
}
