///////////////////////////////////////////////////////////////////////
//
// debug.cc
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Oct  9 18:48:38 2000
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

#ifndef DEBUG_CC_DEFINED
#define DEBUG_CC_DEFINED

#include "util/debug.h"
DBG_REGISTER

#include "util/backtrace.h"
#include "util/strings.h"

#include <cstdlib>
#include <iostream>

#define LOCAL_PROF
#include "util/trace.h"

namespace
{
  bool lineComplete = true;
  int nextDebugKey = 0;
}

unsigned char Debug::keyLevels[Debug::MAX_KEYS];
const char*   Debug::keyFilenames[Debug::MAX_KEYS];

#define EVAL_IMPL(T)                                            \
void Debug::Eval(const char* what, int level,                   \
                 const char* where, int line_no,                \
                 bool nl, T expr) throw()                       \
{                                                               \
  STD_IO::cerr.exceptions(STD_IO::ios::goodbit);                \
  if (lineComplete)                                             \
    {                                                           \
      STD_IO::cerr << "[" << level << "] "                      \
                   << where << ":" << line_no << ": ";          \
    }                                                           \
  if (what)                                                     \
    {                                                           \
      STD_IO::cerr << "(" << #T << ") " << what << " = ";       \
    }                                                           \
  STD_IO::cerr << expr;                                         \
  if (nl)                                                       \
    {                                                           \
      STD_IO::cerr << "\n";                                     \
      lineComplete = true;                                      \
    }                                                           \
  else                                                          \
    {                                                           \
      STD_IO::cerr << ", ";                                     \
      lineComplete = false;                                     \
    }                                                           \
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
EVAL_IMPL(fstring);

void Debug::PanicImpl(const char* what, const char* where, int line_no) throw()
{
  fprintf(stderr, "Panic (%s:%d):\n\texpected '%s'\n\n", where, line_no, what);
  Util::BackTrace::current().print();
  abort();
}

void Debug::AssertImpl(const char* what, const char* where, int line_no) throw()
{
  fprintf(stderr, "Assertion failed (%s:%d):\n\texpected '%s'\n\n", where, line_no, what);
  Util::BackTrace::current().print();
  abort();
}

void Debug::PreconditionImpl(const char* what, const char* where, int line_no) throw()
{
  fprintf(stderr, "Precondition failed (%s:%d):\n\texpected '%s'\n\n", where, line_no, what);
  Util::BackTrace::current().print();
  abort();
}

void Debug::PostconditionImpl(const char* what, const char* where, int line_no) throw()
{
  fprintf(stderr, "Postcondition failed (%s:%d):\n\texpected '%s'\n\n", where, line_no, what);
  Util::BackTrace::current().print();
  abort();
}

void Debug::InvariantImpl(const char* what, const char* where, int line_no) throw()
{
  fprintf(stderr, "Invariant failed (%s:%d):\n\texpected '%s'\n\n", where, line_no, what);
  Util::BackTrace::current().print();
  abort();
}

int Debug::createKey(const char* filename)
{
  int key = nextDebugKey;
  keyFilenames[key] = filename;
  if (nextDebugKey < (MAX_KEYS-1))
    ++nextDebugKey;
  return key;
}

int Debug::lookupKey(const char* filename)
{
  for (int i = 0; i < nextDebugKey; ++i)
    {
      if (strcmp(keyFilenames[i], filename) == 0)
        return i;
    }
  return -1;
}

void Debug::setGlobalLevel(int lev)
{
  for (int i = 0; i < MAX_KEYS; ++i)
    keyLevels[i] = (unsigned char) lev;
}

static const char vcid_debug_cc[] = "$Header$";
#endif // !DEBUG_CC_DEFINED
