///////////////////////////////////////////////////////////////////////
//
// response.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Mar 13 18:30:04 2000
// written: Wed Mar 22 16:47:02 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSE_H_DEFINED
#define RESPONSE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VALUE_H)
#include "value.h"
#endif

class Response : public Value {
public:
  static const int INVALID_VALUE = -1;

  Response(int v = -1, int m = -1) : itsVal(v), itsMsec(m) {}

  virtual Value* clone() const;
  virtual Type getNativeType() const;
  virtual const char* getNativeTypeName() const;

  virtual void printTo(ostream& os) const;
  virtual void scanFrom(istream& is);

  bool isValid() const { return (itsVal >= 0); }

  int val() const { return itsVal; }
  int msec() const { return itsMsec; }

  void setVal(int new_val) { itsVal = new_val; }
  void setMsec(int new_val) { itsMsec = new_val; }

private:
  int itsVal;
  int itsMsec;
};

static const char vcid_response_h[] = "$Header$";
#endif // !RESPONSE_H_DEFINED
