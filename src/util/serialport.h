///////////////////////////////////////////////////////////////////////
//
// serialport.h
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Mar 29 13:45:35 2000
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

#ifndef SERIALPORT_H_DEFINED
#define SERIALPORT_H_DEFINED

#ifdef HAVE_IOSFWD
#  include <iosfwd>
#else
class iostream;
class streambuf;
#endif

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
  STD_IO::streambuf* m_filebuf;
  STD_IO::iostream*  m_stream;
  int                m_exit_status;
};

static const char vcid_serialport_h[] = "$Id$ $URL$";
#endif // !SERIALPORT_H_DEFINED
