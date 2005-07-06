/** @file rutz/serialport.h character-at-a-time access to serial port
    data */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Mar 29 13:45:35 2000
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

  bool is_closed() const { return (m_stream != 0); }

  int exit_status() const { return m_exit_status; }

private:
  int filedes() const { return m_filedes; }

  serial_port(const serial_port&);
  serial_port& operator=(const serial_port&);

  int                m_filedes;
  std::streambuf*    m_filebuf;
  std::iostream*     m_stream;
  int                m_exit_status;
};

static const char vcid_groovx_rutz_serialport_h_utc20050626084020[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_SERIALPORT_H_UTC20050626084020_DEFINED
