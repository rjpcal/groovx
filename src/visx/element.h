/** @file visx/element.h abstract base class for psychophysics
    experiment components */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Dec  4 15:35:37 2002
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_VISX_ELEMENT_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_ELEMENT_H_UTC20050626084015_DEFINED

#include "io/io.h"

namespace nub
{
  template <class T> class soft_ref;
}

namespace rutz
{
  class fstring;
}

class Toglet;

//  #######################################################
//  =======================================================

/// Element is an abstract base class for all experiment components.
/** This conceptually includes individual trials, blocks, experiment
    sessions, series of multiple experiments, etc. */

class Element : public io::serializable
{
public:
  /// Virtual destructor.
  virtual ~Element() noexcept;

  /// Status type for child to tell its parent how it ended.
  enum class ChildStatus
    {
      OK,
      ABORTED,
      REPEAT
    };

  /// Return the Widget in which the experiment is running.
  virtual const nub::soft_ref<Toglet>& getWidget() const = 0;

  /// Get the user-defined "trial type".
  virtual int trialType() const = 0;

  /// Get the value of the last response.
  virtual int lastResponse() const = 0;

  /// Get a string describing the object and/or its current state.
  virtual rutz::fstring vxInfo() const = 0;

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

#endif // !GROOVX_VISX_ELEMENT_H_UTC20050626084015_DEFINED
