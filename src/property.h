///////////////////////////////////////////////////////////////////////
//
// property.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 10:24:22 1999
// written: Sat Nov 11 10:23:54 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_H_DEFINED
#define PROPERTY_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBSERVABLE_H_DEFINED)
#include "util/observable.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VALUE_H_DEFINED)
#include "util/value.h"
#endif

class fixed_string;

///////////////////////////////////////////////////////////////////////
/**
 *
 * Property defines an interface for getting and setting Value's whose
 * type is not statically known.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Property {
public:
  /// Virtual destructor.
  virtual ~Property();

  ///
  virtual void set(const Value& val) = 0;
  ///
  virtual const Value& get() const = 0;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * TProperty provides a simple implementation of Property using the
 * template class TValue. This provides the benefit that the owner of
 * the TProperty will know the static type represented by its TValue,
 * and can therefor treat the value efficiently as a native type. On
 * the other hand, clients who do not want or need to know the real
 * static type of the TValue can access the value through the more
 * generic Property and Value interfaces.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class TProperty : public Property {
public:
  ///
  TProperty(const T& init=T());

  /// Virtual destructor.
  virtual ~TProperty();

  virtual void set(const Value& new_val) { new_val.get(itsVal.itsVal); }
  virtual const Value& get() const { return itsVal; }

  ///
  virtual void setNative(T new_val) { itsVal.itsVal = new_val; }
  ///
  virtual T getNative() const { return itsVal.itsVal; }

  /// Returns a reference to the stored value
  T& operator()() { return itsVal.itsVal; }
  /// Returns a reference to the stored value
  const T& operator()() const { return itsVal.itsVal; }

  /// The stored value
  TValue<T> itsVal;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * TBoundedProperty is just like TProperty except that it places
 * limits its stored value to a specified range. The range is
 * specified through the template arguments min, max and div, such
 * that the acceptable range will be [min/div, max/div].
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T, int min, int max, int div>
class TBoundedProperty : public TProperty<T> {
public:
  ///
  TBoundedProperty(const T& init=T()) : TProperty<T>(init) {}

public:

  ///
  virtual void set(const Value& new_val)
	 {
		T temp; new_val.get(temp);
		if (temp >= min/T(div) && temp <= max/T(div))
		  TProperty<T>::itsVal.itsVal = temp;
	 }

  ///
  virtual const Value& get() const { return TProperty<T>::itsVal; }

  ///
  virtual void setNative(T new_val)
	 {
		if (new_val >= min/T(div) && new_val <= max/T(div))
		  TProperty<T>::itsVal.itsVal = new_val;
	 }

  ///
  virtual T getNative() const { return TProperty<T>::itsVal.itsVal; }
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * TPtrProperty implements the property interface using a TValuePtr as
 * its stored value. The TValuePtr can be reseated to point at
 * different objects of the same native type.
 *
 * @short A property whose stored value is a reseatable reference.
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class TPtrProperty : public Property {
public:
  ///
  TPtrProperty(T& valRef);

  virtual ~TPtrProperty();

  ///
  void reseat(T& valRef) { itsVal.reseat(valRef); }

  virtual void set(const Value& new_val) { new_val.get(itsVal()); }
  virtual const Value& get() const { return itsVal; }

  ///
  virtual void setNative(T new_val) { itsVal() = new_val; }
  ///
  virtual T getNative() const { return itsVal(); }

  ///
  T& operator()() { return itsVal(); }
  ///
  const T& operator()() const { return itsVal(); }

private:
  TValuePtr<T> itsVal;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * PropFriend is a mixin class that provides get/set functions for
 * classes that own Property's. Additionally, there are get/setNative
 * functions that can be used on TProperty's.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class PropFriend : public virtual Observable {
public:
  ///
  const Value& get(const Property C::* prop) const
	 { return (static_cast<const C*>(this)->*prop).get(); }

  ///
  void set(Property C::* prop, const Value& new_val)
	 { (static_cast<C*>(this)->*prop).set(new_val); sendStateChangeMsg(); }

  ///
  template <class T>
  T getNative(const TProperty<T> C::* prop) const
	 { return (static_cast<const C*>(this)->*prop).getNative(); }

  ///
  template <class T>
  void setNative(TProperty<T> C::* prop, T new_val)
	 { (static_cast<C*>(this)->*prop).setNative(new_val); sendStateChangeMsg(); }
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * PropertyInfoBase provides the bulk of the implementation for the
 * template class PropertyInfo.
 *
 **/
///////////////////////////////////////////////////////////////////////

class PropertyInfoBase {
public:
  template <class T>
  PropertyInfoBase(
    const char* name, T min, T max, T res, bool new_group);

  PropertyInfoBase(const PropertyInfoBase& other);

  virtual ~PropertyInfoBase();

  PropertyInfoBase& operator=(const PropertyInfoBase& other);

  /// Returns a user-friendly name of the property
  const fixed_string& name() const;

  /// Returns a user-friendly name of the property
  const char* name_cstr() const;

  /// Returns the suggested minimum value
  const Value& min() const;
  /// Returns the suggested maximum value
  const Value& max() const;
  /// Returns the suggested resolution
  const Value& res() const;

  /// Returns true if this is the first in a new group of properties
  bool startNewGroup() const;

private:
  class Impl;
  Impl* const itsImpl;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * PropertyInfo stores ancillary information about a property, that
 * might be useful, for example, in constructing a user interface to
 * manipulate various Property's. It stores a human-readable name for
 * the property, a pointer-to-member that refers to the property, as
 * will as suggested minimum and maximum values for the Property's
 * value. Finally, a startNewGroup member provides a simple way to
 * segregate a list of Property's into groups.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class PropertyInfo : public PropertyInfoBase {
public:
  ///
  typedef Property C::* PropPtr;

  ///
  template <class T>
  PropertyInfo(const char* name, PropPtr property,
					T min, T max, T res, bool new_group = false) :
	 PropertyInfoBase( name, min, max, res, new_group),
	 itsProperty(property)
	 {}

  PropertyInfo(const PropertyInfo& other) :
	 PropertyInfoBase(other),
	 itsProperty(other.itsProperty)
	 {}

  PropertyInfo& operator=(const PropertyInfo& other)
	 {
		PropertyInfoBase::operator=(other);
		itsProperty = other.itsProperty;
	 }

public:
  /// Returns a pointer-to-member that refers to the property
  PropPtr property() const { return itsProperty; }

private:
  PropPtr itsProperty;
};


static const char vcid_property_h[] = "$Header$";
#endif // !PROPERTY_H_DEFINED
