///////////////////////////////////////////////////////////////////////
//
// fields.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Nov 11 15:24:47 2000
// written: Mon Nov 13 20:37:12 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDS_CC_DEFINED
#define FIELDS_CC_DEFINED

#include "io/fields.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/pointers.h"

#include <map>

#include "util/trace.h"

FieldMemberPtr::~FieldMemberPtr() {}

Field::Field(FieldContainer* owner) : itsOwner(owner) {}
Field::~Field() {}

void Field::setValue(const Value& new_val) {
  doSetValue(new_val);
  itsOwner->sendStateChangeMsg();
}

template <class T>
TField<T>::TField(FieldContainer* owner, const T& val) :
  Field(owner), itsVal(val) {}

template <class T>
TField<T>::~TField() {}

template <class T>
const Value& TField<T>::value() const { return itsVal; }

template <class T>
void TField<T>::doSetValue(const Value& new_val)
{ new_val.get(itsVal.itsVal); }

template class TField<int>;
template class TField<bool>;
template class TField<double>;

template <class T>
TPtrField<T>::TPtrField(FieldContainer* owner, T& valRef) :
  Field(owner),
  itsVal(valRef)
{}

template <class T>
TPtrField<T>::~TPtrField() {}

template <class T>
void TPtrField<T>::doSetValue(const Value& new_val)
{ new_val.get(itsVal()); }

template <class T>
const Value& TPtrField<T>::value() const
{ return itsVal; }

template class TPtrField<int>;
template class TPtrField<bool>;
template class TPtrField<double>;

///////////////////////////////////////////////////////////////////////
//
// FieldMap
//
///////////////////////////////////////////////////////////////////////

class FieldMap::Impl {
public:
  typedef map<fixed_string, const FieldInfo*> MapType;
  MapType itsNameMap;
  const FieldInfo* itsIoBegin;
  const FieldInfo* itsIoEnd;

  Impl(const FieldInfo* begin, const FieldInfo* end) :
	 itsNameMap(),
	 itsIoBegin(begin),
	 itsIoEnd(end)
	 {
		while (begin != end)
		  {
 			 itsNameMap.insert(MapType::value_type(begin->name(), begin));
			 ++begin;
		  }
	 }
};

FieldMap::FieldMap(const FieldInfo* begin, const FieldInfo* end) :
  itsImpl(new Impl(begin,end))
{}

FieldMap::~FieldMap() { delete itsImpl; }

const FieldInfo& FieldMap::info(const fixed_string& name) const {
  Impl::MapType::const_iterator itr = itsImpl->itsNameMap.find(name);
  if (itr == itsImpl->itsNameMap.end())
	 {
		ErrorWithMsg err("no such field: '");
		err.appendMsg(name.c_str(), "'");
		throw err;
	 }
  return *((*itr).second);
}

class FieldMap::ItrImpl {
public:
  typedef map<fixed_string, const FieldInfo*> MapType;

  ItrImpl(MapType::const_iterator i) : itsItr(i) {}

  MapType::const_iterator itsItr;
};

FieldMap::Iterator::Iterator(const FieldMap::Iterator& other) :
  itsImpl(new ItrImpl(other.itsImpl->itsItr))
{}

FieldMap::Iterator&
FieldMap::Iterator::operator=(const FieldMap::Iterator& other)
{ itsImpl->itsItr = other.itsImpl->itsItr; return *this; }

FieldMap::Iterator::~Iterator() { delete itsImpl; }

const FieldInfo& FieldMap::Iterator::operator*() const
{ return *(operator->()); }

const FieldInfo* FieldMap::Iterator::operator->() const
{ return (*(itsImpl->itsItr)).second; }

FieldMap::Iterator& FieldMap::Iterator::operator++()
{ ++(itsImpl->itsItr); return *this; }

bool FieldMap::Iterator::operator==(const FieldMap::Iterator& other)
{ return (itsImpl->itsItr) == (other.itsImpl->itsItr); }

FieldMap::Iterator FieldMap::begin() const
{ return Iterator(new ItrImpl(itsImpl->itsNameMap.begin())); }

FieldMap::Iterator FieldMap::end() const
{ return Iterator(new ItrImpl(itsImpl->itsNameMap.end())); }

FieldMap::IoIterator FieldMap::ioBegin() const
{ return itsImpl->itsIoBegin; }

FieldMap::IoIterator FieldMap::ioEnd() const
{ return itsImpl->itsIoEnd; }

///////////////////////////////////////////////////////////////////////
//
// FieldContainer
//
///////////////////////////////////////////////////////////////////////

FieldContainer::FieldContainer(const FieldMap& pmap) : itsFieldMap(pmap)
{}

FieldContainer::~FieldContainer() {}

Field& FieldContainer::field(const fixed_string& name)
{ return itsFieldMap.info(name).dereference(this); }

Field& FieldContainer::field(const FieldInfo& pinfo)
{ return pinfo.dereference(this); }

const Field& FieldContainer::field(const fixed_string& name) const
{ return itsFieldMap.info(name).dereference(
                const_cast<FieldContainer*>(this)); }

const Field& FieldContainer::field(const FieldInfo& pinfo) const
{ return pinfo.dereference(const_cast<FieldContainer*>(this)); }

void FieldContainer::readFieldsFrom(IO::Reader* reader) {
DOTRACE("FieldContainer::readFieldsFrom");
  for (FieldMap::IoIterator
			itr = itsFieldMap.ioBegin(),
			end = itsFieldMap.ioEnd();
		 itr != end;
		 ++itr)
	 {
		reader->readValueObj(itr->name(),
									const_cast<Value&>(field(*itr).value()));
	 }

  sendStateChangeMsg();
}

void FieldContainer::writeFieldsTo(IO::Writer* writer) const {
DOTRACE("FieldContainer::writeFieldsTo");
  for (FieldMap::IoIterator
			itr = itsFieldMap.ioBegin(),
			end = itsFieldMap.ioEnd();
		 itr != end;
		 ++itr)
	 {
		writer->writeValueObj(itr->name().c_str(), field(*itr).value());
	 }
}

static const char vcid_fields_cc[] = "$Header$";
#endif // !FIELDS_CC_DEFINED
