///////////////////////////////////////////////////////////////////////
//
// utilfwd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun  5 10:47:29 2001
// written: Tue Aug 21 11:43:02 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef UTILFWD_H_DEFINED
#define UTILFWD_H_DEFINED

namespace Util
{
  class RefCounted;
  class Object;

  class Signal;
  class Observer;

  /// This type is used for IoObject unique identifiers
  typedef unsigned long UID;

  //
  // Iterators
  //

  struct IterEnd_t {};

  const IterEnd_t IterEnd = IterEnd_t();

  template <class T> class FwdIter;
  template <class T> class BidirIter;
  template <class T> class RxsIter;
}

static const char vcid_utilfwd_h[] = "$Header$";
#endif // !UTILFWD_H_DEFINED
