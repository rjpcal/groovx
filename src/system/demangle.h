///////////////////////////////////////////////////////////////////////
//
// demangle.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct 13 10:41:03 1999
// written: Fri Nov 10 17:03:50 2000
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
