///////////////////////////////////////////////////////////////////////
//
// pipe.h
//
// Copyright (c) 2000-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jan 14 17:33:24 2000
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

#ifndef PIPE_H_DEFINED
#define PIPE_H_DEFINED

#include "util/stdiobuf.h"

#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef HAVE_ISTREAM
#  include <istream>
#else
#  include <istream.h>
#endif

namespace Util
{
  class ShellPipe;
  class PipeFds;
  class ChildProcess;
  class ExecPipe;
}

/// Adapts UNIX-style process pipes to a std::iostream interface.
class Util::ShellPipe
{
public:
  ShellPipe(const char* command, const char* mode);

  ~ShellPipe();

  STD_IO::iostream& stream() { return itsStream; }

  int close();

  bool isClosed() const { return (itsFile == 0) || !itsStream.is_open(); }

  int exitStatus() const { return itsExitStatus; }

private:
  ShellPipe(const ShellPipe&);
  ShellPipe& operator=(const ShellPipe&);

  FILE* itsFile;
  Util::stdiostream itsStream;
  int itsExitStatus;
};

/// An exception-safe wrapper around a pair of file descriptors from pipe().
class Util::PipeFds
{
public:
  /// Create a pipe pair of file descriptors.
  /** Throws an exception if the pipe() call fails. */
  PipeFds();

  /// Destructor closes both file descriptors.
  ~PipeFds() throw();

  int reader() const throw() { return fds[0]; }
  int writer() const throw() { return fds[1]; }

  void closeReader() throw() { if (fds[0] >= 0) close(fds[0]); fds[0] = -1; }
  void closeWriter() throw() { if (fds[0] >= 0) close(fds[1]); fds[1] = -1; }

private:
  PipeFds(const PipeFds&);
  PipeFds& operator=(const PipeFds&);

  int fds[2]; // reading == fds[0], writing == fds[1]
};

/// An exception-safe wrapper around a child process from fork().
class Util::ChildProcess
{
public:
  /// Fork a child process.
  /** Throws an exception if the fork() call fails. */
  ChildProcess();

  /// Destructor waits for child process to complete.
  ~ChildProcess() throw();

  /// Check if we're in the parent or child process after the fork().
  bool inParent() const throw() { return itsPid != 0; }

  /// Wait for child process to complete, and return its status code.
  int wait() throw();

private:
  ChildProcess(const ChildProcess&);
  ChildProcess& operator=(const ChildProcess&);

  int itsChildStatus;
  pid_t itsPid;
};


/// An exception-safe wrapper around a pipe-fork-exec sequence.
class Util::ExecPipe
{
public:
  /// Set up a pipe to a child process with the given argv array.
  /** The mode should be "r" if the parent is reading, and "w" if the
      parent is writing. */
  ExecPipe(const char* m, char* const* argv);

  /// Destructor cleans up child process and the pipe's file descriptors.
  ~ExecPipe() throw();

  /// Get the stream that is linked to the child process.
  STD_IO::iostream& stream() throw();

  /// Wait for child process to complete, return 0 if all is OK, -1 if error.
  int exitStatus() throw();

private:
  bool parentIsReader;
  PipeFds fds;
  ChildProcess p;
  Util::stdiostream* strm;
};

static const char vcid_pipe_h[] = "$Header$";
#endif // !PIPE_H_DEFINED
