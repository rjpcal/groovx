///////////////////////////////////////////////////////////////////////
//
// serialport.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 29 13:46:11 2000
// written: Thu May 10 12:04:42 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SERIALPORT_CC_DEFINED
#define SERIALPORT_CC_DEFINED

#include "util/serialport.h"

#include <cstdio>
#include <termios.h>
#if !defined(IRIX6)
#  include <sys/fcntl.h>
#else
#  include <fcntl.h>
#endif
#include <unistd.h>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

Util::SerialPort::SerialPort(const char* serial_device) :
  itsFiledes(::open(serial_device, O_RDONLY|O_NOCTTY|O_NONBLOCK)),
  itsStream(),
  itsClosed(false),
  itsExitStatus(0)
{
DOTRACE("Util::SerialPort::SerialPort");
  if (itsFiledes != -1) {
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
	 ti.c_cc[VTIME] = 0;			  // intercharacter timer
	 ti.c_cc[VMIN]  = 1;			  // minimum number of characters to be read

	 if ( tcsetattr( filedes(), TCSANOW, &ti) == -1 )
		{ close(); return; }

	 itsStream.attach(filedes());
  }
  else {
	 itsClosed = true;
  }
}

int Util::SerialPort::close(){
DOTRACE("Util::SerialPort::close");
  if ( !itsClosed ) {
	 itsStream.close();
	 itsExitStatus = ::close(itsFiledes);
	 itsClosed = true;
  }
  return itsExitStatus;
}

static const char vcid_serialport_cc[] = "$Header$";
#endif // !SERIALPORT_CC_DEFINED
