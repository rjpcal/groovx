///////////////////////////////////////////////////////////////////////
//
// timinghandler.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed May 19 10:56:20 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef TIMINGHANDLER_H_DEFINED
#define TIMINGHANDLER_H_DEFINED

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

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

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

static const char vcid_timinghandler_h[] = "$Id$ $URL$";
#endif // !TIMINGHANDLER_H_DEFINED
