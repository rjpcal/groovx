///////////////////////////////////////////////////////////////////////
//
// element.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Dec  4 15:35:37 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef ELEMENT_H_DEFINED
#define ELEMENT_H_DEFINED

#include "io/io.h"

namespace Util
{
  template <class T> class SoftRef;
}

class Toglet;

class fstring;

//  ###################################################################
//  ===================================================================

/// Element is an abstract base class for all experiment components.
/** This conceptually includes individual trials, blocks, experiment
    sessions, series of multiple experiments, etc. */

class Element : public IO::IoObject
{
public:
  /// Virtual destructor.
  virtual ~Element() throw();

  /// Status type for child to tell its parent how it ended.
  enum ChildStatus
    {
      CHILD_OK,
      CHILD_ABORTED,
      CHILD_REPEAT
    };

  /// Return the Widget in which the experiment is running.
  virtual const Util::SoftRef<Toglet>& getWidget() const = 0;

  /// Get the user-defined "trial type".
  virtual int trialType() const = 0;

  /// Get the value of the last response.
  virtual int lastResponse() const = 0;

  /// Get a string describing the object and/or its current state.
  virtual fstring vxInfo() const = 0;

  /// Run this element with a reference to its parent.
  virtual void vxRun(Element& parent) = 0;

  /// Halt this element.
  virtual void vxHalt() const = 0;

  /// Hook function that gets called each time a leaf (i.e., trial) completes.
  /** Shouldn't be used for substantive action, but just for bookkeeping
      such as timekeeping, autosaving, etc. Default version is a no-op. */
  virtual void vxEndTrialHook();

  /// Called when an element's child finishes running.
  virtual void vxReturn(ChildStatus s) = 0;

  /// Undo one trial.
  virtual void vxUndo() = 0;

  /// Reset this element to the starting state.
  virtual void vxReset() = 0;
};

static const char vcid_element_h[] = "$Header$";
#endif // !ELEMENT_H_DEFINED
