///////////////////////////////////////////////////////////////////////
//
// debug.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jan  4 08:00:00 1999
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

#ifndef DEBUG_H_DEFINED
#define DEBUG_H_DEFINED

class fstring;

namespace Debug
{
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, bool expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, char expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, unsigned char expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, short expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, unsigned short expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, int expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, unsigned int expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, long expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, unsigned long expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, float expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, double expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, const char* expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, void* expr) throw();
  void Eval (const char* what, int level, const char* where, int line_no, bool nl, fstring expr) throw();

  void PanicImpl         (const char* what, const char* where, int line_no) throw();
  void AssertImpl        (const char* what, const char* where, int line_no) throw();
  void PreconditionImpl  (const char* what, const char* where, int line_no) throw();
  void PostconditionImpl (const char* what, const char* where, int line_no) throw();
  void InvariantImpl     (const char* what, const char* where, int line_no) throw();

  extern int level;
}

#define Panic(message) Debug::PanicImpl(message, __FILE__, __LINE__)

// Like Assert(), but can't ever be turned off.
#define AbortIf(expr) \
      do { if ( expr ) { Debug::PanicImpl(#expr, __FILE__, __LINE__); } } while (0)

static const char vcid_debug_h[] = "$Id$";

#else // DEBUG_H_DEFINED

//
// Everything below here might get processed multiple times if the file is
// #include'd more than once
//

#undef dbgEval
#undef dbgEvalNL
#undef dbgPrint
#undef dbgPrintNL

#undef Assert
#undef Invariant
#undef Precondition
#undef Postcondition

#endif // DEBUG_H_DEFINED

#if !defined(NO_DEBUG)
#  define dbgEval(lev, x)    do { if (Debug::level >= lev) Debug::Eval(#x, lev, __FILE__, __LINE__, false, x); } while (0)
#  define dbgEvalNL(lev, x)  do { if (Debug::level >= lev) Debug::Eval(#x, lev, __FILE__, __LINE__, true, x); } while (0)
#  define dbgPrint(lev, x)   do { if (Debug::level >= lev) Debug::Eval(0, lev, __FILE__, __LINE__, false, x); } while (0)
#  define dbgPrintNL(lev, x) do { if (Debug::level >= lev) Debug::Eval(0, lev, __FILE__, __LINE__, true, x); } while (0)
#  define dbgDump(lev, x)    do { if (Debug::level >= lev) { Debug::Eval(#x, lev, __FILE__, __LINE__, true, "..."); (x).debugDump(); } } while (0)

#  define Assert(expr)        do { if ( !(expr) ) Debug::AssertImpl(#expr, __FILE__, __LINE__); } while(0)
#  define Invariant(expr)     do { if ( !(expr) ) Debug::InvariantImpl(#expr, __FILE__, __LINE__); } while(0)
#  define Precondition(expr)  do { if ( !(expr) ) Debug::PreconditionImpl(#expr, __FILE__, __LINE__); } while(0)
#  define Postcondition(expr) do { if ( !(expr) ) Debug::PostconditionImpl(#expr, __FILE__, __LINE__); } while(0)

#else // defined(NO_DEBUG)

#  define dbgEval(lev, x)     do {} while (0)
#  define dbgEvalNL(lev, x)   do {} while (0)
#  define dbgPrint(lev, x)    do {} while (0)
#  define dbgPrintNL(lev, x)  do {} while (0)
#  define dbgDump(lev, expr)  do {} while (0)

#  define Assert(x)        do {} while (0)
#  define Invariant(x)     do {} while (0)
#  define Precondition(x)  do {} while (0)
#  define Postcondition(x) do {} while (0)

#endif
