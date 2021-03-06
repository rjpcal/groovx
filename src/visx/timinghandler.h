/** @file visx/timinghandler.h specialized TimingHdlr subclass for
    object categorization experiments */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed May 19 10:56:20 1999
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

#ifndef GROOVX_VISX_TIMINGHANDLER_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_TIMINGHANDLER_H_UTC20050626084015_DEFINED

#include "visx/timinghdlr.h"

#include <cstddef>

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

  virtual ~TimingHandler() noexcept;

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  // Accessors + Manipulators
  unsigned int getAbortWait() const;
  unsigned int getInterTrialInterval() const;
  unsigned int getStimDur() const;
  unsigned int getTimeout() const;

  void setAbortWait(unsigned int msec);
  void setInterTrialInterval(unsigned int msec);
  void setStimDur(unsigned int msec);
  void setTimeout(unsigned int msec);

private:
  size_t stimdur_start_id;
  size_t timeout_start_id;
  size_t iti_response_id;
  size_t abortwait_abort_id;
};

#endif // !GROOVX_VISX_TIMINGHANDLER_H_UTC20050626084015_DEFINED
