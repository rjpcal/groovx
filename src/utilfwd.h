///////////////////////////////////////////////////////////////////////
//
// utilfwd.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun  5 10:47:29 2001
// written: Tue Jun  5 10:52:31 2001
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
}

static const char vcid_utilfwd_h[] = "$Header$";
#endif // !UTILFWD_H_DEFINED
