///////////////////////////////////////////////////////////////////////
//
// error.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 22 14:59:47 1999
// written: Wed Aug  8 15:56:08 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_H_DEFINED
#define ERROR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TOSTRING_H_DEFINED)
#include "util/tostring.h"
#endif

class dynamic_string;

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
  Error() : itsInfo(0), itsCount(0)
  { init(); }

  /** Construct with an error message. */
  template <class T1>
  Error(const T1& part1) :
    itsInfo(0), itsCount(0)
  {
    init(); appendOne(part1);
  }

  /** Construct with an error message. */
  template <class T1, class T2>
  Error(const T1& part1, const T2& part2) :
    itsInfo(0), itsCount(0)
  {
    init(); appendOne(part1); appendOne(part2);
  }

  /** Construct with an error message. */
  template <class T1, class T2, class T3>
  Error(const T1& part1, const T2& part2, const T3& part3) :
    itsInfo(0), itsCount(0)
  {
    init(); appendOne(part1); appendOne(part2); appendOne(part3);
  }

  /** Construct with an error message. */
  template <class T1, class T2, class T3, class T4>
  Error(const T1& part1, const T2& part2, const T3& part3,
        const T4& part4) :
    itsInfo(0), itsCount(0)
  {
    init(); appendOne(part1); appendOne(part2); appendOne(part3);
    appendOne(part4);
  }

  /// Copy constructor.
  Error(const Error& other);

  /// Virtual destructor.
  virtual ~Error();

  /// Get a C-style string describing the error.
  virtual const char* msg_cstr() const;

  /** Append additional text to the error message. */
  template <class T1>
  void append(const T1& part1)
  {
    appendOne(part1);
  }

  /** Append additional text to the error message. */
  template <class T1, class T2>
  void append(const T1& part1, const T2& part2)
  {
    appendOne(part1); appendOne(part2);
  }

  /** Append additional text to the error message. */
  template <class T1, class T2, class T3>
  void append(const T1& part1, const T2& part2, const T3& part3)
  {
    appendOne(part1); appendOne(part2); appendOne(part3);
  }

  /** Append additional text to the error message. */
  template <class T1, class T2, class T3, class T4>
  void append(const T1& part1, const T2& part2, const T3& part3,
              const T4& part4)
  {
    appendOne(part1); appendOne(part2); appendOne(part3); appendOne(part4);
  }

  void raise() { throw *this; }

protected:
  /// Change the informative message to \a newMessage.
  virtual void setMsg(const char* newMessage);

  /** Append additional text to the error message. */
  template <class T>
  void appendOne(const T& t)
  {
    doAppend(Util::Convert<T>::toString(t));
  }

  void doAppend(const char* text);

  void init();

private:
  Error& operator=(const Error& other);

  typedef unsigned short ushort;

  dynamic_string* itsInfo;
  ushort* itsCount;
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
