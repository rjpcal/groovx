///////////////////////////////////////////////////////////////////////
//
// traits.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri May 18 16:13:27 2001
// written: Fri May 18 16:15:32 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRAITS_H_DEFINED
#define TRAITS_H_DEFINED

namespace Util {

  template <class T>
  struct TypeTraits {
  };

  template <class T>
  struct TypeTraits<T*> {
	 typedef T Pointee;
  };
}

static const char vcid_traits_h[] = "$Header$";
#endif // !TRAITS_H_DEFINED
