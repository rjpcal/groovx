///////////////////////////////////////////////////////////////////////
//
// tclvalue.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 28 11:23:55 1999
// written: Tue Sep 28 12:05:00 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVALUE_H_DEFINED
#define TCLVALUE_H_DEFINED

#ifndef VALUE_H_DEFINED
#include "value.h"
#endif

struct Tcl_Obj;
struct Tcl_Interp;

///////////////////////////////////////////////////////////////////////
//
// TclValue class definition
//
///////////////////////////////////////////////////////////////////////

class TclValue : public Value {
public:
  TclValue(Tcl_Interp* interp, Tcl_Obj* obj);

  TclValue(Tcl_Interp* interp, int val);
  TclValue(Tcl_Interp* interp, long val);
  TclValue(Tcl_Interp* interp, bool val);
  TclValue(Tcl_Interp* interp, double val);
  TclValue(Tcl_Interp* interp, const char* val);
  TclValue(Tcl_Interp* interp, const string& val);

  virtual ~TclValue();

  virtual Type getNativeType();

  virtual int getInt();
  virtual long getLong();
  virtual bool getBool();
  virtual double getDouble();
  virtual const char* getCstring();
  virtual string getString();

  virtual void get(int& val);
  virtual void get(long& val);
  virtual void get(bool& val);
  virtual void get(double& val);
  virtual void get(const char*& val);
  virtual void get(string& val);

private:
  Tcl_Interp* itsInterp;
  Tcl_Obj* itsObj;
};

static const char vcid_tclvalue_h[] = "$Header$";
#endif // !TCLVALUE_H_DEFINED
