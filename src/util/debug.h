///////////////////////////////////////////////////////////////////////
//
// debug.h
// Rob Peters
// created: Jan-99
// written: Wed Jun 16 15:08:05 1999
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

#if defined(LOCAL_ASSERT) || defined(LOCAL_INVARIANT) || defined(LOCAL_CONTRACT)
#  include <cassert>
#endif

#ifdef LOCAL_ASSERT
#  define Assert(expression) assert((expression))
#else // !LOCAL_ASSERT
#  define Assert(x) {}
#endif // !LOCAL_ASSERT

#ifdef LOCAL_INVARIANT
#  define Invariant(expression) assert((expression))
#else // !INVARIANT
#  define Invariant(x) {}
#endif // !INVARIANT

#ifdef LOCAL_CONTRACT
#  define Precondition(expression) assert((expression))
#  define Postcondition(expression) assert((expression))
#else
#  define Precondition(x) {}
#  define Postcondition(x) {}
#endif

static const char vcid_debug_h[] = "$Id$";
#endif // !DEBUG_H_DEFINED
