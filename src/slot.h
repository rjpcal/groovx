///////////////////////////////////////////////////////////////////////
//
// observer.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:37:02 1999
// written: Tue Aug 21 11:45:30 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBSERVER_H_DEFINED
#define OBSERVER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

namespace Util
{
  class Observer;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Along with Signal, implements the Observer design pattern. An
 * Observer can be informed of changes in an Signal by calling
 * connect() on that Signal. Thereafter, the Observer will receive
 * notifications of changes in the Signal via receiveStateChangeMsg().
 *
 **/
///////////////////////////////////////////////////////////////////////

class Util::Observer : public virtual Util::Object {
public:
  /// Virtual destructor.
  virtual ~Observer();

  /// Informs the Observer that one of its subjects has changed.
  virtual void receiveStateChangeMsg() = 0;
};

static const char vcid_observer_h[] = "$Header$";
#endif // !OBSERVER_H_DEFINED
