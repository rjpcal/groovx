///////////////////////////////////////////////////////////////////////
//
// serialport.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 29 13:46:11 2000
// written: Mon Sep  9 11:44:31 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SERIALPORT_CC_DEFINED
#define SERIALPORT_CC_DEFINED

#include "util/serialport.h"

#include <cstdio>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#if defined(__GNUC__) && __GNUC__ >= 3
#include <ext/stdio_filebuf.h>
#endif

#include "util/trace.h"
#include "util/debug.h"

Util::SerialPort::SerialPort(const char* serial_device) :
  itsFiledes(::open(serial_device, O_RDONLY|O_NOCTTY|O_NONBLOCK)),
#if defined(__GNUC__) && __GNUC__ >= 3
  itsFilebuf(0),
#endif
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

#if defined(__GNUC__) && __GNUC__ >= 3
      typedef __gnu_cxx::stdio_filebuf<char> filebuf_t;
      itsFilebuf = new filebuf_t(fdopen(filedes(), "r"), std::ios::in);
      itsStream = new std::iostream(itsFilebuf);
#else
      itsStream = new STD_IO::fstream;
      itsStream->attach(filedes());
#endif
    }
}

int Util::SerialPort::close()
{
DOTRACE("Util::SerialPort::close");
  if ( !isClosed() )
    {
      delete itsStream;
      itsStream = 0;

#if defined(__GNUC__) && __GNUC__ >= 3
      delete itsFilebuf;
      itsFilebuf = 0;
#endif

      itsExitStatus = ::close(itsFiledes);
    }
  return itsExitStatus;
}

static const char vcid_serialport_cc[] = "$Header$";
#endif // !SERIALPORT_CC_DEFINED
