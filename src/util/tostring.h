///////////////////////////////////////////////////////////////////////
//
// tostring.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sun Aug  5 20:00:26 2001
// written: Wed Mar 19 17:58:51 2003
// $Id$
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

#ifndef TOSTRING_H_DEFINED
#define TOSTRING_H_DEFINED


namespace Util
{
  /// Holds information about a char array and its length
  struct CharData
  {
  private:
    CharData& operator=(const CharData&);

  public:
    CharData(const char* t, unsigned int n) : text(t), len(n) {}

    CharData(const char* t);

    CharData(const CharData& other) : text(other.text), len(other.len) {}

    const char* const text;
    unsigned int const len;
  };

  const char* num2str(bool x);
  const char* num2str(int x);
  const char* num2str(unsigned int x);
  const char* num2str(long x);
  const char* num2str(unsigned long x);
  const char* num2str(double x);

  /// Functor for converting any type to a string (as a CharData).
  template <class T>
  struct Convert
  {
    static CharData toString(const T& x) { return CharData(num2str(x)); }
  };

  /// Specialization for (trivially) converting CharData to CharData.
  template <>
  struct Convert<CharData>
  {
    static CharData toString(const CharData& x) { return x; }
  };

  /// Specialization for converting \c char* to CharData.
  template <>
  struct Convert<char*>
  {
    static CharData toString(char* const& x) { return CharData(x); }
  };

  /// Specialization for converting const char* to CharData.
  template <>
  struct Convert<const char*>
  {
    static CharData toString(const char* const& x) { return CharData(x); }
  };

  /// Specialization for converting string literals (i.e., const char[]'s)
  template <unsigned int N>
  struct Convert<char[N]>
  {
    typedef char CharBlock[N];
    static CharData toString(const CharBlock& x)
    {
      return CharData(&x[0]);
    }
  };

  /// Specialization for converting string literals (i.e., const char[]'s)
  template <unsigned int N>
  struct Convert<const char[N]>
  {
    typedef const char CharBlock[N];
    static CharData toString(const CharBlock& x)
    {
      return CharData(&x[0]);
    }
  };
}

static const char vcid_tostring_h[] = "$Header$";
#endif // !TOSTRING_H_DEFINED
