#!/usr/bin/env tclsh

# A tcl script for generating link-time dependency information.


source [file dirname [info script]]/deps.tcl

set default_args \
    "--src ./src --objdir ./obj/$env(ARCH) --libdir $env(HOME)/local/$env(ARCH)/lib/"

set args [depconf::parse_argv [concat $default_args $argv]]

set lmapping [subst -nobackslashes -nocommands {
    {.*\\.h {}}
    {${depconf::srcdir}/(.*)\\.cc ${depconf::objdir}/\\1.do}
    {${depconf::objdir}/gfx/.*\\.do ${depconf::libdir}/libDeepGfx.d.so}
    {${depconf::objdir}/gwt/.*\\.do ${depconf::libdir}/libDeepGwt.d.so}
    {${depconf::objdir}/gx/.*\\.do ${depconf::libdir}/libDeepGx.d.so}
    {${depconf::objdir}/io/.*\\.do ${depconf::libdir}/libDeepIO.d.so}
    {${depconf::objdir}/system/.*\\.do ${depconf::libdir}/libDeepSystem.d.so}
    {${depconf::objdir}/tcl/.*\\.do ${depconf::libdir}/libDeepTcl.d.so}
    {${depconf::objdir}/togl/.*\\.do ${depconf::libdir}/libDeepTogl.d.so}
    {${depconf::objdir}/util/.*\\.do ${depconf::libdir}/libDeepUtil.d.so}
    {${depconf::objdir}/visx/.*\\.do ${depconf::libdir}/libDeepVisx.d.so}
}]

puts [ldeps::get_batch $args $depconf::srcdir $lmapping]

exit
