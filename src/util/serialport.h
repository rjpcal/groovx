///////////////////////////////////////////////////////////////////////
//
// serialport.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Mar 29 13:45:35 2000
// written: Wed Mar 19 12:45:36 2003
// $Id$
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

namespace Util
{
  class SerialPort;
}

/// Util::SerialPort provides character-at-a-time access to a serial port.
class Util::SerialPort
{
public:
  SerialPort(const char* serial_device);

  ~SerialPort() { close(); }

  /** Returns the next character from the stream, or EOF (-1) if the
      stream is empty. */
  int get();

  /// Returns the exit status of closing its stream
  int close();

  bool isClosed() const { return (itsStream != 0); }

  int exitStatus() const { return itsExitStatus; }

private:
  int filedes() const { return itsFiledes; }

  SerialPort(const SerialPort&);
  SerialPort& operator=(const SerialPort&);

  int itsFiledes;
  STD_IO::streambuf* itsFilebuf;
  STD_IO::iostream* itsStream;
  int itsExitStatus;
};

static const char vcid_serialport_h[] = "$Header$";
#endif // !SERIALPORT_H_DEFINED
