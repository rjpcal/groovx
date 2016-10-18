/** @file visx/responsehandler.h interface for getting user responses
    during a psychophysics experiment */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue May 18 16:21:09 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_RESPONSEHANDLER_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_RESPONSEHANDLER_H_UTC20050626084015_DEFINED

#include "io/io.h"

namespace nub
{
  template <class T> class soft_ref;
}

class Block;
class Toglet;
class Trial;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c ResponseHandler defines an abstract interface for collecting
 * responses from a user during an experiment. However, it says
 * nothing about which modality will provide those responses. The key
 * virtual functions are the actions \c rhXxx(); these are called by
 * \c ExptDriver at appropriate points in the trial sequence.
 *
 **/
///////////////////////////////////////////////////////////////////////

class ResponseHandler : public io::serializable
{
public:
  /// Default constructor.
  ResponseHandler();

  /// Virtual destructor.
  virtual ~ResponseHandler() noexcept;

  virtual void read_from(io::reader& reader) = 0;
  virtual void write_to(io::writer& writer) const = 0;

  /// Called by a \c Trial at the beginning of a trial.
  /** \c ResponseHandler subclasses implement this function to prepare
      listening for responses, etc. */
  virtual void rhBeginTrial(nub::soft_ref<Toglet> widget,
                            Trial& trial) const = 0;

  /// Called by a \c Trial if a trial is aborted.
  /** \c ResponseHandler subclasses implement this function to quit
      listening for responses, etc. */
  virtual void rhAbortTrial() const = 0;

  /// Called by a \c Trial when a trial is complete.
  /** \c ResponseHandler subclasses implement this function to quit
      listening for responses, etc. */
  virtual void rhEndTrial() const = 0;

  /// Called by a \c Trial when the sequence of trials is halted suddenly.
  /** \c ResponseHandler subclasses implement this function to quit
      listening for responses, etc. */
  virtual void rhHaltExpt() const = 0;

  /// Subclasses implement this function to attend to responses.
  virtual void rhAllowResponses(nub::soft_ref<Toglet> widget,
                                Trial& trial) const = 0;

  /// Subclasses implement this function to stop attending to responses.
  virtual void rhDenyResponses() const = 0;

private:
  ResponseHandler(const ResponseHandler&);
  ResponseHandler& operator=(const ResponseHandler&);
};

#endif // !GROOVX_VISX_RESPONSEHANDLER_H_UTC20050626084015_DEFINED
