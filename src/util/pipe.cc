///////////////////////////////////////////////////////////////////////
//
// pipe.cc
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri May  2 16:38:30 2003
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef PIPE_CC_DEFINED
#define PIPE_CC_DEFINED

#include "util/pipe.h"

#include "util/error.h"
#include "util/fstring.h"

#include "util/debug.h"
DBG_REGISTER

rutz::shell_pipe::shell_pipe(const char* command, const char* mode) :
  m_file(popen(command, mode)),
  m_stream(m_file, std::ios::in|std::ios::out),
  m_exit_status(0)
{}

rutz::shell_pipe::~shell_pipe()
{ close(); }

int rutz::shell_pipe::close()
{
  if ( !is_closed() )
    {
      m_stream.close();
      m_exit_status = pclose(m_file);
      m_file = 0;
    }
  return m_exit_status;
}

rutz::pipe_fds::pipe_fds()
{
  if (pipe(m_fds) != 0)
    throw rutz::error("couldn't create pipe", SRC_POS);
}

rutz::pipe_fds::~pipe_fds() throw()
{
  close_reader();
  close_writer();
}

rutz::child_process::child_process() :
  m_child_status(0),
  m_pid(fork())
{
  if (m_pid == -1)
    throw rutz::error("couldn't fork child process", SRC_POS);
}

rutz::child_process::~child_process() throw()
{
  wait();
}

int rutz::child_process::wait() throw()
{
  if (m_pid != 0)
    {
      waitpid(m_pid, &m_child_status, /*options*/ 0);
      m_pid = 0;
    }

  return m_child_status;
}

namespace
{
  bool is_read_mode(const char* m)
  {
    if (m == 0) throw rutz::error("invalid read/write mode", SRC_POS);
    switch (m[0])
      {
      case 'r': return true;
      case 'w': return false;
      }

    throw rutz::error(rutz::fstring("invalid read/write mode '", m, "'"),
                      SRC_POS);
    return false; // "can't happen"
  }
}

rutz::exec_pipe::exec_pipe(const char* m, char* const* argv) :
  m_parent_is_reader(is_read_mode(m)),
  m_fds(),
  m_child(),
  m_stream(0)
{
  if (m_child.in_parent())
    {
      if (m_parent_is_reader)
        {
          m_fds.close_writer();

          m_stream =
            new rutz::stdiostream(m_fds.reader(),
                                  std::ios::in|std::ios::binary);
        }
      else // parent is writer
        {
          m_fds.close_reader();

          m_stream =
            new rutz::stdiostream(m_fds.writer(),
                                  std::ios::out|std::ios::binary);
        }

      if (m_stream == 0)
        throw rutz::error("couldn't open stream in parent process", SRC_POS);
    }
  else // in child
    {
      if (m_parent_is_reader) // ==> child is writer
        {
          m_fds.close_reader();

          if (dup2(m_fds.writer(), STDOUT_FILENO) == -1)
            {
              fprintf(stderr, "dup2 failed in child process\n");
              exit(-1);
            }
        }
      else // parent is writer, child is reader
        {
          m_fds.close_writer();

          if (dup2(m_fds.reader(), STDIN_FILENO) == -1)
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


rutz::exec_pipe::~exec_pipe() throw()
{
  delete m_stream;
}

STD_IO::iostream& rutz::exec_pipe::stream() throw()
{
  ASSERT(m_stream != 0);
  return *m_stream;
}

void rutz::exec_pipe::close()
{
  if (m_stream != 0)
    {
      m_stream->close();
      m_fds.close_reader();
      m_fds.close_writer();
    }
}

int rutz::exec_pipe::exit_status() throw()
{
  const int child_status = m_child.wait();

  // Check if the child process exited abnormally
  if (WIFEXITED(child_status) == 0) return -1;

  // Check if the child process gave an error exit code
  if (WEXITSTATUS(child_status) != 0) return -1;

  // OK, everything looks fine
  return 0;
}

rutz::bidir_pipe::bidir_pipe(char* const* argv) :
  m_in_pipe(),
  m_out_pipe(),
  m_child(),
  m_in_stream(0),
  m_out_stream(0)
{
  if (m_child.in_parent())
    {
      m_in_pipe.close_writer();

      m_in_stream =
        new rutz::stdiostream(m_in_pipe.reader(),
                              std::ios::in|std::ios::binary);

      if (m_in_stream == 0)
        throw rutz::error("couldn't open input stream in parent process",
                          SRC_POS);

      m_out_pipe.close_reader();

      m_out_stream =
        new rutz::stdiostream(m_out_pipe.writer(),
                              std::ios::out|std::ios::binary);

      if (m_out_stream == 0)
        throw rutz::error("couldn't open input stream in parent process",
                          SRC_POS);
    }
  else // in child
    {
      m_in_pipe.close_reader();

      if (dup2(m_in_pipe.writer(), STDOUT_FILENO) == -1)
        {
          fprintf(stderr, "dup2 failed in child process\n");
          exit(-1);
        }

      m_out_pipe.close_writer();

      if (dup2(m_out_pipe.reader(), STDIN_FILENO) == -1)
        {
          fprintf(stderr, "dup2 failed in child process\n");
          exit(-1);
        }

      execv(argv[0], argv);

      fprintf(stderr, "execv failed in child process\n");
      exit(-1);
    }
}


rutz::bidir_pipe::~bidir_pipe() throw()
{
  delete m_out_stream;
  delete m_in_stream;
}

STD_IO::iostream& rutz::bidir_pipe::in_stream() throw()
{
  ASSERT(m_in_stream != 0);
  return *m_in_stream;
}

STD_IO::iostream& rutz::bidir_pipe::out_stream() throw()
{
  ASSERT(m_out_stream != 0);
  return *m_out_stream;
}

void rutz::bidir_pipe::close_in()
{
  if (m_in_stream != 0)
    {
      m_in_stream->close();
    }

  m_in_pipe.close_reader();
  m_in_pipe.close_writer();
}

void rutz::bidir_pipe::close_out()
{
  if (m_out_stream != 0)
    {
      m_out_stream->close();
    }

  m_out_pipe.close_reader();
  m_out_pipe.close_writer();
}

int rutz::bidir_pipe::exit_status() throw()
{
  const int child_status = m_child.wait();

  // Check if the child process exited abnormally
  if (WIFEXITED(child_status) == 0) return -1;

  // Check if the child process gave an error exit code
  if (WEXITSTATUS(child_status) != 0) return -1;

  // OK, everything looks fine
  return 0;
}

static const char vcid_pipe_cc[] = "$Id$ $URL$";
#endif // !PIPE_CC_DEFINED
