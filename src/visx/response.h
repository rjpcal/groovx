///////////////////////////////////////////////////////////////////////
//
// response.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 13 18:30:04 2000
// written: Wed Sep 25 19:03:13 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSE_H_DEFINED
#define RESPONSE_H_DEFINED

#include "util/value.h"

/// A Value subclass for observers' responses in a experiment trials.
class Response : public Value
{
public:
  static const int INVALID_VALUE = -1;
  static const int ALWAYS_CORRECT = -2;

  Response(int v = INVALID_VALUE, int m = -1, int c = ALWAYS_CORRECT) :
    itsVal(v), itsMsec(m), itsCorrectVal(c) {}

  virtual ~Response();

  virtual fstring getNativeTypeName() const;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  bool isValid() const { return (itsVal >= 0); }

  bool isCorrect() const
    { return (itsCorrectVal == ALWAYS_CORRECT) || (itsVal == itsCorrectVal); }

  int correctVal() const { return itsCorrectVal; }
  int val() const { return itsVal; }
  int msec() const { return itsMsec; }

  void setCorrectVal(int new_val) { itsCorrectVal = new_val; }
  void setVal(int new_val) { itsVal = new_val; }
  void setMsec(int new_val) { itsMsec = new_val; }

private:
  int itsVal;
  int itsMsec;
  int itsCorrectVal;
};

static const char vcid_response_h[] = "$Header$";
#endif // !RESPONSE_H_DEFINED
