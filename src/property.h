///////////////////////////////////////////////////////////////////////
//
// property.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 10:24:22 1999
// written: Wed Sep 29 12:06:26 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_H_DEFINED
#define PROPERTY_H_DEFINED

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef OBSERVABLE_H_DEFINED
#include "observable.h"
#endif

#ifndef VALUE_H_DEFINED
#include "value.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// Property abstract base class definition
//
///////////////////////////////////////////////////////////////////////

class Property {
public:
  template <class C> friend class PropFriend;

  virtual ~Property();

protected:
  virtual void set(const Value& val) = 0;
  virtual const Value& get() const = 0;
};

///////////////////////////////////////////////////////////////////////
//
// TProperty template concrete class defintion
//
///////////////////////////////////////////////////////////////////////

template <class T>
class TProperty : public Property, public virtual IO {
public:
  TProperty(T init=T()) : itsVal(init) {}

  template <class C> friend class PropFriend;

protected:
  virtual void serialize(ostream& os, IOFlag) const 
	 { os << itsVal.itsVal << ' '; }
  virtual void deserialize(istream& is, IOFlag)
	 { is >> itsVal.itsVal; }

  virtual int charCount() const
	 { return gCharCount<T>(itsVal.itsVal); }


  virtual void set(const Value& new_val) { new_val.get(itsVal.itsVal); }
  virtual const Value& get() const { return itsVal; }

  void setNative(T new_val) { itsVal.itsVal = new_val; }
  T getNative() const { return itsVal.itsVal; }

  TValue<T> itsVal;
};

///////////////////////////////////////////////////////////////////////
//
// TBoundedProperty template concrete class definition
//
///////////////////////////////////////////////////////////////////////

template <class T, T min, T max>
class TBoundedProperty : public TProperty<T> {
  TBoundedProperty(T init=T()) : TProperty<T>(init) {}

protected:
  virtual void set(const Value& new_val)
	 {
		T temp; new_val.get(temp);
		if (temp >= min && temp <= max) TProperty<T>::itsVal.itsVal = temp;
	 }
  virtual const Value& get() const { return TProperty<T>::itsVal; }

  void setNative(T new_val)
	 { if (new_val >= min && new_val <= max) TProperty<T>::itsVal.itsVal = new_val; }
  T getNative() const { return TProperty<T>::itsVal.itsVal; }
};

///////////////////////////////////////////////////////////////////////
//
// PropFriend template class definition
//
///////////////////////////////////////////////////////////////////////

template <class C>
class PropFriend : public virtual Observable {
public:
  const Value& get(const Property C::* prop) const
	 { return (static_cast<const C*>(this)->*prop).get(); }

  void set(Property C::* prop, const Value& new_val)
	 { (static_cast<C*>(this)->*prop).set(new_val); sendStateChangeMsg(); }

  template <class T>
  T getNative(const TProperty<T> C::* prop) const
	 { return (static_cast<const C*>(this)->*prop).getNative(); }

  template <class T>
  void setNative(TProperty<T> C::* prop, T new_val)
	 { (static_cast<C*>(this)->*prop).setNative(new_val); }
};

static const char vcid_property_h[] = "$Header$";
#endif // !PROPERTY_H_DEFINED
