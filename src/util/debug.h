///////////////////////////////////////////////////////////////////////
//
// debug.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Wed Aug 22 15:29:52 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEBUG_H_DEFINED
#define DEBUG_H_DEFINED

#ifdef DEBUG
#  ifndef LOCAL_DEBUG
#    define LOCAL_DEBUG
#  endif
#endif

#if defined(ASSERT) || defined(LOCAL_DEBUG)
#  ifndef LOCAL_ASSERT
#    define LOCAL_ASSERT
#  endif
#endif

namespace Debug {
  void AssertImpl        (const char* what, const char* where, int line_no);
  void PreconditionImpl  (const char* what, const char* where, int line_no);
  void PostconditionImpl (const char* what, const char* where, int line_no);
  void InvariantImpl     (const char* what, const char* where, int line_no);
}

#ifdef LOCAL_DEBUG
#  include <iostream.h>
#  define DebugEval(expr) cerr << #expr << " == " << (expr) << ", ";
#  define DebugEvalNL(expr) cerr << #expr << " == " << (expr) << endl;
#  define DebugPrint(expr) cerr << expr << " ";
#  define DebugPrintNL(expr) cerr << expr << " " << endl;
#else
#  define DebugEval(expr) {}
#  define DebugEvalNL(expr) {}
#  define DebugPrint(str) {}
#  define DebugPrintNL(str) {}
#endif

#ifdef LOCAL_ASSERT
#  define Assert(expr) \
        if ( !(expr) ) { Debug::AssertImpl(#expr, __FILE__, __LINE__); }
#  define Invariant(expr) \
        if ( !(expr) ) { Debug::InvariantImpl(#expr, __FILE__, __LINE__); }
#  define Precondition(expr) \
        if ( !(expr) ) { Debug::PreconditionImpl(#expr, __FILE__, __LINE__); }
#  define Postcondition(expr) \
        if ( !(expr) ) { Debug::PostconditionImpl(#expr, __FILE__, __LINE__); }
#else // !LOCAL_ASSERT
#  define Assert(x) {}
#  define Invariant(x) {}
#  define Precondition(x) {}
#  define Postcondition(x) {}
#endif // !LOCAL_ASSERT

static const char vcid_debug_h[] = "$Id$";
#endif // !DEBUG_H_DEFINED
