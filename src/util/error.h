///////////////////////////////////////////////////////////////////////
//
// error.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 22 14:59:47 1999
// written: Wed Nov 10 12:15:08 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_H_DEFINED
#define ERROR_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

/// The base class for all error classes
class Error {
public:
  ///
  Error();
  ///
  virtual ~Error();
};

/// A simple error class that carries a string message describing the error.
class ErrorWithMsg : public virtual Error {
public:
  ///
  ErrorWithMsg() : Error() {}
  ///
  ErrorWithMsg(const string& str);
  ///
  virtual const string& msg() const;
protected:
  ///
  virtual void setMsg(const string& str);
private:
  string itsInfo;
};

static const char vcid_error_h[] = "$Header$";
#endif // !ERROR_H_DEFINED
