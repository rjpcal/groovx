///////////////////////////////////////////////////////////////////////
// debug.h
// Rob Peters
// created: Jan-99
// written: Mon Apr 26 12:18:59 1999
// $Id$
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

#ifdef INVARIANT
#  include <cassert>
#  define Invariant(invariant) assert((invariant))
#else // !INVARIANT
#  define Invariant(x) {}
#endif // !INVARIANT

static const char vcid_debug_h[] = "$Id$";
#endif // !DEBUG_H_DEFINED
