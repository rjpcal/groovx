/** @file rutz/debug.cc debugging facilities, assertions,
    preconditions, postconditions, invariants */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Oct  9 18:48:38 2000
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

#include "rutz/debug.h"
GVX_DBG_REGISTER

#include "rutz/backtrace.h"
#include "rutz/mutex.h"

#include <cstdio> // for stderr
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>

namespace
{
  const int MAX_KEYS = 1024;

  // output-related variables and guard mutex
  bool            g_debug_line_complete = true;
  std::mutex      g_debug_output_mutex;

  // debug keys and guard mutex
  int             g_debug_next_key = 0;
  int             g_key_levels[MAX_KEYS];
  const char*     g_key_filenames[MAX_KEYS];
  std::mutex      g_debug_keys_mutex;

  void show_position(int level, const char* where, int line_no) noexcept
  {
    std::cerr << "[" << std::setw(2) << level << "] "
              << std::setw(20) << where << ":"
              << line_no << ": ";
  }
}

#define EVAL_IMPL(T)                                    \
void rutz::debug::eval(const char* what, int level,     \
                       const char* where, int line_no,  \
                       bool nl, T expr) noexcept        \
{                                                       \
  GVX_MUTEX_LOCK(g_debug_output_mutex);                 \
  using std::cerr;                                      \
  cerr.exceptions(std::ios::goodbit);                   \
  if (g_debug_line_complete)                            \
    {                                                   \
      show_position(level, where, line_no);             \
    }                                                   \
  if (what)                                             \
    {                                                   \
      cerr << "(" << #T << ") " << what << " = ";       \
    }                                                   \
  cerr << expr;                                         \
  if (nl)                                               \
    {                                                   \
      cerr << "\n";                                     \
      g_debug_line_complete = true;                     \
    }                                                   \
  else                                                  \
    {                                                   \
      cerr << ", ";                                     \
      g_debug_line_complete = false;                    \
    }                                                   \
}

EVAL_IMPL(bool)
EVAL_IMPL(char)
EVAL_IMPL(unsigned char)
EVAL_IMPL(short)
EVAL_IMPL(unsigned short)
EVAL_IMPL(int)
EVAL_IMPL(unsigned int)
EVAL_IMPL(long)
EVAL_IMPL(unsigned long)
EVAL_IMPL(float)
EVAL_IMPL(double)
EVAL_IMPL(const char*)
EVAL_IMPL(const void*)

void rutz::debug::dump(const char* what, int level, const char* where, int line_no) noexcept
{
  GVX_MUTEX_LOCK(g_debug_output_mutex);
  std::cerr.exceptions(std::ios::goodbit);
  show_position(level, where, line_no);
  std::cerr << std::setw(15) << what << " := ";
  g_debug_line_complete = false;
}

void rutz::debug::start_newline() noexcept
{
  GVX_MUTEX_LOCK(g_debug_output_mutex);
  std::cerr.exceptions(std::ios::goodbit);
  if (!g_debug_line_complete)
    {
      std::cerr << '\n';
      g_debug_line_complete = true;
    }
}

void rutz::debug::panic_aux(const char* what, const char* where,
                            int line_no) noexcept
{
  fprintf(stderr, "Panic (%s:%d):\n\t%s\n\n",
          where, line_no, what);
  rutz::backtrace::current().print();
  abort();
}

void rutz::debug::assert_aux(const char* what, const char* where,
                             int line_no) noexcept
{
  fprintf(stderr, "Assertion failed (%s:%d):\n\texpected '%s'\n\n",
          where, line_no, what);
  rutz::backtrace::current().print();
  abort();
}

void rutz::debug::precondition_aux(const char* what, const char* where,
                                   int line_no) noexcept
{
  fprintf(stderr, "Precondition failed (%s:%d):\n\texpected '%s'\n\n",
          where, line_no, what);
  rutz::backtrace::current().print();
  abort();
}

void rutz::debug::postcondition_aux(const char* what, const char* where,
                                    int line_no) noexcept
{
  fprintf(stderr, "Postcondition failed (%s:%d):\n\texpected '%s'\n\n",
          where, line_no, what);
  rutz::backtrace::current().print();
  abort();
}

void rutz::debug::invariant_aux(const char* what, const char* where,
                                int line_no) noexcept
{
  fprintf(stderr, "Invariant failed (%s:%d):\n\texpected '%s'\n\n",
          where, line_no, what);
  rutz::backtrace::current().print();
  abort();
}

int rutz::debug::create_key(const char* filename)
{
  GVX_MUTEX_LOCK(g_debug_keys_mutex);
  const int key = g_debug_next_key;
  g_key_filenames[key] = filename;
  if (g_debug_next_key < (MAX_KEYS-1))
    ++g_debug_next_key;
  return key;
}

bool rutz::debug::is_valid_key(int key)
{
  return key >= 0 && key < MAX_KEYS;
}

int rutz::debug::lookup_key(const char* filename)
{
  GVX_MUTEX_LOCK(g_debug_keys_mutex);
  for (int i = 0; i < g_debug_next_key; ++i)
    {
      if (strcmp(g_key_filenames[i], filename) == 0)
        return i;
    }
  return -1;
}

int rutz::debug::get_level_for_key(int key)
{
  GVX_MUTEX_LOCK(g_debug_keys_mutex);
  if (key < MAX_KEYS)
    return g_key_levels[key];
  return 0;
}

void rutz::debug::set_level_for_key(int key, int level)
{
  GVX_MUTEX_LOCK(g_debug_keys_mutex);
  if (key <MAX_KEYS)
    g_key_levels[key] = level;
}

const char* rutz::debug::get_filename_for_key(int key)
{
  GVX_MUTEX_LOCK(g_debug_keys_mutex);
  return g_key_filenames[key];
}

void rutz::debug::set_global_level(int lev)
{
  GVX_MUTEX_LOCK(g_debug_keys_mutex);
  for (int i = 0; i < MAX_KEYS; ++i)
    g_key_levels[i] = lev;
}
