///////////////////////////////////////////////////////////////////////
//
// slot.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 25 18:37:02 1999
// written: Tue Aug 21 14:24:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SLOT_H_DEFINED
#define SLOT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

namespace Util
{
  class Slot;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Along with Signal, implements the Slot design pattern. An Slot can
 * be informed of changes in an Signal by calling connect() on that
 * Signal. Thereafter, the Slot will receive notifications of changes
 * in the Signal via receiveSignal().
 *
 **/
///////////////////////////////////////////////////////////////////////

class Util::Slot : public virtual Util::Object {
public:
  /// Virtual destructor.
  virtual ~Slot();

  /// Informs the Slot that one of its subjects has changed.
  virtual void receiveSignal() = 0;
};

static const char vcid_slot_h[] = "$Header$";
#endif // !SLOT_H_DEFINED
