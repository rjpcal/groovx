///////////////////////////////////////////////////////////////////////
//
// property.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 10:24:22 1999
// written: Tue Nov 30 16:44:26 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_H_DEFINED
#define PROPERTY_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef MEMORY_DEFINED
#include <memory>
#define MEMORY_DEFINED
#endif

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef OBSERVABLE_H_DEFINED
#include "observable.h"
#endif

#ifndef VALUE_H_DEFINED
#include "value.h"
#endif

#ifndef READER_H_DEFINED
#include "reader.h"
#endif

#ifndef WRITER_H_DEFINED
#include "writer.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * Property defines an interface for getting and setting Value's whose
 * type is not statically known.
 *
 * @memo Abstract base class for get/set dynamically-typed Value's.
 **/

class Property {
public:
  ///
  template <class C> friend class PropFriend;

  ///
  virtual ~Property();

#ifndef GCC_COMPILER
protected:
#else
public:
#endif

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
 * generic Property and Value interfaces. The native-type related
 * functions are protected, but CTProperty allows friendship to be
 * granted to one class (probably the owner of the property).
 *
 * @memo Simple implementation of Property using TValue template.
 **/

template <class T>
class TProperty : public Property, public virtual IO {
public:
  ///
  TProperty(T init=T()) : itsVal(init) {}

  ///
  template <class C> friend class PropFriend;

  ///
  virtual void serialize(ostream& os, IOFlag) const 
	 { os << itsVal.itsVal << ' '; }
  ///
  virtual void deserialize(istream& is, IOFlag)
	 { is >> itsVal.itsVal; }

  ///
  virtual int charCount() const
	 { return gCharCount<T>(itsVal.itsVal); }

  ///
  virtual void readFrom(Reader* reader)
	 { reader->readValue("value", itsVal.itsVal); }

  ///
  virtual void writeTo(Writer* writer) const
	 { writer->writeValue("value", itsVal.itsVal); }
  
#ifndef GCC_COMPILER
protected:
#else
public:
#endif

  ///
  virtual void set(const Value& new_val) { new_val.get(itsVal.itsVal); }
  ///
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

template <>
void TProperty<bool>::deserialize(istream& is, IOFlag) {
  int temp; is >> temp; itsVal.itsVal = bool(temp);
}


///////////////////////////////////////////////////////////////////////
/**
 *
 * CTProperty simply injects a friend declaration into the TProperty
 * interface, so that the class that owns a TProperty can access its
 * native-type interface.
 *
 * @memo Allows an owner class to have friendship of a TProperty.
 **/
template <class C, class T>
class CTProperty : public TProperty<T> { 
public:
  ///
  CTProperty(T init = T()) : TProperty<T>(init) {}

#ifndef GCC_COMPILER
  ///
  friend class C;
#endif

};

///////////////////////////////////////////////////////////////////////
/**
 *
 * TBoundedProperty is just like TProperty except that it places
 * limits its stored value to a specified range. The range is
 * specified through the template arguments min, max and div, such
 * that the acceptable range will be [min/div, max/div].
 *
 * @memo Like TProperty, but limits the range of the stored value.
 **/

template <class T, int min, int max, int div>
class TBoundedProperty : public TProperty<T> {
public:
  ///
  TBoundedProperty(T init=T()) : TProperty<T>(init) {}

#ifndef GCC_COMPILER
protected:
#else
public:
#endif

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
  T virtual getNative() const { return TProperty<T>::itsVal.itsVal; }
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * CTBoundedProperty simply injects a friend declaration into the
 * TBoundedProperty interface, so that the class that owns a TProperty
 * can access its native-type interface.
 *
 * @memo Allows an owner class to have friendship of a TBoundedProperty.
 **/

template <class C, class T, int min, int max, int div>
class CTBoundedProperty : public TBoundedProperty<T, min, max, div> { 
public:
  ///
  CTBoundedProperty(T init = T()) : TBoundedProperty<T, min, max, div>(init) {}

#ifndef GCC_COMPILER
  ///
  friend class C;
#endif

};


///////////////////////////////////////////////////////////////////////
/**
 *
 * TPtrProperty implements the property interface using a TValuePtr as
 * its stored value. The TValuePtr can be reseated to point at
 * different objects of the same native type.
 *
 * @memo A property whose stored value is a reseatable reference.
 **/

template <class T>
class TPtrProperty : public Property, public virtual IO {
public:
  ///
  TPtrProperty(T& valRef) : itsVal(valRef) {}

  ///
  template <class C> friend class PropFriend;

#ifndef GCC_COMPILER
protected:
#else
public:
#endif

  ///
  void reseat(T& valRef) { itsVal.reseat(valRef); }

  ///
  virtual void serialize(ostream& os, IOFlag) const 
	 { os << itsVal() << ' '; }
  ///
  virtual void deserialize(istream& is, IOFlag)
	 { is >> itsVal(); }

  ///
  virtual int charCount() const
	 { return gCharCount<T>(itsVal()); }

  ///
  virtual void readFrom(Reader* reader)
	 { reader->readValue("value", itsVal()); }

  ///
  virtual void writeTo(Writer* writer) const
	 { writer->writeValue("value", itsVal()); }
  

  ///
  virtual void set(const Value& new_val) { new_val.get(itsVal()); }
  ///
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
 * CTPtrProperty simply injects a friend declaration into the
 * TPtrProperty interface, so that the class that owns a TProperty can
 * access its native-type interface.
 *
 * @memo Allows an owner class to have friendship of a TPtrProperty.
 **/

template <class C, class T>
class CTPtrProperty : public TPtrProperty<T> {
public:
  ///
  CTPtrProperty(T& valRef) : TPtrProperty<T>(valRef) {}

#ifndef GCC_COMPILER
  friend class C;
#endif

};

///////////////////////////////////////////////////////////////////////
/**
 *
 * PropFriend is a mixin class that provides get/set functions for
 * classes that own Property's. Additionally, there are get/setNative
 * functions that can be used on TProperty's.
 *
 * @memo Mixin class to provide get/set functions for classes that own
 * Property's.
 **/

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
 * PropertyInfo stores ancillary information about a property, that
 * might be useful, for example, in constructing a user interface to
 * manipulate various Property's. It stores a human-readable name for
 * the property, a pointer-to-member that refers to the property, as
 * will as suggested minimum and maximum values for the Property's
 * value. Finally, a startNewGroup member provides a simple way to
 * segregate a list of Property's into groups.
 *
 * @memo Stores ancillary information about a property.
 **/

template <class C>
class PropertyInfo {
public:
  ///
  typedef Property C::* PropPtr;

  ///
  template <class T>
  PropertyInfo(const string& name_, PropPtr property_,
					T min_, T max_, T res_, bool new_group = false) :
	 name(name_),
	 property(property_),
	 min(new TValue<T>(min_)),
	 max(new TValue<T>(max_)),
	 res(new TValue<T>(res_)),
	 startNewGroup(new_group) 
  {}
  
  ///
  PropertyInfo(const PropertyInfo& rhs) :
	 name(rhs.name),
	 property(rhs.property),
	 min(rhs.min->clone()),
	 max(rhs.max->clone()),
	 res(rhs.res->clone()),
	 startNewGroup(rhs.startNewGroup)
  {}

  ///
  PropertyInfo& operator=(const PropertyInfo& rhs) {
	 name = rhs.name;
	 property = rhs.property;
	 min.reset(rhs.min->clone());
	 max.reset(rhs.max->clone());
	 res.reset(rhs.res->clone());
	 startNewGroup = rhs.startNewGroup;
	 return *this;
  }

  /// User-friendly name of the property
  string name;
  /// Pointer-to-member that refers to the property
  PropPtr property;
  /// Suggested minimum value
  auto_ptr<Value> min;
  /// Suggested maximum value
  auto_ptr<Value> max;
  /// Suggested resolution
  auto_ptr<Value> res;
  /// True if this is the first in a new group of properties
  bool startNewGroup;
};

static const char vcid_property_h[] = "$Header$";
#endif // !PROPERTY_H_DEFINED
