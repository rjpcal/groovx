///////////////////////////////////////////////////////////////////////
//
// tclvalue.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:23:55 1999
// written: Thu Aug  9 07:32:49 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVALUE_H_DEFINED
#define TCLVALUE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLOBJPTR_H_DEFINED)
#include "tcl/tclobjptr.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VALUE_H_DEFINED)
#include "util/value.h"
#endif

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
  template <class T>
  TclValue(T t) : itsObjPtr(t) {}

  /// Construct with initial value \a val.
  TclValue(const Value& val);

  /// Copy constructor.
  TclValue(const TclValue& rhs);

  /// Virtual destructor.
  virtual ~TclValue();

  /// Get the \c Tcl_Obj used as the internal representation.
  Tcl_Obj* getObj() const { return itsObjPtr.obj(); }

  /// Assignment operator.
  TclValue& operator=(const TclValue& other)
    { itsObjPtr = other.itsObjPtr; return *this; }

  /// Assignment from raw \c Tcl_Obj*.
  TclValue& operator=(Tcl_Obj* obj)
    { itsObjPtr = obj; return *this; }

  virtual Value* clone() const;

  virtual fstring getNativeTypeName() const;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  virtual int get(Util::TypeCue<int>) const;
  virtual long get(Util::TypeCue<long>) const;
  virtual bool get(Util::TypeCue<bool>) const;
  virtual double get(Util::TypeCue<double>) const;
  virtual const char* get(Util::TypeCue<const char*>) const;

  virtual void set(int val);
  virtual void set(long val);
  virtual void set(bool val);
  virtual void set(double val);
  virtual void set(const char* val);

  virtual void assignTo(Value& other) const;

private:
  mutable Tcl::ObjPtr itsObjPtr;
};

static const char vcid_tclvalue_h[] = "$Header$";
#endif // !TCLVALUE_H_DEFINED
