///////////////////////////////////////////////////////////////////////
//
// pipe.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jan 14 17:33:24 2000
// written: Tue Jun 19 15:04:48 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PIPE_H_DEFINED
#define PIPE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTDIO_DEFINED)
#include <cstdio>
#define CSTDIO_DEFINED
#endif

#ifdef PRESTANDARD_IOSTREAMS
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FSTREAM_H_DEFINED)
#    include <fstream.h>
#    define FSTREAM_H_DEFINED
#  endif
#else
#  include <fstream>
#endif

namespace Util {
  class Pipe;
}

class Util::Pipe {
public:
  Pipe(const char* command, const char* mode) :
    itsFile(popen(command, mode)),
#if defined(PRESTANDARD_IOSTREAMS)
    itsStream(),
#elif defined(GCC_COMPILER) && GCC_COMPILER >= 3
    itsFilebuf(file(),std::ios::in|std::ios::out),
    itsStream(&itsFilebuf),
#else
    itsStream(&itsFilebuf),
#endif
    itsClosed(false),
    itsExitStatus(0)
    {
      if ( itsFile != 0 ) {
#if defined(PRESTANDARD_IOSTREAMS)
        itsStream.attach(filedes());
#elif !defined(GCC_COMPILER) || GCC_COMPILER < 3
        itsFilebuf.open(filedes());
#endif
      }
      else {
        itsClosed = true;
      }
    }

  ~Pipe()
    { close(); }

  STD_IO::iostream& stream() { return itsStream; }

  int close()
    {
      if ( !itsClosed ) {
#ifdef PRESTANDARD_IOSTREAMS
        itsStream.close();
#endif
        itsExitStatus = pclose(itsFile);
        itsClosed = true;
      }
      return itsExitStatus;
    }

  bool isClosed() const { return itsClosed; }

  int exitStatus() const { return itsExitStatus; }

private:
  FILE* file() const { return itsFile; }
  int filedes() const { return fileno(itsFile); }

  Pipe(const Pipe&);
  Pipe& operator=(const Pipe&);

  FILE* itsFile;
#ifdef PRESTANDARD_IOSTREAMS
  fstream itsStream;
#else
  std::filebuf itsFilebuf;
  STD_IO::iostream itsStream;
#endif
  bool itsClosed;
  int itsExitStatus;
};


static const char vcid_pipe_h[] = "$Header$";
#endif // !PIPE_H_DEFINED
