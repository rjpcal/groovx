///////////////////////////////////////////////////////////////////////
//
// tclvalue.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 28 11:23:55 1999
// written: Tue Dec  7 18:04:11 1999
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

namespace Tcl {

///////////////////////////////////////////////////////////////////////
/**
 *
 * A subclass of Value that can be used with Tcl_Obj's. Internally,
 * TclValue uses the Tcl C API to manage conversions among basic
 * types.
 *
 **/
///////////////////////////////////////////////////////////////////////

class TclValue : public ::Value {
public:
  TclValue(Tcl_Interp* interp, Tcl_Obj* obj);

  TclValue(Tcl_Interp* interp, int val);
  TclValue(Tcl_Interp* interp, long val);
  TclValue(Tcl_Interp* interp, bool val);
  TclValue(Tcl_Interp* interp, double val);
  TclValue(Tcl_Interp* interp, const char* val);
  TclValue(Tcl_Interp* interp, const string& val);

  TclValue(Tcl_Interp* interp, const Value& rhs);

  TclValue(const TclValue& rhs);

  virtual ~TclValue();

  Tcl_Obj* getObj() const;
  void setObj(Tcl_Obj* obj);

  TclValue& operator=(Tcl_Obj* obj) { setObj(obj); return *this; }

  virtual Value* clone() const;
  
  virtual Type getNativeType() const;

  virtual const char* getNativeTypeName() const;

  virtual void printTo(ostream& os) const;
  virtual void scanFrom(istream& is);

  virtual int getInt() const;
  virtual long getLong() const;
  virtual bool getBool() const;
  virtual double getDouble() const;
  virtual const char* getCstring() const;
  virtual string getString() const;

  virtual void get(int& val) const;
  virtual void get(long& val) const;
  virtual void get(bool& val) const;
  virtual void get(double& val) const;
  virtual void get(const char*& val) const;
  virtual void get(string& val) const;

private:
  mutable Tcl_Interp* itsInterp;
  mutable Tcl_Obj* itsObj;
};

} // end namespace Tcl

static const char vcid_tclvalue_h[] = "$Header$";
#endif // !TCLVALUE_H_DEFINED
