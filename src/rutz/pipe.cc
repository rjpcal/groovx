/** @file rutz/pipe.cc wrap posix inter-process pipes in a c++
    iostreams interface */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri May  2 16:38:30 2003
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "rutz/pipe.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include <cerrno>
#include <cstdarg>
#include <cstdlib> // for free()
#include <vector>

#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  char** make_argv(const char* argv0, va_list a)
  {
    std::vector<const char*> args;
    std::vector<size_t> offsets;
    size_t totalchars = 0;
    for (const char* arg = argv0;
         arg != nullptr;
         arg = va_arg(a, char*))
      {
        args.push_back(arg);
        offsets.push_back(totalchars);

        totalchars += strlen(arg) + 1;
      }

    void* mem = malloc(totalchars + (args.size() + 1) * sizeof(char*));

    if (mem == nullptr)
      throw rutz::error("memory allocation failed", SRC_POS);

    char** const ptrs = static_cast<char**>(mem);
    char* const chars = static_cast<char*>(mem) + (args.size() + 1) * sizeof(char*);

    char* q = chars;

    for (uint i = 0; i < args.size(); ++i)
      {
        ptrs[i] = &chars[0] + offsets[i];

        for (const char* p = args[i]; *p != '\0'; ++p)
          *q++ = *p;

        *q++ = '\0';
      }

    ptrs[args.size()] = 0;

    return ptrs;
  }
}

rutz::shell_pipe::shell_pipe(const char* command, const char* mode) :
  m_file(popen(command, mode)),
  m_stream(fileno(m_file), std::ios::in|std::ios::out),
  m_exit_status(0)
{}

rutz::shell_pipe::~shell_pipe()
{ close(); }

int rutz::shell_pipe::close()
{
  if ( !is_closed() )
    {
      m_stream.flush();
      m_exit_status = pclose(m_file);
      m_file = nullptr;
    }
  return m_exit_status;
}

rutz::pipe_fds::pipe_fds()
{
  if (pipe(m_fds) != 0)
    throw rutz::error("couldn't create pipe", SRC_POS);
}

rutz::pipe_fds::~pipe_fds() noexcept
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

rutz::child_process::~child_process() noexcept
{
  wait();
}

int rutz::child_process::wait() noexcept
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
    if (m == nullptr) throw rutz::error("invalid read/write mode", SRC_POS);
    switch (m[0])
      {
      case 'r': return true;
      case 'w': return false;
      }

    throw rutz::error(rutz::sfmt("invalid read/write mode '%s'", m),
                      SRC_POS);
  }
}

void rutz::exec_pipe::init(char* const* argv)
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

      if (m_stream == nullptr)
        throw rutz::error("couldn't open stream in parent process", SRC_POS);
    }
  else // in child
    {
      if (m_parent_is_reader) // ==> child is writer
        {
          m_fds.close_reader();

          if (dup2(m_fds.writer(), STDOUT_FILENO) == -1)
            {
              fprintf(stderr, "dup2 failed in child process (%s):\n",
                      argv[0]);
              fprintf(stderr, "%s\n", strerror(errno));
              exit(-1);
            }
        }
      else // parent is writer, child is reader
        {
          m_fds.close_writer();

          if (dup2(m_fds.reader(), STDIN_FILENO) == -1)
            {
              fprintf(stderr, "dup2 failed in child process (%s):\n",
                      argv[0]);
              fprintf(stderr, "%s\n", strerror(errno));
              exit(-1);
            }
        }

      execv(argv[0], argv);

      fprintf(stderr, "execv failed in child process (%s)\n", argv[0]);
      fprintf(stderr, "%s\n", strerror(errno));
      exit(-1);
    }
}

rutz::exec_pipe::exec_pipe(const char* m, char* const* argv) :
  m_parent_is_reader(is_read_mode(m)),
  m_fds(),
  m_child(),
  m_stream(nullptr)
{
  this->init(argv);
}

rutz::exec_pipe::exec_pipe(const char* m, const char* argv0, ...) :
  m_parent_is_reader(is_read_mode(m)),
  m_fds(),
  m_child(),
  m_stream(nullptr)
{
  va_list a;
  va_start(a, argv0);
  char** argv = make_argv(argv0, a);
  va_end(a);

  try { this->init(argv); }
  catch (...) { free(argv); throw; }

  free(argv);
}

rutz::exec_pipe::~exec_pipe() noexcept
{
  delete m_stream;
}

std::iostream& rutz::exec_pipe::stream() noexcept
{
  GVX_ASSERT(m_stream != nullptr);
  return *m_stream;
}

void rutz::exec_pipe::close()
{
  if (m_stream != nullptr)
    {
      m_stream->flush();
      m_fds.close_reader();
      m_fds.close_writer();
    }
}

int rutz::exec_pipe::exit_status() noexcept
{
  const int child_status = m_child.wait();

  // Check if the child process exited abnormally
  if (WIFEXITED(child_status) == 0) return -1;

  // Check if the child process gave an error exit code
  if (WEXITSTATUS(child_status) != 0) return -1;

  // OK, everything looks fine
  return 0;
}

rutz::bidir_pipe::bidir_pipe() :
  m_in_pipe(),
  m_out_pipe(),
  m_child(),
  m_in_stream(nullptr),
  m_out_stream(nullptr),
  m_block_child_sigint(true)
{
  // user must call init() before using the bidir_pipe's streams
}

rutz::bidir_pipe::bidir_pipe(char* const* argv) :
  m_in_pipe(),
  m_out_pipe(),
  m_child(),
  m_in_stream(nullptr),
  m_out_stream(nullptr),
  m_block_child_sigint(true)
{
  this->init(argv);
}

rutz::bidir_pipe::bidir_pipe(const char* argv0, ...) :
  m_in_pipe(),
  m_out_pipe(),
  m_child(),
  m_in_stream(nullptr),
  m_out_stream(nullptr),
  m_block_child_sigint(true)
{
  va_list a;
  va_start(a, argv0);
  char** argv = make_argv(argv0, a);
  va_end(a);

  try { this->init(argv); }
  catch (...) { free(argv); throw; }

  free(argv);
}

rutz::bidir_pipe::~bidir_pipe() noexcept
{
  close_in();
  close_out();

  delete m_out_stream;
  delete m_in_stream;
}

void rutz::bidir_pipe::block_child_sigint()
{
  m_block_child_sigint = true;
}

void rutz::bidir_pipe::init(char* const* argv)
{
  if (m_child.in_parent())
    {
      m_in_pipe.close_writer();

      m_in_stream =
        new rutz::stdiostream(m_in_pipe.reader(),
                              std::ios::in|std::ios::binary);

      if (m_in_stream == nullptr)
        throw rutz::error("couldn't open input stream in parent process",
                          SRC_POS);

      m_out_pipe.close_reader();

      m_out_stream =
        new rutz::stdiostream(m_out_pipe.writer(),
                              std::ios::out|std::ios::binary);

      if (m_out_stream == nullptr)
        throw rutz::error("couldn't open input stream in parent process",
                          SRC_POS);
    }
  else // in child
    {
      m_in_pipe.close_reader();

      if (dup2(m_in_pipe.writer(), STDOUT_FILENO) == -1)
        {
          fprintf(stderr, "dup2 failed in child process (%s):\n",
                  argv[0]);
          fprintf(stderr, "%s\n", strerror(errno));
          exit(-1);
        }

      m_out_pipe.close_writer();

      if (dup2(m_out_pipe.reader(), STDIN_FILENO) == -1)
        {
          fprintf(stderr, "dup2 failed in child process (%s):\n",
                  argv[0]);
          fprintf(stderr, "%s\n", strerror(errno));
          exit(-1);
        }

      if (m_block_child_sigint)
        signal(SIGINT, SIG_IGN);

      errno = 0;

      execv(argv[0], argv);

      fprintf(stderr, "execv failed in child process (%s):\n", argv[0]);
      fprintf(stderr, "%s\n", strerror(errno));
      exit(-1);
    }
}

void rutz::bidir_pipe::init(const char* argv0, ...)
{
  va_list a;
  va_start(a, argv0);
  char** argv = make_argv(argv0, a);
  va_end(a);

  try { this->init(argv); }
  catch (...) { free(argv); throw; }

  free(argv);
}

std::iostream& rutz::bidir_pipe::in_stream() noexcept
{
  GVX_ASSERT(m_in_stream != nullptr);
  return *m_in_stream;
}

std::iostream& rutz::bidir_pipe::out_stream() noexcept
{
  GVX_ASSERT(m_out_stream != nullptr);
  return *m_out_stream;
}

void rutz::bidir_pipe::close_in()
{
  if (m_in_stream != nullptr)
    {
      m_in_stream->flush();
    }

  m_in_pipe.close_reader();
  m_in_pipe.close_writer();
}

void rutz::bidir_pipe::close_out()
{
  if (m_out_stream != nullptr)
    {
      m_out_stream->flush();
    }

  m_out_pipe.close_reader();
  m_out_pipe.close_writer();
}

int rutz::bidir_pipe::exit_status() noexcept
{
  const int child_status = m_child.wait();

  // Check if the child process exited abnormally
  if (WIFEXITED(child_status) == 0) return -1;

  // Check if the child process gave an error exit code
  if (WEXITSTATUS(child_status) != 0) return -1;

  // OK, everything looks fine
  return 0;
}
