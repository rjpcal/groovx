///////////////////////////////////////////////////////////////////////
//
// time.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
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

class fstring;

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

  /// Return a formatted string representing the stored timeval.
  /** Formatting codes (see 'man strftime' for more details):

      %a     The abbreviated weekday name according to the current locale.

      %A     The full weekday name according to the current locale.

      %b     The abbreviated month name according to the current locale.

      %B     The full month name according to the current locale.

      %c     The preferred date and time representation for the current locale.

      %C     The century number (year/100) as a 2-digit integer. (SU)

      %d     The day of the month as a decimal number (range 01 to 31).

      %D     Equivalent to %m/%d/%y. (Yecch - for Americans only. Americans
             should note that in other countries %d/%m/%y is rather
             common. This means that in international context this format
             is ambiguous and should not be used.) (SU)

      %e     Like %d, the day of the month as a decimal number, but a leading
             zero is replaced by a space. (SU)

      %E     Modifier: use alternative format, see below. (SU)

      %F     Equivalent to %Y-%m-%d (the ISO 8601 date format). (C99)

      %G     The ISO 8601 year with century as a decimal number. The 4-digit
             year corresponding to the ISO week num­ ber (see %V). This
             has the same format and value as %y, except that if the ISO
             week number belongs to the previous or next year, that year
             is used instead. (TZ)

      %g     Like %G, but without century, i.e., with a 2-digit year
             (00-99). (TZ)

      %h     Equivalent to %b. (SU)

      %H     The hour as a decimal number using a 24-hour clock
             (range 00 to 23).

      %I     The hour as a decimal number using a 12-hour clock
             (range 01 to 12).

      %j     The day of the year as a decimal number (range 001 to 366).

      %k     The hour (24-hour clock) as a decimal number (range
             0 to 23); single digits are preceded by a blank.
             (See also %H.) (TZ)

      %l     The hour (12-hour clock) as a decimal number (range 1 to 12);
             single digits are preceded by a blank. (See also %I.) (TZ)

      %m     The month as a decimal number (range 01 to 12).

      %M     The minute as a decimal number (range 00 to 59).

      %n     A newline character. (SU)

      %O     Modifier: use alternative format, see below. (SU)

      %p     Either `AM' or `PM' according to the given time value, or the
             corresponding strings for the current locale. Noon is
             treated as `pm' and midnight as `am'.

      %P     Like %p but in lowercase: `am' or `pm' or a corresponding string
             for the current locale. (GNU)

      %r     The time in a.m. or p.m. notation. In the POSIX
             locale this is equivalent to `%I:%M:%S %p'. (SU)

      %R     The time in 24-hour notation (%H:%M). (SU) For a
             version including the seconds, see %T below.

      %s     The number of seconds since the Epoch, i.e., since
             1970-01-01 00:00:00 UTC. (TZ)

      %S     The second as a decimal number (range 00 to 61).

      %t     A tab character. (SU)

      %T     The time in 24-hour notation (%H:%M:%S). (SU)

      %u     The day of the week as a decimal, range 1 to 7,
             Monday being 1. See also %w. (SU)

      %U     The week number of the current year as a decimal
             number, range 00 to 53, starting with the first
             Sunday as the first day of week 01. See also %V and
             %W.

      %V     The ISO 8601:1988 week number of the current year as a decimal
             number, range 01 to 53, where week 1 is the first week that
             has at least 4 days in the current year, and with Monday as
             the first day of the week. See also %U and %W. (SU)

      %w     The day of the week as a decimal, range 0 to 6,
             Sunday being 0. See also %u.

      %W     The week number of the current year as a decimal
             number, range 00 to 53, starting with the first
             Monday as the first day of week 01.

      %x     The preferred date representation for the current
             locale without the time.

      %X     The preferred time representation for the current
             locale without the date.

      %y     The year as a decimal number without a century (range 00 to 99).

      %Y     The year as a decimal number including the century.

      %z     The time-zone as hour offset from GMT. Required to
             emit RFC822-conformant dates (using "%a, %d %b %Y
             %H:%M:%S %z"). (GNU)

      %Z     The time zone or name or abbreviation.

      %+     The date and time in date(1) format. (TZ)

      %%     A literal `%' character.

  */
  fstring format(const char* formatstring = "%a %b %d %H:%M:%S %Z %Y") const;

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
