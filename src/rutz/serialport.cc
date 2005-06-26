///////////////////////////////////////////////////////////////////////
//
// serialport.cc
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Mar 29 13:46:11 2000
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

#ifndef SERIALPORT_CC_DEFINED
#define SERIALPORT_CC_DEFINED

#include "rutz/serialport.h"

#ifdef HAVE_EXT_STDIO_FILEBUF_H
#include <ext/stdio_filebuf.h>
#else
#include "rutz/stdiobuf.h"
#endif

#include <cstdio>
#include <fcntl.h>
#include <istream>
#include <termios.h>
#include <unistd.h>

#include "rutz/trace.h"
#include "rutz/debug.h"
DBG_REGISTER

rutz::serial_port::serial_port(const char* serial_device) :
  m_filedes(::open(serial_device, O_RDONLY|O_NOCTTY|O_NONBLOCK)),
  m_filebuf(0),
  m_stream(0),
  m_exit_status(0)
{
DOTRACE("rutz::serial_port::serial_port");
  if (m_filedes != -1)
    {
      struct termios ti;

      if ( tcgetattr( filedes(), &ti ) == -1 )
        { close(); return; }

      // input modes:
      //   ignore BREAK condition on input,
      //   ignore framing errors and parity errors
      ti.c_iflag = IGNBRK | IGNPAR;

      // output modes:
      //   none
      ti.c_oflag = 0x0;

      // control modes:
      //    character size 8
      //    ignore modem control lines
      //    baud rate 9600
      //    enable receiver
      ti.c_cflag = CS8 | CLOCAL | B9600 | CREAD;

      // local modes:
      //    none
      ti.c_lflag = 0;

      // control characters
      ti.c_cc[VTIME] = 0;         // intercharacter timer
      ti.c_cc[VMIN]  = 1;         // minimum number of characters to be read

      if ( tcsetattr( filedes(), TCSANOW, &ti) == -1 )
        { close(); return; }

#ifdef HAVE_EXT_STDIO_FILEBUF_H
      typedef __gnu_cxx::stdio_filebuf<char> filebuf_t;
      m_filebuf = new filebuf_t(fdopen(m_filedes, "r"), std::ios::in);
#else
      m_filebuf = new rutz::stdiobuf(m_filedes, std::ios::in, true);
#endif
      m_stream = new std::iostream(m_filebuf);
    }
}

int rutz::serial_port::get()
{
  if ( is_closed() ) return -1;

  m_stream->clear();
  m_stream->sync();
  return m_stream->get();
}

int rutz::serial_port::close()
{
DOTRACE("rutz::serial_port::close");
  if ( !is_closed() )
    {
      delete m_stream;
      m_stream = 0;

      delete m_filebuf;
      m_filebuf = 0;

      m_exit_status = ::close(m_filedes);
    }
  return m_exit_status;
}

static const char vcid_serialport_cc[] = "$Id$ $URL$";
#endif // !SERIALPORT_CC_DEFINED
