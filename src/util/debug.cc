///////////////////////////////////////////////////////////////////////
//
// debug.cc
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Oct  9 18:48:38 2000
// commit: $Id$
// $HeadURL$
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

#ifndef DEBUG_CC_DEFINED
#define DEBUG_CC_DEFINED

#include "util/debug.h"
DBG_REGISTER

#include "util/backtrace.h"
#include "util/fstring.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>

#define LOCAL_PROF
#include "util/trace.h"

namespace
{
  bool debug_line_complete = true;
  int debug_next_key = 0;

  void show_position(int level, const char* where, int line_no) throw()
  {
    std::cerr << "[" << std::setw(2) << level << "] "
              << std::setw(20) << where << ":"
              << line_no << ": ";
  }
}

unsigned char rutz::debug::key_levels[rutz::debug::MAX_KEYS];
const char*   rutz::debug::key_filenames[rutz::debug::MAX_KEYS];

#define EVAL_IMPL(T)                                    \
void rutz::debug::eval(const char* what, int level,     \
                       const char* where, int line_no,  \
                       bool nl, T expr) throw()         \
{                                                       \
  using std::cerr;                                      \
  cerr.exceptions(std::ios::goodbit);                   \
  if (debug_line_complete)                              \
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
      debug_line_complete = true;                       \
    }                                                   \
  else                                                  \
    {                                                   \
      cerr << ", ";                                     \
      debug_line_complete = false;                      \
    }                                                   \
}

EVAL_IMPL(bool);
EVAL_IMPL(char);
EVAL_IMPL(unsigned char);
EVAL_IMPL(short);
EVAL_IMPL(unsigned short);
EVAL_IMPL(int);
EVAL_IMPL(unsigned int);
EVAL_IMPL(long);
EVAL_IMPL(unsigned long);
EVAL_IMPL(float);
EVAL_IMPL(double);
EVAL_IMPL(const char*);
EVAL_IMPL(void*);
EVAL_IMPL(rutz::fstring);

void rutz::debug::dump(const char* what, int level, const char* where, int line_no) throw()
{
  std::cerr.exceptions(std::ios::goodbit);
  show_position(level, where, line_no);
  std::cerr << std::setw(15) << what << " := ";
  debug_line_complete = false;
}

void rutz::debug::start_newline() throw()
{
  std::cerr.exceptions(std::ios::goodbit);
  if (!debug_line_complete)
    {
      std::cerr << '\n';
      debug_line_complete = true;
    }
}

void rutz::debug::panic_aux(const char* what, const char* where, int line_no) throw()
{
  fprintf(stderr, "Panic (%s:%d):\n\tgot '%s'\n\n", where, line_no, what);
  rutz::backtrace::current().print();
  abort();
}

void rutz::debug::assert_aux(const char* what, const char* where, int line_no) throw()
{
  fprintf(stderr, "Assertion failed (%s:%d):\n\texpected '%s'\n\n", where, line_no, what);
  rutz::backtrace::current().print();
  abort();
}

void rutz::debug::precondition_aux(const char* what, const char* where, int line_no) throw()
{
  fprintf(stderr, "Precondition failed (%s:%d):\n\texpected '%s'\n\n", where, line_no, what);
  rutz::backtrace::current().print();
  abort();
}

void rutz::debug::postcondition_aux(const char* what, const char* where, int line_no) throw()
{
  fprintf(stderr, "Postcondition failed (%s:%d):\n\texpected '%s'\n\n", where, line_no, what);
  rutz::backtrace::current().print();
  abort();
}

void rutz::debug::invariant_aux(const char* what, const char* where, int line_no) throw()
{
  fprintf(stderr, "Invariant failed (%s:%d):\n\texpected '%s'\n\n", where, line_no, what);
  rutz::backtrace::current().print();
  abort();
}

int rutz::debug::create_key(const char* filename)
{
  int key = debug_next_key;
  key_filenames[key] = filename;
  if (debug_next_key < (MAX_KEYS-1))
    ++debug_next_key;
  return key;
}

int rutz::debug::lookup_key(const char* filename)
{
  for (int i = 0; i < debug_next_key; ++i)
    {
      if (strcmp(key_filenames[i], filename) == 0)
        return i;
    }
  return -1;
}

void rutz::debug::set_global_level(int lev)
{
  for (int i = 0; i < MAX_KEYS; ++i)
    key_levels[i] = (unsigned char) lev;
}

static const char vcid_debug_cc[] = "$Id$ $URL$";
#endif // !DEBUG_CC_DEFINED
