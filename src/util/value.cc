///////////////////////////////////////////////////////////////////////
//
// value.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 28 11:21:32 1999
// written: Tue Oct 19 17:07:04 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_CC_DEFINED
#define VALUE_CC_DEFINED

#include "value.h"

ValueError::ValueError(const string& msg) :
  ErrorWithMsg(msg)
{}


Value::~Value() {}

void Value::setInt(int) { throw ValueError(); }
void Value::setLong(long) { throw ValueError(); }
void Value::setBool(bool) { throw ValueError(); }
void Value::setDouble(double) { throw ValueError(); }
void Value::setCstring(const char*) { throw ValueError(); }
void Value::setString(const string&) { throw ValueError(); }


///////////////////////////////////////////////////////////////////////
//
// TValue member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
TValue<T>::~TValue() {}

template <class T>
Value* TValue<T>::clone() const { return new TValue<T>(itsVal); }

template <>
Value::Type TValue<int>::getNativeType() const { return Value::INT; }

template <>
Value::Type TValue<long>::getNativeType() const { return Value::LONG; }

template <>
Value::Type TValue<bool>::getNativeType() const { return Value::BOOL; }

template <>
Value::Type TValue<double>::getNativeType() const { return Value::DOUBLE; }

template <>
Value::Type TValue<const char*>::getNativeType() const { return Value::CSTRING; }

template <>
Value::Type TValue<string>::getNativeType() const { return Value::STRING; }


//
// The get functions default to throwing an exception, but are
// specialized to return the correct value when the type requested
// matches the type of the template parameter.
//

template <class T> int TValue<T>::getInt() const { throw ValueError(); }
template <class T> long TValue<T>::getLong() const { throw ValueError(); }
template <class T> bool TValue<T>::getBool() const { throw ValueError(); }
template <class T> double TValue<T>::getDouble() const { throw ValueError(); }
template <class T> const char* TValue<T>::getCstring() const { throw ValueError(); }
template <class T> string TValue<T>::getString() const { throw ValueError(); }

template <> int TValue<int>::getInt() const { return itsVal; }
template <> long TValue<long>::getLong() const { return itsVal; }
template <> bool TValue<bool>::getBool() const { return itsVal; }
template <> double TValue<double>::getDouble() const { return itsVal; }
template <> const char* TValue<const char*>::getCstring() const { return itsVal; }
template <> string TValue<string>::getString() const { return itsVal; }

template <class T> void TValue<T>::get(int&) const { throw ValueError(); }
template <class T> void TValue<T>::get(long&) const { throw ValueError(); }
template <class T> void TValue<T>::get(bool&) const { throw ValueError(); }
template <class T> void TValue<T>::get(double&) const { throw ValueError(); }
template <class T> void TValue<T>::get(const char*&) const { throw ValueError(); }
template <class T> void TValue<T>::get(string&) const { throw ValueError(); }

template <> void TValue<int>::get(int& val) const { val = itsVal; }
template <> void TValue<long>::get(long& val) const { val = itsVal; }
template <> void TValue<bool>::get(bool& val) const { val = itsVal; }
template <> void TValue<double>::get(double& val) const { val = itsVal; }
template <> void TValue<const char*>::get(const char*& val) const { val = itsVal; }
template <> void TValue<string>::get(string& val) const { val = itsVal; }

template <class T> void TValue<T>::setInt(int) { throw ValueError(); }
template <class T> void TValue<T>::setLong(long) { throw ValueError(); }
template <class T> void TValue<T>::setBool(bool) { throw ValueError(); }
template <class T> void TValue<T>::setDouble(double) { throw ValueError(); }
template <class T> void TValue<T>::setCstring(const char*) { throw ValueError(); }
template <class T> void TValue<T>::setString(const string&) { throw ValueError(); }

template <> void TValue<int>::setInt(int val) { itsVal = val; }
template <> void TValue<long>::setLong(long val) { itsVal = val; }
template <> void TValue<bool>::setBool(bool val) { itsVal = val; }
template <> void TValue<double>::setDouble(double val) { itsVal = val; }
template <> void TValue<const char*>::setCstring(const char* val) { itsVal = val; }
template <> void TValue<string>::setString(const string& val) { itsVal = val; }

// Explicit instantiations
template class TValue<int>;
template class TValue<long>;
template class TValue<bool>;
template class TValue<double>;
template class TValue<const char*>;
template class TValue<string>;

///////////////////////////////////////////////////////////////////////
//
// TValuePtr member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
TValuePtr<T>::~TValuePtr() {}

template <class T>
Value* TValuePtr<T>::clone() const { return new TValuePtr<T>(*itsValPtr); }

template <>
Value::Type TValuePtr<int>::getNativeType() const { return Value::INT; }

template <>
Value::Type TValuePtr<long>::getNativeType() const { return Value::LONG; }

template <>
Value::Type TValuePtr<bool>::getNativeType() const { return Value::BOOL; }

template <>
Value::Type TValuePtr<double>::getNativeType() const { return Value::DOUBLE; }

template <>
Value::Type TValuePtr<const char*>::getNativeType() const { return Value::CSTRING; }

template <>
Value::Type TValuePtr<string>::getNativeType() const { return Value::STRING; }


//
// The get functions default to throwing an exception, but are
// specialized to return the correct value when the type requested
// matches the type of the template parameter.
//

template <class T> int TValuePtr<T>::getInt() const { throw ValueError(); }
template <class T> long TValuePtr<T>::getLong() const { throw ValueError(); }
template <class T> bool TValuePtr<T>::getBool() const { throw ValueError(); }
template <class T> double TValuePtr<T>::getDouble() const { throw ValueError(); }
template <class T> const char* TValuePtr<T>::getCstring() const { throw ValueError(); }
template <class T> string TValuePtr<T>::getString() const { throw ValueError(); }

template <> int TValuePtr<int>::getInt() const { return *itsValPtr; }
template <> long TValuePtr<long>::getLong() const { return *itsValPtr; }
template <> bool TValuePtr<bool>::getBool() const { return *itsValPtr; }
template <> double TValuePtr<double>::getDouble() const { return *itsValPtr; }
template <> const char* TValuePtr<const char*>::getCstring() const { return *itsValPtr; }
template <> string TValuePtr<string>::getString() const { return *itsValPtr; }

template <class T> void TValuePtr<T>::get(int&) const { throw ValueError(); }
template <class T> void TValuePtr<T>::get(long&) const { throw ValueError(); }
template <class T> void TValuePtr<T>::get(bool&) const { throw ValueError(); }
template <class T> void TValuePtr<T>::get(double&) const { throw ValueError(); }
template <class T> void TValuePtr<T>::get(const char*&) const { throw ValueError(); }
template <class T> void TValuePtr<T>::get(string&) const { throw ValueError(); }

template <> void TValuePtr<int>::get(int& val) const { val = *itsValPtr; }
template <> void TValuePtr<long>::get(long& val) const { val = *itsValPtr; }
template <> void TValuePtr<bool>::get(bool& val) const { val = *itsValPtr; }
template <> void TValuePtr<double>::get(double& val) const { val = *itsValPtr; }
template <> void TValuePtr<const char*>::get(const char*& val) const { val = *itsValPtr; }
template <> void TValuePtr<string>::get(string& val) const { val = *itsValPtr; }

template <class T> void TValuePtr<T>::setInt(int) { throw ValueError(); }
template <class T> void TValuePtr<T>::setLong(long) { throw ValueError(); }
template <class T> void TValuePtr<T>::setBool(bool) { throw ValueError(); }
template <class T> void TValuePtr<T>::setDouble(double) { throw ValueError(); }
template <class T> void TValuePtr<T>::setCstring(const char*) { throw ValueError(); }
template <class T> void TValuePtr<T>::setString(const string&) { throw ValueError(); }

template <> void TValuePtr<int>::setInt(int val) { *itsValPtr = val; }
template <> void TValuePtr<long>::setLong(long val) { *itsValPtr = val; }
template <> void TValuePtr<bool>::setBool(bool val) { *itsValPtr = val; }
template <> void TValuePtr<double>::setDouble(double val) { *itsValPtr = val; }
template <> void TValuePtr<const char*>::setCstring(const char* val) { *itsValPtr = val; }
template <> void TValuePtr<string>::setString(const string& val) { *itsValPtr = val; }


// Explicit instantiations
template class TValuePtr<int>;
template class TValuePtr<long>;
template class TValuePtr<bool>;
template class TValuePtr<double>;
template class TValuePtr<const char*>;
template class TValuePtr<string>;

static const char vcid_value_cc[] = "$Header$";
#endif // !VALUE_CC_DEFINED
