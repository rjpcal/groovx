///////////////////////////////////////////////////////////////////////
//
// hooktcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct  5 13:51:43 2000
// written: Tue May 14 19:44:48 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOOKTCL_CC_DEFINED
#define HOOKTCL_CC_DEFINED

#include "tcl/tclpkg.h"

#include "util/trace.h"

namespace
{
  size_t TOTAL = 0;
}

#if 0
#include <cstdlib>
#include <cstdio>

void* operator new(size_t bytes)
{
  TOTAL += bytes;
  printf("%d bytes  ", (int) bytes);
  Util::Trace::printStackTrace();
  return malloc(bytes);
}

void operator delete(void* space)
{
  free(space);
}
#endif

#include "util/strings.h"

template <class T>
const char* sformat_partial(char x, fstring& str, const char* format,
                            const T& arg)
{
  const char* p = format;
  for (; p != '\0'; ++p)
    {
      if (*p == x)
        {
          str.append(Util::CharData(format, p - format));
          str.append(arg);
          return p+1;
        }
    }
  return p; // here, *p == '\0'
}

template <char x, class T1, class T2>
const char* sformat_partial(fstring& str, const char* format,
                            const T1& arg1, const T2& arg2)
{
  const char* remainder = sformat_partial(x, str, format, arg1);
  return sformat_partial(x, str, remainder, arg2);
}

template <char x, class T1, class T2, class T3>
const char* sformat_partial(fstring& str, const char* format,
                            const T1& arg1, const T2& arg2, const T3& arg3)
{
  const char* remainder = sformat_partial<x>(str, format, arg1, arg2);
  return sformat_partial(x, str, remainder, arg3);
}

template <char x, class T>
fstring sformat(const char* format, const T& arg)
{
  fstring result;
  result.append(sformat_partial(x, result, format, arg));
  return result;
}

template <char x, class T1, class T2>
fstring sformat(const char* format, const T1& arg1, const T2& arg2)
{
  fstring result;
  result.append(sformat_partial<x>(result, format, arg1, arg2));
  return result;
}

namespace HookTcl
{
  fstring hook()
  {
    return sformat<'*'>("The * is *, so there.", "value", 42);
  }

  size_t memUsage() { return TOTAL; }
}

extern "C"
int Hook_Init(Tcl_Interp* interp)
{
  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Hook", "$Revision$");

  pkg->def( "::hook", "", HookTcl::hook );
  pkg->def( "::memUsage", 0, HookTcl::memUsage );

  return pkg->initStatus();
}

static const char vcid_hooktcl_cc[] = "$Header$";
#endif // !HOOKTCL_CC_DEFINED
