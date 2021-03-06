/** @file rutz/tracer.h dynamically control the behavior of GVX_TRACE
    statements */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Feb  8 17:48:17 2000
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

#ifndef GROOVX_RUTZ_TRACER_H_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_TRACER_H_UTC20050626084019_DEFINED

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

#endif // !GROOVX_RUTZ_TRACER_H_UTC20050626084019_DEFINED
