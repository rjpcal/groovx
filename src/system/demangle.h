///////////////////////////////////////////////////////////////////////
//
// demangle.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Oct 13 10:41:03 1999
// written: Wed Mar 19 12:45:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEMANGLE_H_DEFINED
#define DEMANGLE_H_DEFINED

// Warning: the result of this function is only valid until the next
// call to the function.
const char* demangle_cstr(const char* in);

static const char vcid_demangle_h[] = "$Header$";
#endif // !DEMANGLE_H_DEFINED
