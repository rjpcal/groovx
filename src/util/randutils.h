///////////////////////////////////////////////////////////////////////
//
// randutils.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar  8 02:35:53 1999
// written: Fri Jan 18 16:06:54 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RANDUTILS_H_DEFINED
#define RANDUTILS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTDLIB_DEFINED)
#include <cstdlib>
#define CSTDLIB_DEFINED
#endif

namespace Util
{
  template <class T>
  inline T randRange(const T& min, const T& max)
  {
    return T( (double(rand()) / (double(RAND_MAX)+1.0)) * (max-min) + min );
  }
}

static const char vcid_randutils_h[] = "$Header$";
#endif // !RANDUTILS_H_DEFINED
