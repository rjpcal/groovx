///////////////////////////////////////////////////////////////////////
//
// error.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 22 14:59:47 1999
// written: Thu Jun 24 11:21:19 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ERROR_H_DEFINED
#define ERROR_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

class Error {
public:
  Error();
  virtual ~Error();
};

class ErrorWithMsg : public virtual Error {
public:
  ErrorWithMsg() : Error() {}
  ErrorWithMsg(const string& str);
  virtual const string& msg() const;
protected:
  virtual void setMsg(const string& str);
private:
  string itsInfo;
};

static const char vcid_error_h[] = "$Header$";
#endif // !ERROR_H_DEFINED
