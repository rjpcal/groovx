#!/bin/sh
# \
exec `dirname $0`/groovx "$0" "$@"

# $Id$

# Copyright (c) 2004-2004 Rob Peters <rjpeters at klab dot caltech dot edu>

# A wrapper script to run the code in editor.tcl using the neighboring
# groovx executable. (This way, for example, there can be multiple groovx
# installations on a given system, and each installation's visedit.tcl will
# be run using the groovx from the same installation.)

source [file dirname [info nameofexecutable]]/editor.tcl
