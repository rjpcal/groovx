///////////////////////////////////////////////////////////////////////
//
// object.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun  5 10:23:15 2001
// written: Mon Jan 13 11:08:25 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJECT_H_DEFINED
#define OBJECT_H_DEFINED

#include "util/refcounted.h"
#include "util/uid.h"

namespace Util
{
  class Object;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Util::Object is a base class for objects that are to be exposed to
 * a scripting language. Associates with each object a unique integer
 * identifier. Util::Object's are reference counted for automatic
 * memory management, and are generally passed around via Util::Ref's
 * or Util::SoftRef's, which automatically manage the reference
 * count. Subclass IO::IoObject provides persistence facilities.
 *
 **/
///////////////////////////////////////////////////////////////////////


class Util::Object : public Util::RefCounted
{
protected:
  /// Default constructor.
  Object();

  /// Virtual destructor.
  virtual ~Object();

public:
  /** Returns the unique id for this object. The unique id will always
      be strictly positive; zero is always an invalid unique id. */
  Util::UID id() const;

private:
  Util::UID itsId;
};

static const char vcid_object_h[] = "$Header$";
#endif // !OBJECT_H_DEFINED
