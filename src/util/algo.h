///////////////////////////////////////////////////////////////////////
//
// algo.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Jul 22 23:31:48 2001
// written: Mon Jan 13 11:08:26 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ALGO_H_DEFINED
#define ALGO_H_DEFINED

namespace Util
{
  template <class T>
  inline const T& max(const T& a, const T& b) { return (a > b) ? a : b; }

  template <class T>
  inline const T& min(const T& a, const T& b) { return (a < b) ? a : b; }

  template <class T>
  inline T abs(const T& val)                  { return (val < 0) ? -val : val; }

  template <class T>
  inline T clamp(const T& val, const T& lower, const T& upper)
    { return Util::max(lower, Util::min(upper, val)); }

  template <class T>
  inline void swap2(T& t1, T& t2)
  {
    T t2_copy = t2;
    t2 = t1;
    t1 = t2_copy;
  }
}

static const char vcid_algo_h[] = "$Header$";
#endif // !ALGO_H_DEFINED
