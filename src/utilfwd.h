///////////////////////////////////////////////////////////////////////
//
// utilfwd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun  5 10:47:29 2001
// written: Fri Aug 17 14:11:11 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef UTILFWD_H_DEFINED
#define UTILFWD_H_DEFINED

namespace Util
{
  class RefCounted;
  class Object;

  class Observable;
  class Observer;

  /// This type is used for IoObject unique identifiers
  typedef unsigned long UID;

  //
  // Iterators
  //

  struct IterEnd_t {};

  const IterEnd_t IterEnd = IterEnd_t();

  template <class T> class FwdIterIfx;
  template <class T> class BidirIterIfx;
  template <class T> class RxsIterIfx;

  template <class Iter, class T> class FwdIterAdapter;
  template <class Iter, class T> class BidirIterAdapter;
  template <class Iter, class T> class RxsIterAdapter;

  template <class T> class FwdIter;
  template <class T> class BidirIter;
  template <class T> class RxsIter;

  template <class T, class Ifx> class ConcreteIter;
}

static const char vcid_utilfwd_h[] = "$Header$";
#endif // !UTILFWD_H_DEFINED
