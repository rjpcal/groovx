///////////////////////////////////////////////////////////////////////
//
// time.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Nov  7 16:43:03 2002
// written: Wed Mar 19 12:45:35 2003
// $Id$
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

class Util::Time
{
public:
  Time() throw() : itsTimeVal()
  {
    reset();
  }

  Time(const timeval& t) throw() : itsTimeVal(t) {}

  Time(unsigned long s, long us) throw() : itsTimeVal()
  {
    reset(s, us);
  }

  Time(const Time& x) throw() : itsTimeVal(x.itsTimeVal) {}

  ~Time() throw() {}

  static Time wallClockNow() throw();

  static Time rusageNow() throw();

  void reset(unsigned long s = 0, long us = 0) throw()
  {
    itsTimeVal.tv_sec = s;
    itsTimeVal.tv_usec = us;
  }

  Time& operator=(const Time& x) throw()
  {
    itsTimeVal = x.itsTimeVal;
    return *this;
  }

  Time operator+(const Time& t2) const throw()
  {
    timeval result;
    result.tv_sec  = this->itsTimeVal.tv_sec  + t2.itsTimeVal.tv_sec;
    result.tv_usec = this->itsTimeVal.tv_usec + t2.itsTimeVal.tv_usec;

    normalize(result);

    return Time(result);
  }

  Time& operator+=(const Time& t2) throw()
  {
    itsTimeVal.tv_sec  += t2.itsTimeVal.tv_sec;
    itsTimeVal.tv_usec += t2.itsTimeVal.tv_usec;

    normalize(this->itsTimeVal);

    return *this;
  }

  Time operator-(const Time& t2) const throw()
  {
    timeval result;
    result.tv_sec  = this->itsTimeVal.tv_sec  - t2.itsTimeVal.tv_sec;
    result.tv_usec = this->itsTimeVal.tv_usec - t2.itsTimeVal.tv_usec;

    normalize(result);

    return Time(result);
  }

  Time& operator-=(const Time& t2) throw()
  {
    itsTimeVal.tv_sec  -= t2.itsTimeVal.tv_sec;
    itsTimeVal.tv_usec -= t2.itsTimeVal.tv_usec;

    normalize(this->itsTimeVal);

    return *this;
  }

  double sec() const throw()
  {
    return double(itsTimeVal.tv_sec) + itsTimeVal.tv_usec / 1000000.0;
  }

  double msec() const throw()
  {
    return (itsTimeVal.tv_sec * 1000.0) + (itsTimeVal.tv_usec / 1000.0);
  }

  double usec() const throw()
  {
    return (itsTimeVal.tv_sec * 1000000.0) + double(itsTimeVal.tv_usec);
  }

  const timeval& tval() const throw()
  {
    return itsTimeVal;
  }

private:
  static void normalize(timeval& t) throw()
  {
    // avoid overflow/underflow in the usec field
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
