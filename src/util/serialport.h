///////////////////////////////////////////////////////////////////////
//
// serialport.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 29 13:45:35 2000
// written: Mon Sep  9 12:19:05 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SERIALPORT_H_DEFINED
#define SERIALPORT_H_DEFINED

#ifdef HAVE_FSTREAM
#  include <fstream>
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FSTREAM_H_DEFINED)
#    include <fstream.h>
#    define FSTREAM_H_DEFINED
#  endif
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
  int get()
    {
      if ( isClosed() ) return -1;

      itsStream->clear();
      itsStream->sync();
      return itsStream->get();
    }

  /// Returns the exit status of closing its stream
  int close();

  bool isClosed() const { return (itsStream != 0); }

  int exitStatus() const { return itsExitStatus; }

private:
  int filedes() const { return itsFiledes; }

  SerialPort(const SerialPort&);
  SerialPort& operator=(const SerialPort&);

  int itsFiledes;

#if defined(__GNUC__) && __GNUC__ >= 3
  std::filebuf* itsFilebuf;
  std::iostream* itsStream;
#else
  STD_IO::fstream* itsStream;
#endif
  int itsExitStatus;
};

static const char vcid_serialport_h[] = "$Header$";
#endif // !SERIALPORT_H_DEFINED
