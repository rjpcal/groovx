///////////////////////////////////////////////////////////////////////
//
// property.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep 29 11:57:34 1999
// written: Fri Mar  3 18:05:12 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_CC_DEFINED
#define PROPERTY_CC_DEFINED

#include "property.h"

#include <memory>
#include <string>

Property::~Property() {}

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
  string name;
  /// Suggested minimum value
  auto_ptr<Value> min;
  /// Suggested maximum value
  auto_ptr<Value> max;
  /// Suggested resolution
  auto_ptr<Value> res;
  /// True if this is the first in a new group of properties
  bool startNewGroup;
};

PropertyInfoBase::PropertyInfoBase(
  const char* name, Value* min, Value* max, Value* res, bool new_group) :
  itsImpl(new Impl(name, min, max, res, new_group))
{}

PropertyInfoBase::PropertyInfoBase(const PropertyInfoBase& other) :
  itsImpl(new Impl(*(other.itsImpl)))
{}

PropertyInfoBase::~PropertyInfoBase() 
{
  delete itsImpl;
}

PropertyInfoBase& PropertyInfoBase::operator=(const PropertyInfoBase& other)
{
  *itsImpl = *(other.itsImpl);
}

const string& PropertyInfoBase::name() const {
  return itsImpl->name;
}

const char* PropertyInfoBase::name_cstr() const {
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
