///////////////////////////////////////////////////////////////////////
//
// value.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 28 11:21:32 1999
// written: Sat Oct  2 21:24:15 1999
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


///////////////////////////////////////////////////////////////////////
//
// TValue member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
TValue::~TValue() {}

template <class T>
Value* TValue::clone() const { return new TValue<T>(itsVal); }

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

template <class T> int TValue::getInt() const { throw ValueError(); }
template <class T> long TValue::getLong() const { throw ValueError(); }
template <class T> bool TValue::getBool() const { throw ValueError(); }
template <class T> double TValue::getDouble() const { throw ValueError(); }
template <class T> const char* TValue::getCstring() const { throw ValueError(); }
template <class T> string TValue::getString() const { throw ValueError(); }

template <> int TValue<int>::getInt() const { return itsVal; }
template <> long TValue<long>::getLong() const { return itsVal; }
template <> bool TValue<bool>::getBool() const { return itsVal; }
template <> double TValue<double>::getDouble() const { return itsVal; }
template <> const char* TValue<const char*>::getCstring() const { return itsVal; }
template <> string TValue<string>::getString() const { return itsVal; }

template <class T> void TValue::get(int&) const { throw ValueError(); }
template <class T> void TValue::get(long&) const { throw ValueError(); }
template <class T> void TValue::get(bool&) const { throw ValueError(); }
template <class T> void TValue::get(double&) const { throw ValueError(); }
template <class T> void TValue::get(const char*&) const { throw ValueError(); }
template <class T> void TValue::get(string&) const { throw ValueError(); }

template <> void TValue<int>::get(int& val) const { val = itsVal; }
template <> void TValue<long>::get(long& val) const { val = itsVal; }
template <> void TValue<bool>::get(bool& val) const { val = itsVal; }
template <> void TValue<double>::get(double& val) const { val = itsVal; }
template <> void TValue<const char*>::get(const char*& val) const { val = itsVal; }
template <> void TValue<string>::get(string& val) const { val = itsVal; }


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
TValuePtr::~TValuePtr() {}

template <class T>
Value* TValuePtr::clone() const { return new TValuePtr<T>(*itsValPtr); }

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

template <class T> int TValuePtr::getInt() const { throw ValueError(); }
template <class T> long TValuePtr::getLong() const { throw ValueError(); }
template <class T> bool TValuePtr::getBool() const { throw ValueError(); }
template <class T> double TValuePtr::getDouble() const { throw ValueError(); }
template <class T> const char* TValuePtr::getCstring() const { throw ValueError(); }
template <class T> string TValuePtr::getString() const { throw ValueError(); }

template <> int TValuePtr<int>::getInt() const { return *itsValPtr; }
template <> long TValuePtr<long>::getLong() const { return *itsValPtr; }
template <> bool TValuePtr<bool>::getBool() const { return *itsValPtr; }
template <> double TValuePtr<double>::getDouble() const { return *itsValPtr; }
template <> const char* TValuePtr<const char*>::getCstring() const { return *itsValPtr; }
template <> string TValuePtr<string>::getString() const { return *itsValPtr; }

template <class T> void TValuePtr::get(int&) const { throw ValueError(); }
template <class T> void TValuePtr::get(long&) const { throw ValueError(); }
template <class T> void TValuePtr::get(bool&) const { throw ValueError(); }
template <class T> void TValuePtr::get(double&) const { throw ValueError(); }
template <class T> void TValuePtr::get(const char*&) const { throw ValueError(); }
template <class T> void TValuePtr::get(string&) const { throw ValueError(); }

template <> void TValuePtr<int>::get(int& val) const { val = *itsValPtr; }
template <> void TValuePtr<long>::get(long& val) const { val = *itsValPtr; }
template <> void TValuePtr<bool>::get(bool& val) const { val = *itsValPtr; }
template <> void TValuePtr<double>::get(double& val) const { val = *itsValPtr; }
template <> void TValuePtr<const char*>::get(const char*& val) const { val = *itsValPtr; }
template <> void TValuePtr<string>::get(string& val) const { val = *itsValPtr; }


// Explicit instantiations
template class TValuePtr<int>;
template class TValuePtr<long>;
template class TValuePtr<bool>;
template class TValuePtr<double>;
template class TValuePtr<const char*>;
template class TValuePtr<string>;

static const char vcid_value_cc[] = "$Header$";
#endif // !VALUE_CC_DEFINED
