///////////////////////////////////////////////////////////////////////
//
// debug.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Oct  9 18:48:38 2000
// written: Wed Mar 19 17:58:04 2003
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

#ifndef DEBUG_CC_DEFINED
#define DEBUG_CC_DEFINED

#include "util/debug.h"

#include "util/strings.h"

#include <cstdlib>
#include <iostream>

#define LOCAL_PROF
#include "util/trace.h"

namespace
{
  bool lineComplete = true;
}

int Debug::level = 0;

#define EVAL_IMPL(T)                                    \
void Debug::Eval(const char* what, int level,           \
                 const char* where, int line_no,        \
                 T expr, bool nl)                       \
{                                                       \
  if (lineComplete)                                     \
    {                                                   \
      STD_IO::cerr << "[" << level << "]";              \
      STD_IO::cerr << where << ":" << line_no << ": ";  \
    }                                                   \
  if (what)                                             \
    STD_IO::cerr << "(" << #T << ") " << what << " = "; \
  STD_IO::cerr << expr;                                 \
  if (nl)                                               \
    {                                                   \
      STD_IO::cerr << STD_IO::endl;                     \
      lineComplete = true;                              \
    }                                                   \
  else                                                  \
    {                                                   \
      STD_IO::cerr << ", ";                             \
      lineComplete = false;                             \
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
EVAL_IMPL(fstring);

void Debug::AssertImpl(const char* what, const char* where, int line_no)
{
  STD_IO::cerr << "Assertion failed: '" << what << "' in "
               << where << " line " << line_no << '\n';
  Util::BackTrace::current().print(STD_IO::cerr);
  abort();
}

void Debug::PreconditionImpl(const char* what, const char* where, int line_no)
{
  STD_IO::cerr << "Precondition failed: '" << what << "' in "
               << where << " line " << line_no << '\n';
  Util::BackTrace::current().print(STD_IO::cerr);
  abort();
}

void Debug::PostconditionImpl(const char* what, const char* where, int line_no)
{
  STD_IO::cerr << "Postcondition failed: '" << what << "' in "
               << where << " line " << line_no << '\n';
  Util::BackTrace::current().print(STD_IO::cerr);
  abort();
}

void Debug::InvariantImpl(const char* what, const char* where, int line_no)
{
  STD_IO::cerr << "Invariant failed: '" << what << "' in "
               << where << " line " << line_no << '\n';
  Util::BackTrace::current().print(STD_IO::cerr);
  abort();
}

static const char vcid_debug_cc[] = "$Header$";
#endif // !DEBUG_CC_DEFINED
