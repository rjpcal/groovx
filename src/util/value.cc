///////////////////////////////////////////////////////////////////////
//
// value.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:21:32 1999
// written: Thu May 10 12:04:41 2001
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

int Value::getInt() const { raiseValueError("getInt"); return 0; }
long Value::getLong() const { raiseValueError("getLong"); return 0; }
bool Value::getBool() const { raiseValueError("getBool"); return false; }
double Value::getDouble() const { raiseValueError("getDouble"); return 0.0; }
const char* Value::getCstring() const { raiseValueError("getCstring"); return ""; }

void Value::get(int&) const { raiseValueError("get(int&)"); }
void Value::get(long&) const { raiseValueError("get(long&)"); }
void Value::get(bool&) const { raiseValueError("get(bool&)"); }
void Value::get(double&) const { raiseValueError("get(double&)"); }
void Value::get(const char*&) const { raiseValueError("get(const char*&)"); }

void Value::setInt(int) { raiseValueError("setInt"); }
void Value::setLong(long) { raiseValueError("setLong"); }
void Value::setBool(bool) { raiseValueError("setBool"); }
void Value::setDouble(double) { raiseValueError("setDouble"); }
void Value::setCstring(const char*) { raiseValueError("setCstring"); }


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

template <class T> int TValue<T>::getInt() const { return Value::getInt(); }
template <class T> long TValue<T>::getLong() const { return Value::getLong(); }
template <class T> bool TValue<T>::getBool() const { return Value::getBool(); }
template <class T> double TValue<T>::getDouble() const { return Value::getDouble(); }
template <class T> const char* TValue<T>::getCstring() const { return Value::getCstring(); }

template <> int TValue<int>::getInt() const { return itsVal; }
template <> long TValue<long>::getLong() const { return itsVal; }
template <> bool TValue<bool>::getBool() const { return itsVal; }
template <> double TValue<double>::getDouble() const { return itsVal; }

template <class T> void TValue<T>::get(int& val) const { Value::get(val); }
template <class T> void TValue<T>::get(long& val) const { Value::get(val); }
template <class T> void TValue<T>::get(bool& val) const { Value::get(val); }
template <class T> void TValue<T>::get(double& val) const { Value::get(val); }
template <class T> void TValue<T>::get(const char*& val) const { Value::get(val); }

template <> void TValue<int>::get(int& val) const { val = itsVal; }
template <> void TValue<long>::get(long& val) const { val = itsVal; }
template <> void TValue<bool>::get(bool& val) const { val = itsVal; }
template <> void TValue<double>::get(double& val) const { val = itsVal; }

template <class T> void TValue<T>::setInt(int val) { Value::setInt(val); }
template <class T> void TValue<T>::setLong(long val) { Value::setLong(val); }
template <class T> void TValue<T>::setBool(bool val) { Value::setBool(val); }
template <class T> void TValue<T>::setDouble(double val) { Value::setDouble(val); }
template <class T> void TValue<T>::setCstring(const char* val) { Value::setCstring(val); }

template <> void TValue<int>::setInt(int val) { itsVal = val; }
template <> void TValue<long>::setLong(long val) { itsVal = val; }
template <> void TValue<bool>::setBool(bool val) { itsVal = val; }
template <> void TValue<double>::setDouble(double val) { itsVal = val; }

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

template <class T> int TValuePtr<T>::getInt() const { return Value::getInt(); }
template <class T> long TValuePtr<T>::getLong() const { return Value::getLong(); }
template <class T> bool TValuePtr<T>::getBool() const { return Value::getBool(); }
template <class T> double TValuePtr<T>::getDouble() const { return Value::getDouble(); }
template <class T> const char* TValuePtr<T>::getCstring() const { return Value::getCstring(); }

template <> int TValuePtr<int>::getInt() const { return *itsValPtr; }
template <> long TValuePtr<long>::getLong() const { return *itsValPtr; }
template <> bool TValuePtr<bool>::getBool() const { return *itsValPtr; }
template <> double TValuePtr<double>::getDouble() const { return *itsValPtr; }

template <class T> void TValuePtr<T>::get(int& val) const { Value::get(val); }
template <class T> void TValuePtr<T>::get(long& val) const { Value::get(val); }
template <class T> void TValuePtr<T>::get(bool& val) const { Value::get(val); }
template <class T> void TValuePtr<T>::get(double& val) const { Value::get(val); }
template <class T> void TValuePtr<T>::get(const char*& val) const { Value::get(val); }

template <> void TValuePtr<int>::get(int& val) const { val = *itsValPtr; }
template <> void TValuePtr<long>::get(long& val) const { val = *itsValPtr; }
template <> void TValuePtr<bool>::get(bool& val) const { val = *itsValPtr; }
template <> void TValuePtr<double>::get(double& val) const { val = *itsValPtr; }

template <class T> void TValuePtr<T>::setInt(int val) { Value::setInt(val); }
template <class T> void TValuePtr<T>::setLong(long val) { Value::setLong(val); }
template <class T> void TValuePtr<T>::setBool(bool val) { Value::setBool(val); }
template <class T> void TValuePtr<T>::setDouble(double val) { Value::setDouble(val); }
template <class T> void TValuePtr<T>::setCstring(const char* val) { Value::setCstring(val); }

template <> void TValuePtr<int>::setInt(int val) { *itsValPtr = val; }
template <> void TValuePtr<long>::setLong(long val) { *itsValPtr = val; }
template <> void TValuePtr<bool>::setBool(bool val) { *itsValPtr = val; }
template <> void TValuePtr<double>::setDouble(double val) { *itsValPtr = val; }


// Explicit instantiations
template class TValuePtr<int>;
template class TValuePtr<long>;
template class TValuePtr<bool>;
template class TValuePtr<double>;

static const char vcid_value_cc[] = "$Header$";
#endif // !VALUE_CC_DEFINED
