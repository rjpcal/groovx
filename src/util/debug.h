///////////////////////////////////////////////////////////////////////
// debug.h
// Rob Peters
// created: Jan-99
// written: Fri Mar 12 12:58:41 1999
static const char vcid_debug_h[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef DEBUG_H_DEFINED
#define DEBUG_H_DEFINED

#ifdef DEBUG
#define LOCAL_DEBUG
#endif

#ifdef LOCAL_DEBUG
#  include <iostream.h>
#  define DUMP_VAL1(expr) cerr << #expr << " == " << (expr) << ", ";
#  define DUMP_VAL2(expr) cerr << #expr << " == " << (expr) << endl;
#endif 

#endif // !DEBUG_H_DEFINED
