///////////////////////////////////////////////////////////////////////
//
// tclvalue.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:23:55 1999
// written: Tue Aug  7 10:32:26 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVALUE_H_DEFINED
#define TCLVALUE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VALUE_H_DEFINED)
#include "util/value.h"
#endif

struct Tcl_Obj;

namespace Tcl
{
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
  TclValue(Tcl_Obj* obj);

  /// Construct with initial value \a val.
  TclValue(int val);
  /// Construct with initial value \a val.
  TclValue(long val);
  /// Construct with initial value \a val.
  TclValue(bool val);
  /// Construct with initial value \a val.
  TclValue(double val);
  /// Construct with initial value \a val.
  TclValue(const char* val);

  /// Construct with initial value \a val.
  TclValue(const Value& val);

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

  virtual int get(Util::TypeCue<int>) const;
  virtual long get(Util::TypeCue<long>) const;
  virtual bool get(Util::TypeCue<bool>) const;
  virtual double get(Util::TypeCue<double>) const;
  virtual const char* get(Util::TypeCue<const char*>) const;

private:
  mutable Tcl_Obj* itsObj;
};

static const char vcid_tclvalue_h[] = "$Header$";
#endif // !TCLVALUE_H_DEFINED
