///////////////////////////////////////////////////////////////////////
//
// fields.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Nov 11 15:24:47 2000
// written: Mon Jun 11 12:12:25 2001
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
#define LOCAL_ASSERT
#include "util/debug.h"

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
void TField<T>::readValueFrom(IO::Reader* reader, const fixed_string& name)
{ reader->readValueObj(name, itsVal); }

template <class T>
void TField<T>::writeValueTo(IO::Writer* writer, const fixed_string& name) const
{ writer->writeValueObj(name.c_str(), itsVal); }

template <class T>
shared_ptr<Value> TField<T>::value() const
{ return shared_ptr<Value>(itsVal.clone()); }

template <class T>
void TField<T>::doSetValue(const Value& new_val)
{ new_val.get(itsVal.itsVal); }

template class TField<int>;
template class TField<bool>;
template class TField<double>;

template <class T>
TBoundedField<T>::TBoundedField(FieldContainer* owner, const T& val,
										  const T& min, const T& max) :
	 Field(owner),
	 itsVal(val),
	 itsMin(min),
	 itsMax(max)
{}

template <class T>
TBoundedField<T>::~TBoundedField() {}

template <class T>
void TBoundedField<T>::readValueFrom(IO::Reader* reader,
												 const fixed_string& name)
{ reader->readValueObj(name, itsVal); }

template <class T>
void TBoundedField<T>::writeValueTo(IO::Writer* writer,
												const fixed_string& name) const
{ writer->writeValueObj(name.c_str(), itsVal); }

template <class T>
shared_ptr<Value> TBoundedField<T>::value() const
{ return shared_ptr<Value>(itsVal.clone()); }

template <class T>
void TBoundedField<T>::doSetValue(const Value& new_val) {
  T temp; new_val.get(temp);
  if (temp >= itsMin && temp <= itsMax)
	 itsVal.itsVal = temp;
}

template class TBoundedField<int>;
template class TBoundedField<bool>;
template class TBoundedField<double>;

template <class T>
TPtrField<T>::TPtrField(FieldContainer* owner, T& valRef) :
  Field(owner),
  itsVal(valRef)
{}

template <class T>
TPtrField<T>::~TPtrField() {}

template <class T>
void TPtrField<T>::readValueFrom(IO::Reader* reader, const fixed_string& name)
{ reader->readValueObj(name, itsVal); }

template <class T>
void TPtrField<T>::writeValueTo(IO::Writer* writer, const fixed_string& name) const
{ writer->writeValueObj(name.c_str(), itsVal); }

template <class T>
void TPtrField<T>::doSetValue(const Value& new_val)
{ new_val.get(itsVal()); }

template <class T>
shared_ptr<Value> TPtrField<T>::value() const
{ return shared_ptr<Value>(itsVal.clone()); }

template class TPtrField<int>;
template class TPtrField<bool>;
template class TPtrField<double>;

///////////////////////////////////////////////////////////////////////
//
// FieldMap
//
///////////////////////////////////////////////////////////////////////

class FieldMap::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  typedef std::map<fixed_string, const FieldInfo*> MapType;
  MapType itsNameMap;
  const FieldInfo* const itsIoBegin;
  const FieldInfo* const itsIoEnd;

  const FieldMap* itsParent;

  Impl(const FieldInfo* begin, const FieldInfo* end,
		 const FieldMap* parent) :
	 itsNameMap(),
	 itsIoBegin(begin),
	 itsIoEnd(end),
	 itsParent(parent)
	 {
		while (begin != end)
		  {
 			 itsNameMap.insert(MapType::value_type(begin->name(), begin));
			 ++begin;
		  }
	 }
};

FieldMap::FieldMap(const FieldInfo* begin, const FieldInfo* end,
						 const FieldMap* parent) :
  itsImpl(new Impl(begin,end,parent))
{}

FieldMap::~FieldMap() { delete itsImpl; }

const FieldMap* FieldMap::emptyFieldMap() {
  static const FieldMap* emptyMap = 0;
  if (emptyMap == 0)
	 emptyMap = new FieldMap((FieldInfo*)0, (FieldInfo*)0, (FieldMap*)0);
  return emptyMap;
}

bool FieldMap::hasParent() const
{ return (itsImpl->itsParent != 0); }

const FieldMap* FieldMap::parent() const
{ return itsImpl->itsParent; }

const FieldInfo& FieldMap::info(const fixed_string& name) const {
  Impl::MapType::const_iterator itr = itsImpl->itsNameMap.find(name);

  if (itr != itsImpl->itsNameMap.end())
	 return *((*itr).second);

  if (hasParent())
	 {
		return parent()->info(name);
	 }
  else
	 {
		ErrorWithMsg err("no such field: '");
		err.appendMsg(name.c_str(), "'");
		throw err;
	 }
}

class FieldMap::ItrImpl {
public:
  typedef std::map<fixed_string, const FieldInfo*> MapType;

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

FieldContainer::FieldContainer() : itsFieldMap(FieldMap::emptyFieldMap())
{}

FieldContainer::~FieldContainer() {}

void FieldContainer::setFieldMap(const FieldMap& fields)
{ itsFieldMap = &fields; }

Field& FieldContainer::field(const fixed_string& name)
{ return itsFieldMap->info(name).dereference(this); }

Field& FieldContainer::field(const FieldInfo& pinfo)
{ return pinfo.dereference(this); }

const Field& FieldContainer::field(const fixed_string& name) const
{ return itsFieldMap->info(name).dereference(
                const_cast<FieldContainer*>(this)); }

const Field& FieldContainer::field(const FieldInfo& pinfo) const
{ return pinfo.dereference(const_cast<FieldContainer*>(this)); }

void FieldContainer::readFieldsFrom(IO::Reader* reader,
												const FieldMap& fields) {
DOTRACE("FieldContainer::readFieldsFrom");
  for (FieldMap::IoIterator
			itr = fields.ioBegin(),
			end = fields.ioEnd();
		 itr != end;
		 ++itr)
	 {
		field(*itr).readValueFrom(reader, itr->name());
	 }

  sendStateChangeMsg();
}

void FieldContainer::writeFieldsTo(IO::Writer* writer,
											  const FieldMap& fields) const {
DOTRACE("FieldContainer::writeFieldsTo");
  for (FieldMap::IoIterator
			itr = fields.ioBegin(),
			end = fields.ioEnd();
		 itr != end;
		 ++itr)
	 {
		field(*itr).writeValueTo(writer, itr->name());
	 }
}

static const char vcid_fields_cc[] = "$Header$";
#endif // !FIELDS_CC_DEFINED
