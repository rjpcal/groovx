///////////////////////////////////////////////////////////////////////
//
// responsehandler.h
// Rob Peters
// created: Tue May 18 16:21:09 1999
// written: Mon Jul 19 16:31:12 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSEHANDLER_H_DEFINED
#define RESPONSEHANDLER_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

struct Tcl_Interp;

///////////////////////////////////////////////////////////////////////
//
// ResponseHandler class defintion
//
///////////////////////////////////////////////////////////////////////

class ResponseHandler : public virtual IO {
public:
  // creators
  ResponseHandler();
  virtual ~ResponseHandler();

  virtual void serialize(ostream &os, IOFlag flag) const = 0;
  virtual void deserialize(istream &is, IOFlag flag) = 0;
  virtual int charCount() const = 0;

  // constants
  static const int INVALID_RESPONSE = -1;

  // manipulators/accessors
  virtual void setInterp(Tcl_Interp* interp) = 0;

  // actions
  virtual void rhBeginTrial() const = 0;
  virtual void rhAbortTrial() const = 0;
  virtual void rhHaltExpt() const = 0;

protected:
  mutable Tcl_Interp* itsInterp;
};

static const char vcid_responsehandler_h[] = "$Header$";
#endif // !RESPONSEHANDLER_H_DEFINED
