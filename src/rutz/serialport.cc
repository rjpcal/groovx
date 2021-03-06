/** @file rutz/serialport.cc character-at-a-time access to serial port
    data */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Mar 29 13:46:11 2000
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
GVX_DBG_REGISTER

rutz::serial_port::serial_port(const char* serial_device) :
  m_filedes(::open(serial_device, O_RDONLY|O_NOCTTY|O_NONBLOCK)),
  m_stream(nullptr),
  m_exit_status(0)
{
GVX_TRACE("rutz::serial_port::serial_port");
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

      m_stream = new rutz::stdiostream(m_filedes, std::ios::in, true);
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
GVX_TRACE("rutz::serial_port::close");
  if ( !is_closed() )
    {
      delete m_stream;
      m_stream = nullptr;

      m_exit_status = ::close(m_filedes);
    }
  return m_exit_status;
}
