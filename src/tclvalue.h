///////////////////////////////////////////////////////////////////////
//
// tclvalue.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:23:55 1999
// written: Fri Nov 10 17:03:49 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVALUE_H_DEFINED
#define TCLVALUE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VALUE_H_DEFINED)
#include "util/value.h"
#endif

struct Tcl_Obj;
struct Tcl_Interp;

namespace Tcl {
  class TclValue;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * A subclass of Value that can be used with Tcl_Obj's. Internally,
 * TclValue uses the Tcl C API to manage conversions among basic
 * types.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::TclValue : public Value {
public:
  /// Construct from a Tcl object \a obj.
  TclValue(Tcl_Interp* interp, Tcl_Obj* obj);

  /// Construct with initial value \a val.
  TclValue(Tcl_Interp* interp, int val);
  /// Construct with initial value \a val.
  TclValue(Tcl_Interp* interp, long val);
  /// Construct with initial value \a val.
  TclValue(Tcl_Interp* interp, bool val);
  /// Construct with initial value \a val.
  TclValue(Tcl_Interp* interp, double val);
  /// Construct with initial value \a val.
  TclValue(Tcl_Interp* interp, const char* val);

  /// Construct with initial value \a val.
  TclValue(Tcl_Interp* interp, const Value& val);

  /// Copy constructor.
  TclValue(const TclValue& rhs);

  /// Virtual destructor.
  virtual ~TclValue();

  /// Get the \c Tcl_Obj used as the internal representation.
  Tcl_Obj* getObj() const;

  /// Change the \c Tcl_Obj used as the internal representation to \a obj.
  void setObj(Tcl_Obj* obj);

  /// Assignment operator.
  TclValue& operator=(const TclValue& other)
	 { setObj(other.itsObj); return *this; }

  /// Assignment from raw \c Tcl_Obj*.
  TclValue& operator=(Tcl_Obj* obj)
	 { setObj(obj); return *this; }

  virtual Value* clone() const;
  
  virtual Type getNativeType() const;

  virtual const char* getNativeTypeName() const;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  virtual int getInt() const;
  virtual long getLong() const;
  virtual bool getBool() const;
  virtual double getDouble() const;
  virtual const char* getCstring() const;

  virtual void get(int& val) const;
  virtual void get(long& val) const;
  virtual void get(bool& val) const;
  virtual void get(double& val) const;
  virtual void get(const char*& val) const;

private:
  mutable Tcl_Interp* itsInterp;
  mutable Tcl_Obj* itsObj;
};

static const char vcid_tclvalue_h[] = "$Header$";
#endif // !TCLVALUE_H_DEFINED
