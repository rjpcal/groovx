///////////////////////////////////////////////////////////////////////
//
// object.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun  5 10:23:15 2001
// written: Tue Jun  5 10:30:50 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJECT_H_DEFINED
#define OBJECT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REFCOUNTED_H_DEFINED)
#include "util/refcounted.h"
#endif

namespace Util
{
  class Object;

  typedef unsigned long UID;
}


///////////////////////////////////////////////////////////////////////
/**
 *
 * Base class for objects that are to be exposed to a scripting
 * language. Associates with each object a unique integer identifier.
 *
 **/
///////////////////////////////////////////////////////////////////////


class Util::Object : public Util::RefCounted
{
protected:
  /// Default constructor.
  Object();

  /// Virtual destructor.
  virtual ~Object() = 0;

public:
  /** Returns the unique id for this object. The unique id will always
      be strictly positive; zero will never be a valid unique id. */
  Util::UID id() const;

private:
  Util::UID itsId;
};

static const char vcid_object_h[] = "$Header$";
#endif // !OBJECT_H_DEFINED
