///////////////////////////////////////////////////////////////////////
//
// value.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 28 11:19:17 1999
// written: Tue Sep 28 11:20:46 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_H_DEFINED
#define VALUE_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// Value abstract class definition
//
///////////////////////////////////////////////////////////////////////

class ValueError : public ErrorWithMsg {
public:
  ValueError(const string& msg="");
};

class Value {
public:

  enum Type { INT, LONG, BOOL, DOUBLE, CSTRING, STRING };

  virtual Type getNativeType() = 0;

  virtual int getInt() = 0;
  virtual long getLong() = 0;
  virtual bool getBool() = 0;
  virtual double getDouble() = 0;
  virtual const char* getCstring() = 0;
  virtual string getString() = 0;
}; 

static const char vcid_value_h[] = "$Header$";
#endif // !VALUE_H_DEFINED
