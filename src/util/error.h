///////////////////////////////////////////////////////////////////////
//
// error.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 22 14:59:47 1999
// written: Wed Feb 16 15:05:31 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_H_DEFINED
#define ERROR_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

/**
 *
 * \c Error is the base class for all error classes.
 *
 **/

class Error {
public:
  /// Default constructor.
  Error();
  /// Virtual constructor ensures correct destruction of subclasses.
  virtual ~Error();
};



/**
 *
 * \c ErrorWithMsg is a simple error class that carries a string
 * message describing the error.
 *
 **/

class ErrorWithMsg : public virtual Error {
public:
  /// Default construct with an empty message string.
  ErrorWithMsg() : Error() {}

  /// Construct with an informative message \a errorMessage.
  ErrorWithMsg(const string& errorMessage);

  /// Get a string describing the error.
  virtual const string& msg() const;

protected:
  /// Change the informative message to \a newMessage.
  virtual void setMsg(const string& newMessage);

private:
  string itsInfo;
};

static const char vcid_error_h[] = "$Header$";
#endif // !ERROR_H_DEFINED
