///////////////////////////////////////////////////////////////////////
//
// error.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 22 14:59:47 1999
// written: Wed Aug  8 12:28:14 2001
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
 * \c Util::Error is a simple error class that carries a string
 * message describing the error.
 *
 **/

class Util::Error {
public:
  /// Default construct with an empty message string.
  Error();

  /// Construct with an informative message \a errorMessage.
  Error(const char* errorMessage);

  /// Copy constructor.
  Error(const Error& other);

  /// Virtual destructor.
  virtual ~Error();

  /// Assignment operator.
  Error& operator=(const Error& other);

  /// Get a C-style string describing the error.
  virtual const char* msg_cstr() const;

  /** Append additional text to the error message. This function
      returns a reference to the invoking object, so several \c
      appendMsg() calls can be chained together. */
  Error& appendMsg(const char* addMsg);

  /** Append additional text to the error message. This function
      returns a reference to the invoking object, so several \c
      appendMsg() calls can be chained together. */
  Error& appendMsg(const char* addMsg1, const char* addMsg2);

  /** Append additional text to the error message. This function
      returns a reference to the invoking object, so several \c
      appendMsg() calls can be chained together. */
  Error& appendMsg(const char* addMsg1, const char* addMsg2, const char* addMsg3);

  /// Append a number to the error message.
  template <class T>
  Error& appendNumber(const T& x)
  {
    return appendMsg(Util::Convert<T>::toString(x));
  }

protected:
  /// Change the informative message to \a newMessage.
  virtual void setMsg(const char* newMessage);

private:
  class Impl;
  friend class Impl;

  dynamic_string* itsInfo;
};

static const char vcid_error_h[] = "$Header$";
#endif // !ERROR_H_DEFINED
