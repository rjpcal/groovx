///////////////////////////////////////////////////////////////////////
//
// volatileobject.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Aug 21 17:16:15 2001
// written: Tue Jul  2 13:22:01 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VOLATILEOBJECT_H_DEFINED
#define VOLATILEOBJECT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

namespace Util
{
  class VolatileObject;
}

/// Subclass of Util::Object for inherently un-shareable objects.
/** This typically applies to objects who must control their own lifetime,
    or whose lifetime is controlled by some external mechanism (such as a
    windowing system, for example). */
class Util::VolatileObject : public virtual Util::Object
{
public:
  VolatileObject();
  virtual ~VolatileObject();

  void destroy();

  virtual bool isNotShareable() const;
};

static const char vcid_volatileobject_h[] = "$Header$";
#endif // !VOLATILEOBJECT_H_DEFINED
