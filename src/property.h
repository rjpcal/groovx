///////////////////////////////////////////////////////////////////////
//
// property.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 10:24:22 1999
// written: Wed Oct  6 10:02:08 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_H_DEFINED
#define PROPERTY_H_DEFINED

#ifndef STRING_DEFINED
#include "string"
#define STRING_DEFINED
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

  virtual void setNative(T new_val) { itsVal.itsVal = new_val; }
  virtual T getNative() const { return itsVal.itsVal; }

  T& operator()() { return itsVal.itsVal; }
  const T& operator()() const { return itsVal.itsVal; }

  TValue<T> itsVal;
};

template <>
void TProperty<bool>::deserialize(istream& is, IOFlag) {
  int temp; is >> temp; itsVal.itsVal = bool(temp);
}

template <class C, class T>
class CTProperty : public TProperty<T> { 
public:
  CTProperty(T init = T()) : TProperty<T>(init) {}
  friend class C;
};

///////////////////////////////////////////////////////////////////////
//
// TBoundedProperty template concrete class definition
//
///////////////////////////////////////////////////////////////////////

template <class T, int min, int max, int div>
class TBoundedProperty : public TProperty<T> {
public:
  TBoundedProperty(T init=T()) : TProperty<T>(init) {}

protected:
  virtual void set(const Value& new_val)
	 {
		T temp; new_val.get(temp);
		if (temp >= min/T(div) && temp <= max/T(div))
		  TProperty<T>::itsVal.itsVal = temp;
	 }
  virtual const Value& get() const { return TProperty<T>::itsVal; }
  
  virtual void setNative(T new_val)
	 {
		if (new_val >= min/T(div) && new_val <= max/T(div))
		  TProperty<T>::itsVal.itsVal = new_val;
	 }
  T virtual getNative() const { return TProperty<T>::itsVal.itsVal; }
};

template <class C, class T, int min, int max, int div>
class CTBoundedProperty : public TBoundedProperty<T, min, max, div> { 
public:
  CTBoundedProperty(T init = T()) : TBoundedProperty<T, min, max, div>(init) {}
  friend class C;
};

///////////////////////////////////////////////////////////////////////
//
// TPtrProperty template concrete class definition
//
///////////////////////////////////////////////////////////////////////

template <class T>
class TPtrProperty : public Property, public virtual IO {
public:
  TPtrProperty(T& valRef) : itsVal(valRef) {}

  template <class C> friend class PropFriend;

protected:
  void reseat(T& valRef) { itsVal.reseat(valRef); }

  virtual void serialize(ostream& os, IOFlag) const 
	 { os << itsVal() << ' '; }
  virtual void deserialize(istream& is, IOFlag)
	 { is >> itsVal(); }

  virtual int charCount() const
	 { return gCharCount<T>(itsVal()); }


  virtual void set(const Value& new_val) { new_val.get(itsVal()); }
  virtual const Value& get() const { return itsVal; }

  virtual void setNative(T new_val) { itsVal() = new_val; }
  virtual T getNative() const { return itsVal(); }

  T& operator()() { return itsVal(); }
  const T& operator()() const { return itsVal(); }

private:
  TValuePtr<T> itsVal;
};

template <class C, class T>
class CTPtrProperty : public TPtrProperty<T> {
public:
  CTPtrProperty(T& valRef) : TPtrProperty<T>(valRef) {}
  friend class C;
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
	 { (static_cast<C*>(this)->*prop).setNative(new_val); sendStateChangeMsg(); }
};

///////////////////////////////////////////////////////////////////////
//
// PropertyInfo template class definition
//
///////////////////////////////////////////////////////////////////////

template <class C>
class PropertyInfo {
public:
  typedef Property C::* PropPtr;

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
  
  PropertyInfo& operator=(const PropertyInfo& rhs) {
	 name = rhs.name;
	 property = rhs.property;
	 min = auto_ptr<Value>(rhs.min->clone());
	 max = auto_ptr<Value>(rhs.max->clone());
	 res = auto_ptr<Value>(rhs.res->clone());
	 startNewGroup = rhs.startNewGroup;
  }

  string name;
  PropPtr property;
  auto_ptr<Value> min;			  // suggested minimum value
  auto_ptr<Value> max;			  // suggested maximum value
  auto_ptr<Value> res;			  // suggested resolution
  bool startNewGroup;			  // whether this is the first of a new group
};

static const char vcid_property_h[] = "$Header$";
#endif // !PROPERTY_H_DEFINED
