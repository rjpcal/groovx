///////////////////////////////////////////////////////////////////////
// trace.h
// Rob Peters
// created: Jan-99
// written: Tue Mar 16 19:21:30 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TRACE_H_DEFINED
#define TRACE_H_DEFINEd

#if (defined(TRACE) && !defined(NO_TRACE))
#define LOCAL_TRACE
#endif

#ifdef LOCAL_TRACE

#ifndef IOSTREAM_H_INCLUDED
#include <iostream.h>
#define IOSTREAM_H_INCLUDED
#endif
#ifndef STRING_INCLUDED
#include <string>
#define STRING_INCLUDED
#endif

const int MAX_TRACE_LEVEL = 4;

extern int TRACE_LEVEL;
const char *const TRACE_TAB = "  ";

class Trace {
public:
  Trace(string s) : str(s) {
    if (TRACE_LEVEL < MAX_TRACE_LEVEL) {
      for (int i=0; i < TRACE_LEVEL; i++)
        cerr << TRACE_TAB;
      cerr << "entering " << str << "...\n";
    }
    TRACE_LEVEL++;
  }
  
  ~Trace() {
    TRACE_LEVEL--;
    if (TRACE_LEVEL < MAX_TRACE_LEVEL) {
      for (int i=0; i < TRACE_LEVEL; i++)
        cerr << TRACE_TAB;
      cerr << "leaving " << str << ".\n";
    }
    if (TRACE_LEVEL == 0) cerr << endl;
  }
private:
  string str;
};

#define DOTRACE(x) Trace __T(x)
#else // !LOCAL_TRACE
#define DOTRACE(x) {}
#endif // !LOCAL_TRACE

static const char vcid_trace_h[] = "$Header$";
#endif // !TRACE_H_DEFINED
