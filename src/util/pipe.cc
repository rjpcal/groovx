///////////////////////////////////////////////////////////////////////
//
// pipe.cc
//
// Copyright (c) 2003-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri May  2 16:38:30 2003
// commit: $Id$
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

#ifndef PIPE_CC_DEFINED
#define PIPE_CC_DEFINED

#include "util/pipe.h"

#include "util/error.h"
#include "util/strings.h"

#include "util/debug.h"
DBG_REGISTER;

Util::ShellPipe::ShellPipe(const char* command, const char* mode) :
  itsFile(popen(command, mode)),
  itsStream(itsFile, std::ios::in|std::ios::out),
  itsExitStatus(0)
{}

Util::ShellPipe::~ShellPipe()
{ close(); }

int Util::ShellPipe::close()
{
  if ( !isClosed() )
    {
      itsStream.close();
      itsExitStatus = pclose(itsFile);
      itsFile = 0;
    }
  return itsExitStatus;
}

Util::PipeFds::PipeFds()
{
  if (pipe(fds) != 0)
    throw Util::Error("couldn't create pipe");
}

Util::PipeFds::~PipeFds() throw()
{
  closeReader();
  closeWriter();
}

Util::ChildProcess::ChildProcess() :
  itsChildStatus(0),
  itsPid(fork())
{
  if (itsPid == -1)
    throw Util::Error("couldn't fork child process");
}

Util::ChildProcess::~ChildProcess() throw()
{
  wait();
}

int Util::ChildProcess::wait() throw()
{
  if (itsPid != 0)
    {
      waitpid(itsPid, &itsChildStatus, /*options*/ 0);
      itsPid = 0;
    }

  return itsChildStatus;
}

namespace
{
  bool isReadMode(const char* m)
  {
    if (m == 0) throw Util::Error("invalid read/write mode");
    switch (m[0])
      {
      case 'r': return true;
      case 'w': return false;
      }

    throw Util::Error(fstring("invalid read/write mode '", m, "'"));
    return false; // "can't happen"
  }
}

Util::ExecPipe::ExecPipe(const char* m, char* const* argv) :
  parentIsReader(isReadMode(m)),
  fds(),
  p(),
  strm(0)
{
  if (p.inParent())
    {
      if (parentIsReader)
        {
          fds.closeWriter();

          strm = new Util::stdiostream(fds.reader(),
                                       std::ios::in|std::ios::binary);
        }
      else // parent is writer
        {
          fds.closeReader();

          strm = new Util::stdiostream(fds.writer(),
                                       std::ios::out|std::ios::binary);
        }

      if (strm == 0)
        throw Util::Error("couldn't open stream in parent process");
    }
  else // in child
    {
      if (parentIsReader) // ==> child is writer
        {
          fds.closeReader();

          if (dup2(fds.writer(), STDOUT_FILENO) == -1)
            {
              fprintf(stderr, "dup2 failed in child process\n");
              exit(-1);
            }
        }
      else // parent is writer, child is reader
        {
          fds.closeWriter();

          if (dup2(fds.reader(), STDIN_FILENO) == -1)
            {
              fprintf(stderr, "dup2 failed in child process\n");
              exit(-1);
            }
        }

      execv(argv[0], argv);

      fprintf(stderr, "execv failed in child process\n");
      exit(-1);
    }
}


Util::ExecPipe::~ExecPipe() throw()
{
  delete strm;
}

STD_IO::iostream& Util::ExecPipe::stream() throw()
{
  Assert(strm != 0);
  return *strm;
}

int Util::ExecPipe::exitStatus() throw()
{
  const int child_status = p.wait();

  // Check if the child process exited abnormally
  if (WIFEXITED(child_status) == 0) return -1;

  // Check if the child process gave an error exit code
  if (WEXITSTATUS(child_status) != 0) return -1;

  // OK, everything looks fine
  return 0;
}

static const char vcid_pipe_cc[] = "$Header$";
#endif // !PIPE_CC_DEFINED
