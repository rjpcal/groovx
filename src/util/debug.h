///////////////////////////////////////////////////////////////////////
//
// debug.h
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
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

  const int MAX_KEYS = 1024;

  extern unsigned char keyLevels[MAX_KEYS];
  extern const char* keyFilenames[MAX_KEYS];

  int createKey(const char* filename);

  /// Get the debug key associated with the given filename.
  /** Returns -1 if the filename is not registered. */
  int lookupKey(const char* filename);

  void setGlobalLevel(int lev);
}

#define Panic(message) Debug::PanicImpl(message, __FILE__, __LINE__)

// Like Assert(), but can't ever be turned off.
#define AbortIf(expr) \
      do { if ( expr ) { Debug::PanicImpl(#expr, __FILE__, __LINE__); } } while (0)

#define CONCAT(x, y) x ## y

#define DO_DBG_REGISTER(ext)                                            \
static const int CONCAT(DEBUG_KEY, ext) = Debug::createKey(__FILE__);   \
                                                                        \
static inline int CONCAT(dbgLevel, ext) ()                              \
{                                                                       \
  return Debug::keyLevels[CONCAT(DEBUG_KEY, ext)];                      \
}

#define DBG_REGISTER DO_DBG_REGISTER(1)
#define GET_DBG_LEVEL dbgLevel1

static const char vcid_debug_h[] = "$Header$";

#else // DEBUG_H_DEFINED

//
// Everything here gets processed on the second and subsequent times that
// this file is #include'ed.
//

#undef DBG_REGISTER
#undef GET_DBG_LEVEL

#if   !defined(DEBUG_H_2)
#      define  DEBUG_H_2
#      define  DBG_REGISTER DO_DBG_REGISTER(2)
#      define  GET_DBG_LEVEL dbgLevel2
#elif !defined(DEBUG_H_3)
#      define  DEBUG_H_3
#      define  DBG_REGISTER DO_DBG_REGISTER(3)
#      define  GET_DBG_LEVEL dbgLevel3
#elif !defined(DEBUG_H_4)
#      define  DEBUG_H_4
#      define  DBG_REGISTER DO_DBG_REGISTER(4)
#      define  GET_DBG_LEVEL dbgLevel4
#elif !defined(DEBUG_H_5)
#      define  DEBUG_H_5
#      define  DBG_REGISTER DO_DBG_REGISTER(5)
#      define  GET_DBG_LEVEL dbgLevel5
#elif !defined(DEBUG_H_6)
#      define  DEBUG_H_6
#      define  DBG_REGISTER DO_DBG_REGISTER(6)
#      define  GET_DBG_LEVEL dbgLevel6
#elif !defined(DEBUG_H_7)
#      define  DEBUG_H_7
#      define  DBG_REGISTER DO_DBG_REGISTER(7)
#      define  GET_DBG_LEVEL dbgLevel7
#elif !defined(DEBUG_H_8)
#      define  DEBUG_H_8
#      define  DBG_REGISTER DO_DBG_REGISTER(8)
#      define  GET_DBG_LEVEL dbgLevel8
#else
#      error "debug.h included too many times!"
#endif

#undef dbgEval
#undef dbgEvalNL
#undef dbgPrint
#undef dbgPrintNL

#undef Assert
#undef Invariant
#undef Precondition
#undef Postcondition

#endif // DEBUG_H_DEFINED

//
// Everything here gets re-processed every time that this file is #include'ed.
//

#if !defined(NO_DEBUG)
#  define dbgEval(lev, x)    do { if (GET_DBG_LEVEL() >= lev) Debug::Eval(#x, lev, __FILE__, __LINE__, false, x); } while (0)
#  define dbgEvalNL(lev, x)  do { if (GET_DBG_LEVEL() >= lev) Debug::Eval(#x, lev, __FILE__, __LINE__, true, x); } while (0)
#  define dbgPrint(lev, x)   do { if (GET_DBG_LEVEL() >= lev) Debug::Eval(0, lev, __FILE__, __LINE__, false, x); } while (0)
#  define dbgPrintNL(lev, x) do { if (GET_DBG_LEVEL() >= lev) Debug::Eval(0, lev, __FILE__, __LINE__, true, x); } while (0)
#  define dbgDump(lev, x)    do { if (GET_DBG_LEVEL() >= lev) { Debug::Eval(#x, lev, __FILE__, __LINE__, true, "..."); (x).debugDump(); } } while (0)

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
