///////////////////////////////////////////////////////////////////////
//
// serialport.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Mar 29 13:45:35 2000
// written: Wed Mar 29 13:53:29 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SERIALPORT_H_DEFINED
#define SERIALPORT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FSTREAM_H_DEFINED)
#include <fstream.h>
#define FSTREAM_H_DEFINED
#endif

namespace Util {
  class SerialPort;
}

class Util::SerialPort {
public:
  SerialPort(const char* serial_device);

  ~SerialPort() { close(); }

  /** Returns the next character from the stream, or EOF (-1) if the
      stream is empty. */
  int get()
	 {
		if ( itsClosed ) return -1;

		itsStream.clear();
		itsStream.sync();
		return itsStream.get();
	 }

  /// Returns the exit status of closing its stream
  int close();

  bool isClosed() const { return itsClosed; }

  int exitStatus() const { return itsExitStatus; }

private:
  int filedes() const { return itsFiledes; }

  SerialPort(const SerialPort&);
  SerialPort& operator=(const SerialPort&);

  int itsFiledes;
  fstream itsStream;
  bool itsClosed;
  int itsExitStatus;
};

static const char vcid_serialport_h[] = "$Header$";
#endif // !SERIALPORT_H_DEFINED
