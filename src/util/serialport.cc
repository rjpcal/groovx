///////////////////////////////////////////////////////////////////////
//
// serialport.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Mar 29 13:46:11 2000
// written: Wed Mar 29 13:51:07 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SERIALPORT_CC_DEFINED
#define SERIALPORT_CC_DEFINED

#include "util/serialport.h"

#include <cstdio>
#include <termios.h>
#include <sys/fcntl.h>
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

	 tcgetattr( filedes(), &ti );

	 ti.c_iflag = IGNBRK | IGNPAR;
	 ti.c_oflag = 0x0;
	 ti.c_cflag = CS8 | CLOCAL | B9600 | CREAD;
	 ti.c_lflag = 0;
	 ti.c_cc[VTIME] = 0;
	 ti.c_cc[VMIN]  = 1;

	 tcsetattr( filedes(), TCSANOW, &ti);

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
