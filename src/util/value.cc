///////////////////////////////////////////////////////////////////////
//
// value.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:21:32 1999
// written: Tue Aug  7 10:43:20 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_CC_DEFINED
#define VALUE_CC_DEFINED

#include "util/value.h"

#include <iostream.h>

#include "util/trace.h"

namespace VALUE_CC_LOCAL {
  void raiseValueError(const char* msg ="") { throw ValueError(msg); }
}

using namespace VALUE_CC_LOCAL;

ValueError::ValueError() :
  ErrorWithMsg()
{}

ValueError::ValueError(const char* msg) :
  ErrorWithMsg(msg)
{}

ValueError::~ValueError() {}

Value::Value() {
DOTRACE("Value::Value");
}

Value::~Value() {
DOTRACE("Value::~Value");
}


void Value::printTo(STD_IO::ostream&) const { raiseValueError("printTo"); }
void Value::scanFrom(STD_IO::istream&) { raiseValueError("scanFrom"); }

int Value::get(Util::TypeCue<int>) const { raiseValueError("getInt"); return 0; }
long Value::get(Util::TypeCue<long>) const { raiseValueError("getLong"); return 0; }
bool Value::get(Util::TypeCue<bool>) const { raiseValueError("getBool"); return false; }
double Value::get(Util::TypeCue<double>) const { raiseValueError("getDouble"); return 0.0; }
const char* Value::get(Util::TypeCue<const char*>) const { raiseValueError("getCstring"); return ""; }

void Value::set(int) { raiseValueError("set(int)"); }
void Value::set(long) { raiseValueError("set(long)"); }
void Value::set(bool) { raiseValueError("set(bool)"); }
void Value::set(double) { raiseValueError("set(double)"); }
void Value::set(const char*) { raiseValueError("set(const char*)"); }

///////////////////////////////////////////////////////////////////////
//
// TValue member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
TValue<T>::~TValue() {}

template <class T>
Value* TValue<T>::clone() const { return new TValue<T>(itsVal); }

template <class T>
void TValue<T>::printTo(STD_IO::ostream& os) const { os << itsVal; }

template <class T>
void TValue<T>::scanFrom(STD_IO::istream& is) { is >> itsVal; }

template <>
void TValue<bool>::scanFrom(STD_IO::istream& is)
{ int temp; is >> temp; itsVal = bool(temp); }

template <>
Value::Type TValue<int>::getNativeType() const { return Value::INT; }

template <>
Value::Type TValue<long>::getNativeType() const { return Value::LONG; }

template <>
Value::Type TValue<bool>::getNativeType() const { return Value::BOOL; }

template <>
Value::Type TValue<double>::getNativeType() const { return Value::DOUBLE; }


template <>
const char* TValue<int>::getNativeTypeName() const { return "int"; }

template <>
const char* TValue<long>::getNativeTypeName() const { return "long"; }

template <>
const char* TValue<bool>::getNativeTypeName() const { return "bool"; }

template <>
const char* TValue<double>::getNativeTypeName() const { return "double"; }


//
// The get functions default to throwing an exception, but are
// specialized to return the correct value when the type requested
// matches the type of the template parameter.
//

template <class T> int TValue<T>::get(Util::TypeCue<int>) const { return Value::get(Util::TypeCue<int>()); }
template <class T> long TValue<T>::get(Util::TypeCue<long>) const { return Value::get(Util::TypeCue<long>()); }
template <class T> bool TValue<T>::get(Util::TypeCue<bool>) const { return Value::get(Util::TypeCue<bool>()); }
template <class T> double TValue<T>::get(Util::TypeCue<double>) const { return Value::get(Util::TypeCue<double>()); }
template <class T> const char* TValue<T>::get(Util::TypeCue<const char*>) const { return Value::get(Util::TypeCue<const char*>()); }

template <> int TValue<int>::get(Util::TypeCue<int>) const { return itsVal; }
template <> long TValue<long>::get(Util::TypeCue<long>) const { return itsVal; }
template <> bool TValue<bool>::get(Util::TypeCue<bool>) const { return itsVal; }
template <> double TValue<double>::get(Util::TypeCue<double>) const { return itsVal; }

template <class T> void TValue<T>::set(int val) { Value::set(val); }
template <class T> void TValue<T>::set(long val) { Value::set(val); }
template <class T> void TValue<T>::set(bool val) { Value::set(val); }
template <class T> void TValue<T>::set(double val) { Value::set(val); }
template <class T> void TValue<T>::set(const char* val) { Value::set(val); }

template <> void TValue<int>::set(int val) { itsVal = val; }
template <> void TValue<long>::set(long val) { itsVal = val; }
template <> void TValue<bool>::set(bool val) { itsVal = val; }
template <> void TValue<double>::set(double val) { itsVal = val; }

// Explicit instantiations
template class TValue<int>;
template class TValue<long>;
template class TValue<bool>;
template class TValue<double>;

///////////////////////////////////////////////////////////////////////
//
// TValuePtr member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
TValuePtr<T>::~TValuePtr() {}

template <class T>
Value* TValuePtr<T>::clone() const { return new TValuePtr<T>(*itsValPtr); }

template <class T>
void TValuePtr<T>::printTo(STD_IO::ostream& os) const { os << *itsValPtr; }

template <class T>
void TValuePtr<T>::scanFrom(STD_IO::istream& is) { is >> *itsValPtr; }

template <>
void TValuePtr<bool>::scanFrom(STD_IO::istream& is)
{ int temp; is >> temp; *itsValPtr = bool(temp); }

template <>
Value::Type TValuePtr<int>::getNativeType() const { return Value::INT; }

template <>
Value::Type TValuePtr<long>::getNativeType() const { return Value::LONG; }

template <>
Value::Type TValuePtr<bool>::getNativeType() const { return Value::BOOL; }

template <>
Value::Type TValuePtr<double>::getNativeType() const { return Value::DOUBLE; }


template <>
const char* TValuePtr<int>::getNativeTypeName() const { return "int"; }

template <>
const char* TValuePtr<long>::getNativeTypeName() const { return "long"; }

template <>
const char* TValuePtr<bool>::getNativeTypeName() const { return "bool"; }

template <>
const char* TValuePtr<double>::getNativeTypeName() const { return "double"; }


//
// The get functions default to throwing an exception, but are
// specialized to return the correct value when the type requested
// matches the type of the template parameter.
//

template <class T> int TValuePtr<T>::get(Util::TypeCue<int>) const { return Value::get(Util::TypeCue<int>()); }
template <class T> long TValuePtr<T>::get(Util::TypeCue<long>) const { return Value::get(Util::TypeCue<long>()); }
template <class T> bool TValuePtr<T>::get(Util::TypeCue<bool>) const { return Value::get(Util::TypeCue<bool>()); }
template <class T> double TValuePtr<T>::get(Util::TypeCue<double>) const { return Value::get(Util::TypeCue<double>()); }
template <class T> const char* TValuePtr<T>::get(Util::TypeCue<const char*>) const { return Value::get(Util::TypeCue<const char*>()); }

template <> int TValuePtr<int>::get(Util::TypeCue<int>) const { return *itsValPtr; }
template <> long TValuePtr<long>::get(Util::TypeCue<long>) const { return *itsValPtr; }
template <> bool TValuePtr<bool>::get(Util::TypeCue<bool>) const { return *itsValPtr; }
template <> double TValuePtr<double>::get(Util::TypeCue<double>) const { return *itsValPtr; }

template <class T> void TValuePtr<T>::set(int val) { Value::set(val); }
template <class T> void TValuePtr<T>::set(long val) { Value::set(val); }
template <class T> void TValuePtr<T>::set(bool val) { Value::set(val); }
template <class T> void TValuePtr<T>::set(double val) { Value::set(val); }
template <class T> void TValuePtr<T>::set(const char* val) { Value::set(val); }

template <> void TValuePtr<int>::set(int val) { *itsValPtr = val; }
template <> void TValuePtr<long>::set(long val) { *itsValPtr = val; }
template <> void TValuePtr<bool>::set(bool val) { *itsValPtr = val; }
template <> void TValuePtr<double>::set(double val) { *itsValPtr = val; }


// Explicit instantiations
template class TValuePtr<int>;
template class TValuePtr<long>;
template class TValuePtr<bool>;
template class TValuePtr<double>;

static const char vcid_value_cc[] = "$Header$";
#endif // !VALUE_CC_DEFINED
