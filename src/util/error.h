///////////////////////////////////////////////////////////////////////
//
// error.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 22 14:59:47 1999
// written: Wed Aug  8 20:16:37 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_H_DEFINED
#define ERROR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H_DEFINED)
#include "util/strings.h"
#endif

namespace Util
{
  class Error;
}

/**
 *
 * \c Util::Error is a basic error class that carries a string
 * message describing the error.
 *
 **/

class Util::Error {
public:
  /// Default construct with an empty message string.
  Error() : itsInfo() {}

  /** Construct with an error message. */
  template <class T1>
  Error(const T1& part1) :
    itsInfo()
  {
    itsInfo.append(part1);
  }

  /** Construct with an error message. */
  template <class T1, class T2>
  Error(const T1& part1, const T2& part2) :
    itsInfo()
  {
    itsInfo.append(part1); itsInfo.append(part2);
  }

  /** Construct with an error message. */
  template <class T1, class T2, class T3>
  Error(const T1& part1, const T2& part2, const T3& part3) :
    itsInfo()
  {
    itsInfo.append(part1); itsInfo.append(part2); itsInfo.append(part3);
  }

  /** Construct with an error message. */
  template <class T1, class T2, class T3, class T4>
  Error(const T1& part1, const T2& part2, const T3& part3,
        const T4& part4) :
    itsInfo()
  {
    itsInfo.append(part1); itsInfo.append(part2); itsInfo.append(part3);
    itsInfo.append(part4);
  }

  /// Copy constructor.
  Error(const Error& other);

  /// Virtual destructor.
  virtual ~Error();

  /// Get a C-style string describing the error.
  const fstring& msg() const { return itsInfo; }

  /// Get a C-style string describing the error.
  const char* msg_cstr() const { return itsInfo.c_str(); }

  /** Append additional text to the error message. */
  template <class T1>
  void append(const T1& part1)
  {
    itsInfo.append(part1);
  }

  /** Append additional text to the error message. */
  template <class T1, class T2>
  void append(const T1& part1, const T2& part2)
  {
    itsInfo.append(part1); itsInfo.append(part2);
  }

  /** Append additional text to the error message. */
  template <class T1, class T2, class T3>
  void append(const T1& part1, const T2& part2, const T3& part3)
  {
    itsInfo.append(part1); itsInfo.append(part2); itsInfo.append(part3);
  }

  /** Append additional text to the error message. */
  template <class T1, class T2, class T3, class T4>
  void append(const T1& part1, const T2& part2, const T3& part3,
              const T4& part4)
  {
    itsInfo.append(part1); itsInfo.append(part2); itsInfo.append(part3);
    itsInfo.append(part4);
  }

private:
  Error& operator=(const Error& other);

  fstring itsInfo;
};

#define INHERIT_ERROR_CTORS(type) \
  type() : Util::Error() {} \
 \
  template <class T1> \
  type(const T1& x1) : Util::Error(x1) {} \
 \
  template <class T1, class T2> \
  type(const T1& x1, const T2& x2) : Util::Error(x1, x2) {} \
 \
  template <class T1, class T2, class T3> \
  type(const T1& x1, const T2& x2, const T3& x3) : Util::Error(x1, x2, x3) {} \
 \
  template <class T1, class T2, class T3, class T4> \
  type(const T1& x1, const T2& x2, const T3& x3, const T4& x4) : \
    Util::Error(x1, x2, x3, x4) {}

#define FIX_COPY_CTOR(subclass, superclass) \
  subclass(const subclass& other) : superclass((superclass&)other) {}


static const char vcid_error_h[] = "$Header$";
#endif // !ERROR_H_DEFINED
