///////////////////////////////////////////////////////////////////////
//
// pipe.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jan 14 17:33:24 2000
// written: Thu Feb 27 16:34:37 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PIPE_H_DEFINED
#define PIPE_H_DEFINED

#include "util/stdiobuf.h"

#include <cstdio>

#ifdef HAVE_ISTREAM
#  include <istream>
#else
#  include <istream.h>
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
    itsFilebuf(file(), std::ios::in|std::ios::out),
    itsStream(&itsFilebuf),
    itsExitStatus(0)
  {}

  ~Pipe()
    { close(); }

  STD_IO::iostream& stream() { return itsStream; }

  int close()
    {
      if ( !isClosed() )
        {
          itsFilebuf.close();
          itsExitStatus = pclose(itsFile);
          itsFile = 0;
        }
      return itsExitStatus;
    }

  bool isClosed() const { return (itsFile == 0) || !itsFilebuf.is_open(); }

  int exitStatus() const { return itsExitStatus; }

private:
  FILE* file() const { return itsFile; }
  int filedes() const { return fileno(itsFile); }

  Pipe(const Pipe&);
  Pipe& operator=(const Pipe&);

  FILE* itsFile;
  Util::stdiobuf itsFilebuf;
  STD_IO::iostream itsStream;
  int itsExitStatus;
};


static const char vcid_pipe_h[] = "$Header$";
#endif // !PIPE_H_DEFINED
