///////////////////////////////////////////////////////////////////////
//
// property.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 11:57:34 1999
// written: Tue Oct 31 22:51:42 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_CC_DEFINED
#define PROPERTY_CC_DEFINED

#include "io/property.h"

#include "util/pointers.h"
#include "util/strings.h"

#include "util/trace.h"

Property::~Property() {}

template <class T>
TProperty<T>::TProperty(const T& init) : itsVal(init) {}

template <class T>
TProperty<T>::~TProperty() {}

template class TProperty<int>;
template class TProperty<bool>;
template class TProperty<double>;


template <class T>
TPtrProperty<T>::TPtrProperty(T& valRef) : itsVal(valRef) {}

template <class T>
TPtrProperty<T>::~TPtrProperty() {}

template class TPtrProperty<int>;
template class TPtrProperty<bool>;
template class TPtrProperty<double>;


class PropertyInfoBase::Impl {
public:
  Impl(
    const char* name_, Value* min_, Value* max_, Value* res_, bool new_group_) :
	 name(name_),
	 min(min_),
	 max(max_),
	 res(res_),
	 startNewGroup(new_group_)
	 {}

  Impl(const Impl& other) :
	 name(other.name),
	 min(other.min->clone()),
	 max(other.max->clone()),
	 res(other.res->clone()),
	 startNewGroup(other.startNewGroup)
	 {}

  Impl& operator=(const Impl& other)
	 {
		name = other.name;
		min.reset(other.min->clone());
		max.reset(other.max->clone());
		res.reset(other.res->clone());
		startNewGroup = other.startNewGroup;
		return *this;
	 }

  /// User-friendly name of the property
  fixed_string name;
  /// Suggested minimum value
  scoped_ptr<Value> min;
  /// Suggested maximum value
  scoped_ptr<Value> max;
  /// Suggested resolution
  scoped_ptr<Value> res;
  /// True if this is the first in a new group of properties
  bool startNewGroup;
};

template <class T>
PropertyInfoBase::PropertyInfoBase(
  const char* name, T min, T max, T res, bool new_group) :
  itsImpl(new Impl(name, new TValue<T>(min), new TValue<T>(max),
						 new TValue<T>(res), new_group))
{}

namespace {
  static void dummy() {
	 PropertyInfoBase int_("", int(0), int(0), int(0), false);
	 PropertyInfoBase bool_("", bool(0), bool(0), bool(0), false);
	 PropertyInfoBase double_("", double(0), double(0), double(0), false);
  }
}

PropertyInfoBase::PropertyInfoBase(const PropertyInfoBase& other) :
  itsImpl(new Impl(*(other.itsImpl)))
{
DOTRACE("PropertyInfoBase::PropertyInfoBase");
}

PropertyInfoBase::~PropertyInfoBase() 
{
DOTRACE("PropertyInfoBase::~PropertyInfoBase");
  delete itsImpl;
}

PropertyInfoBase& PropertyInfoBase::operator=(const PropertyInfoBase& other)
{
DOTRACE("PropertyInfoBase::operator=");
  *itsImpl = *(other.itsImpl);
  return *this;
}

const char* PropertyInfoBase::name_cstr() const {
DOTRACE("PropertyInfoBase::name_cstr");
  return itsImpl->name.c_str();
}

const Value& PropertyInfoBase::min() const {
  return *(itsImpl->min);
}

const Value& PropertyInfoBase::max() const {
  return *(itsImpl->max);
}

const Value& PropertyInfoBase::res() const {
  return *(itsImpl->res);
}

bool PropertyInfoBase::startNewGroup() const {
  return itsImpl->startNewGroup;
}

static const char vcid_property_cc[] = "$Header$";
#endif // !PROPERTY_CC_DEFINED
