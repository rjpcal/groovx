///////////////////////////////////////////////////////////////////////
//
// tostring.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Aug  5 20:00:26 2001
// written: Fri Feb  1 10:56:36 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TOSTRING_H_DEFINED
#define TOSTRING_H_DEFINED


namespace Util
{
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

  template <class T>
  struct Convert
  {
    static CharData toString(const T& x) { return CharData(num2str(x)); }
  };

  template <>
  struct Convert<CharData>
  {
    static CharData toString(const CharData& x) { return x; }
  };

  template <>
  struct Convert<char*>
  {
    static CharData toString(char* const& x) { return CharData(x); }
  };

  template <>
  struct Convert<const char*>
  {
    static CharData toString(const char* const& x) { return CharData(x); }
  };

  // Specialization for string literals (i.e., const char[]'s)
  // WARNING: this will misbehave if passed a char[] that is not a
  // literal... e.g. one that is only partially filled with characters
  template <unsigned int N>
  struct Convert<char[N]>
  {
    typedef char CharBlock[N];
    static CharData toString(const CharBlock& x)
    {
      return CharData(x, N-1);
    }
  };
}

static const char vcid_tostring_h[] = "$Header$";
#endif // !TOSTRING_H_DEFINED
