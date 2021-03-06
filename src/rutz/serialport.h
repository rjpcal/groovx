/** @file rutz/serialport.h character-at-a-time access to serial port
    data */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Mar 29 13:45:35 2000
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

#ifndef GROOVX_RUTZ_SERIALPORT_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_SERIALPORT_H_UTC20050626084020_DEFINED

#include <iosfwd>

namespace rutz
{
  class serial_port;
}

/// rutz::serial_port provides character-at-a-time access to a serial port.
class rutz::serial_port
{
public:
  serial_port(const char* serial_device);

  ~serial_port() { close(); }

  /** Returns the next character from the stream, or EOF (-1) if the
      stream is empty. */
  int get();

  /// Returns the exit status of closing its stream
  int close();

  bool is_closed() const { return (m_stream != nullptr); }

  int exit_status() const { return m_exit_status; }

private:
  int filedes() const { return m_filedes; }

  serial_port(const serial_port&);
  serial_port& operator=(const serial_port&);

  int                m_filedes;
  std::iostream*     m_stream;
  int                m_exit_status;
};

#endif // !GROOVX_RUTZ_SERIALPORT_H_UTC20050626084020_DEFINED
