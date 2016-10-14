#!/usr/bin/env tclsh

# A tcl script for generating compile-time dependency information.


source [file dirname [info script]]/deps.tcl

depconf::parse_argv $argv

cdeps::batch_print $depconf::srcdir $depconf::objdir $depconf::user_ipath
