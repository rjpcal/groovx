///////////////////////////////////////////////////////////////////////
//
// debug.h
// Rob Peters
// created: Jan-99
// written: Mon Oct  9 18:43:26 2000
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

#if defined(TEST)
#  ifndef LOCAL_TEST
#    define LOCAL_TEST
#  endif
#endif

#if defined(ASSERT) || defined(LOCAL_DEBUG)
#  ifndef LOCAL_ASSERT
#    define LOCAL_ASSERT
#  endif
#endif

#if defined(INVARIANT) || defined(LOCAL_DEBUG)
#  ifndef LOCAL_INVARIANT
#    define LOCAL_INVARIANT
#  endif
#endif

#if defined(CONTRACT) || defined(LOCAL_DEBUG)
#  ifndef LOCAL_CONTRACT
#    define LOCAL_CONTRACT
#  endif
#endif

#ifdef LOCAL_DEBUG
#  include <iostream.h>
#  define DUMP_VAL1(expr) cerr << #expr << " == " << (expr) << ", ";
#  define DUMP_VAL2(expr) cerr << #expr << " == " << (expr) << endl;
#  define DebugEval(expr) cerr << #expr << " == " << (expr) << ", ";
#  define DebugEvalNL(expr) cerr << #expr << " == " << (expr) << endl;
#  define DebugPrint(expr) cerr << expr << " ";
#  define DebugPrintNL(expr) cerr << expr << " " << endl;
#else
#  define DUMP_VAL1(expr) {}
#  define DUMP_VAL2(expr) {}
#  define DebugEval(expr) {}
#  define DebugEvalNL(expr) {}
#  define DebugPrint(str) {}
#  define DebugPrintNL(str) {}
#endif

namespace Debug {
  void AssertImpl        (const char* what, const char* where, int line_no);
  void PreconditionImpl  (const char* what, const char* where, int line_no);
  void PostconditionImpl (const char* what, const char* where, int line_no);
  void InvariantImpl     (const char* what, const char* where, int line_no);
}

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
