///////////////////////////////////////////////////////////////////////
// debug.h
// Rob Peters
// created: Jan-99
// written: Sat Mar 13 16:16:09 1999
///////////////////////////////////////////////////////////////////////

#ifndef DEBUG_H_DEFINED
#define DEBUG_H_DEFINED

#ifdef DEBUG
#define LOCAL_DEBUG
#define LOCAL_ASSERT
#endif

#ifdef LOCAL_DEBUG
#  include <iostream.h>
#  define DUMP_VAL1(expr) cerr << #expr << " == " << (expr) << ", ";
#  define DUMP_VAL2(expr) cerr << #expr << " == " << (expr) << endl;
#endif

#ifdef LOCAL_ASSERT
#  include <cassert>
#  define Assert(expr_that_must_be_true) assert((expr_that_must_be_true))
#else // !LOCAL_ASSERT
#  define Assert(x) {}
#endif // !LOCAL_ASSERT

static const char vcid_debug_h[] = "$Id$";
#endif // !DEBUG_H_DEFINED
