///////////////////////////////////////////////////////////////////////
//
// debug.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Thu Sep 13 11:20:52 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEBUG_H_DEFINED
#define DEBUG_H_DEFINED

static const char vcid_debug_h[] = "$Id$";

#else // DEBUG_H_DEFINED

#undef DebugEval
#undef DebugEvalNL
#undef DebugPrint
#undef DebugPrintNL

#undef Assert
#undef Invariant
#undef Precondition
#undef Postcondition

#endif

#ifdef DEBUG
#  ifndef LOCAL_DEBUG
#    define LOCAL_DEBUG
#  endif
#endif

#ifdef LOCAL_DEBUG
#  include <iostream.h>
#  define DebugEval(expr) STD_IO::cerr << #expr << " == " << (expr) << ", ";
#  define DebugEvalNL(expr) STD_IO::cerr << #expr << " == " << (expr) << STD_IO::endl;
#  define DebugPrint(expr) STD_IO::cerr << expr << " ";
#  define DebugPrintNL(expr) STD_IO::cerr << expr << " " << STD_IO::endl;
#else
#  define DebugEval(expr) {}
#  define DebugEvalNL(expr) {}
#  define DebugPrint(str) {}
#  define DebugPrintNL(str) {}
#endif

namespace Debug
{
  void AssertImpl        (const char* what, const char* where, int line_no);
  void PreconditionImpl  (const char* what, const char* where, int line_no);
  void PostconditionImpl (const char* what, const char* where, int line_no);
  void InvariantImpl     (const char* what, const char* where, int line_no);
}

#if !defined(NO_ASSERT) || defined(LOCAL_DEBUG)

#  define Assert(expr) \
        if ( !(expr) ) { Debug::AssertImpl(#expr, __FILE__, __LINE__); }
#  define Invariant(expr) \
        if ( !(expr) ) { Debug::InvariantImpl(#expr, __FILE__, __LINE__); }
#  define Precondition(expr) \
        if ( !(expr) ) { Debug::PreconditionImpl(#expr, __FILE__, __LINE__); }
#  define Postcondition(expr) \
        if ( !(expr) ) { Debug::PostconditionImpl(#expr, __FILE__, __LINE__); }

#else // NO_ASSERT

#  define Assert(x) {}
#  define Invariant(x) {}
#  define Precondition(x) {}
#  define Postcondition(x) {}

#endif
