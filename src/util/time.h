///////////////////////////////////////////////////////////////////////
//
// time.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Nov  7 16:43:03 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TIME_H_DEFINED
#define TIME_H_DEFINED

#include <sys/time.h>
// struct timeval {
//                unsigned long  tv_sec;   /* seconds since Jan. 1, 1970 */
//                long           tv_usec;  /* and microseconds */
//            };

namespace Util
{
  class Time;
}

/// A simple wrapper around timeval.
/** All operations provide the no-throw guarantee. */
class Util::Time
{
public:
  /// Construct with time=0.
  Time() throw() : itsTimeVal()
  {
    reset();
  }

  /// Construct from a timeval.
  Time(const timeval& t) throw() : itsTimeVal(t) {}

  /// Construct with given seconds+microseconds values.
  Time(unsigned long s, long us) throw() : itsTimeVal()
  {
    reset(s, us);
  }

  /// Copy construct.
  Time(const Time& x) throw() : itsTimeVal(x.itsTimeVal) {}

  /// Destruct.
  ~Time() throw() {}

  /// Create a Time representing current wall-clock time.
  static Time wallClockNow() throw();

  /// Create a Time representing the current process's user cpu usage.
  static Time rusageUserNow() throw();

  /// Create a Time representing the current process's sys cpu usage.
  static Time rusageSysNow() throw();

  /// Reset to given seconds+microseconds values.
  void reset(unsigned long s = 0, long us = 0) throw()
  {
    itsTimeVal.tv_sec = s;
    itsTimeVal.tv_usec = us;
  }

  /// Assignment operator.
  Time& operator=(const Time& x) throw()
  {
    itsTimeVal = x.itsTimeVal;
    return *this;
  }

  /// Addition operator.
  Time operator+(const Time& t2) const throw()
  {
    timeval result;
    result.tv_sec  = this->itsTimeVal.tv_sec  + t2.itsTimeVal.tv_sec;
    result.tv_usec = this->itsTimeVal.tv_usec + t2.itsTimeVal.tv_usec;

    normalize(result);

    return Time(result);
  }

  /// In-place addition operator.
  Time& operator+=(const Time& t2) throw()
  {
    itsTimeVal.tv_sec  += t2.itsTimeVal.tv_sec;
    itsTimeVal.tv_usec += t2.itsTimeVal.tv_usec;

    normalize(this->itsTimeVal);

    return *this;
  }

  /// Subtraction operator.
  Time operator-(const Time& t2) const throw()
  {
    timeval result;
    result.tv_sec  = this->itsTimeVal.tv_sec  - t2.itsTimeVal.tv_sec;
    result.tv_usec = this->itsTimeVal.tv_usec - t2.itsTimeVal.tv_usec;

    normalize(result);

    return Time(result);
  }

  /// In-place subtraction operator.
  Time& operator-=(const Time& t2) throw()
  {
    itsTimeVal.tv_sec  -= t2.itsTimeVal.tv_sec;
    itsTimeVal.tv_usec -= t2.itsTimeVal.tv_usec;

    normalize(this->itsTimeVal);

    return *this;
  }

  /// Return time in floating-point seconds.
  double sec() const throw()
  {
    return double(itsTimeVal.tv_sec) + itsTimeVal.tv_usec / 1000000.0;
  }

  /// Return time in floating-point milliseconds.
  double msec() const throw()
  {
    return (itsTimeVal.tv_sec * 1000.0) + (itsTimeVal.tv_usec / 1000.0);
  }

  /// Return time in floating-point microseconds.
  double usec() const throw()
  {
    return (itsTimeVal.tv_sec * 1000000.0) + double(itsTimeVal.tv_usec);
  }

  /// Return internal timeval.
  const timeval& tval() const throw()
  {
    return itsTimeVal;
  }

private:
  /// Avoid overflow/underflow in the timeval's microseconds field.
  static void normalize(timeval& t) throw()
  {
    long s = t.tv_usec / 1000000;

    if (s != 0)
      {
        t.tv_sec += s;
        t.tv_usec -= (s * 1000000);
      }
  }

  timeval itsTimeVal;
};

static const char vcid_time_h[] = "$Header$";
#endif // !TIME_H_DEFINED
