/** @file visx/timinghandler.h specialized TimingHdlr subclass for
    object categorization experiments */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed May 19 10:56:20 1999
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_VISX_TIMINGHANDLER_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_TIMINGHANDLER_H_UTC20050626084015_DEFINED

#include "visx/timinghdlr.h"

//  #######################################################
//  =======================================================

/// A TimingHdlr subclass for an old "standard" trial sequence.
class TimingHandler : public TimingHdlr
{
protected:
  // Creators
  TimingHandler();

public:
  static TimingHandler* make();

  virtual ~TimingHandler() throw();

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

  // Accessors + Manipulators
  int getAbortWait() const;
  int getInterTrialInterval() const;
  int getStimDur() const;
  int getTimeout() const;

  void setAbortWait(int msec);
  void setInterTrialInterval(int msec);
  void setStimDur(int msec);
  void setTimeout(int msec);

private:
  unsigned int stimdur_start_id;
  unsigned int timeout_start_id;
  unsigned int iti_response_id;
  unsigned int abortwait_abort_id;
};

static const char __attribute__((used)) vcid_groovx_visx_timinghandler_h_utc20050626084015[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TIMINGHANDLER_H_UTC20050626084015_DEFINED
