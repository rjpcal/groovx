///////////////////////////////////////////////////////////////////////
//
// serialport.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Mar 29 13:46:11 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef SERIALPORT_CC_DEFINED
#define SERIALPORT_CC_DEFINED

#include "util/serialport.h"

#ifdef HAVE_EXT_STDIO_FILEBUF_H
#include <ext/stdio_filebuf.h>
#else
#include "util/stdiobuf.h"
#endif

#include <cstdio>
#include <fcntl.h>
#ifdef HAVE_ISTREAM
#include <istream>
#else
#include <istream.h>
#endif
#include <termios.h>
#include <unistd.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER;

Util::SerialPort::SerialPort(const char* serial_device) :
  itsFiledes(::open(serial_device, O_RDONLY|O_NOCTTY|O_NONBLOCK)),
  itsFilebuf(0),
  itsStream(0),
  itsExitStatus(0)
{
DOTRACE("Util::SerialPort::SerialPort");
  if (itsFiledes != -1)
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
      itsFilebuf = new filebuf_t(fdopen(itsFiledes, "r"), std::ios::in);
#else
      itsFilebuf = new Util::stdiobuf(itsFiledes, std::ios::in, true);
#endif
      itsStream = new std::iostream(itsFilebuf);
    }
}

int Util::SerialPort::get()
{
  if ( isClosed() ) return -1;

  itsStream->clear();
  itsStream->sync();
  return itsStream->get();
}

int Util::SerialPort::close()
{
DOTRACE("Util::SerialPort::close");
  if ( !isClosed() )
    {
      delete itsStream;
      itsStream = 0;

      delete itsFilebuf;
      itsFilebuf = 0;

      itsExitStatus = ::close(itsFiledes);
    }
  return itsExitStatus;
}

static const char vcid_serialport_cc[] = "$Header$";
#endif // !SERIALPORT_CC_DEFINED
