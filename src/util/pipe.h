///////////////////////////////////////////////////////////////////////
//
// pipe.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jan 14 17:33:24 2000
// written: Wed Mar 19 17:58:53 2003
// $Id$
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
