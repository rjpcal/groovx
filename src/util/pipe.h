///////////////////////////////////////////////////////////////////////
//
// pipe.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jan 14 17:33:24 2000
// written: Fri Dec  6 15:35:53 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PIPE_H_DEFINED
#define PIPE_H_DEFINED

#include <cstdio>

#ifdef HAVE_FSTREAM
#  include <fstream>
#else
#  include <fstream.h>
#endif

namespace Util
{
  class Pipe;
}

/// Adapts UNIX-style process pipes to a std::iostream interface.
class Util::Pipe
{
public:
  Pipe(const char* command, const char* mode) :
    itsFile(popen(command, mode)),
#if defined(PRESTANDARD_IOSTREAMS)
    itsStream(),
#elif defined(__GNUC__) && __GNUC__ >= 3
    itsFilebuf(file(),std::ios::in|std::ios::out),
    itsStream(&itsFilebuf),
#else
    itsStream(filedes()),
#endif
    itsClosed(false),
    itsExitStatus(0)
    {
      if ( itsFile != 0 )
        {
#if defined(PRESTANDARD_IOSTREAMS)
          itsStream.attach(filedes());
#elif defined(__GNUC__) && __GNUC__ >= 3
          /* nothing */
#else
          /* nothing */
#endif
        }
      else
        {
          itsClosed = true;
        }
    }

  ~Pipe()
    { close(); }

  STD_IO::iostream& stream() { return itsStream; }

  int close()
    {
      if ( !itsClosed )
        {
#ifdef PRESTANDARD_IOSTREAMS
          itsStream.close();
#elif defined(__GNUC__) && __GNUC__ >= 3
          /* nothing */
#else
          itsStream.close();
#endif
          itsExitStatus = pclose(itsFile);
          itsClosed = true;
        }
      return itsExitStatus;
    }

  bool isClosed() const { return itsClosed || !itsStream.is_open(); }

  int exitStatus() const { return itsExitStatus; }

private:
  FILE* file() const { return itsFile; }
  int filedes() const { return fileno(itsFile); }

  Pipe(const Pipe&);
  Pipe& operator=(const Pipe&);

  FILE* itsFile;
#ifdef PRESTANDARD_IOSTREAMS
  fstream itsStream;
#elif defined(__GNUC__) && __GNUC__ >= 3
  std::filebuf itsFilebuf;
  STD_IO::iostream itsStream;
#else
  STD_IO::fstream itsStream;
#endif
  bool itsClosed;
  int itsExitStatus;
};


static const char vcid_pipe_h[] = "$Header$";
#endif // !PIPE_H_DEFINED
