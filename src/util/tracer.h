///////////////////////////////////////////////////////////////////////
//
// tracer.h
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Feb  8 17:48:17 2000
// commit: $Id$
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

#ifndef TRACER_H_DEFINED
#define TRACER_H_DEFINED

namespace rutz
{
  /// A class for dynamic toggling sets of trace statements.
  class tracer
  {
  public:
    tracer() : m_status(false) {}

    void on()           { m_status = true; }
    void off()          { m_status = false; }
    void toggle()       { m_status = !m_status; }
    bool status() const { return m_status; }

  private:
    bool m_status;
  };
}

static const char vcid_tracer_h[] = "$Header$";
#endif // !TRACER_H_DEFINED
