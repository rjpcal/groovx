///////////////////////////////////////////////////////////////////////
//
// fields.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Nov 11 15:24:47 2000
// written: Wed Nov 13 12:10:50 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDS_CC_DEFINED
#define FIELDS_CC_DEFINED

#include "io/fields.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/iter.h"
#include "util/signal.h"

#include <map>

#include "util/trace.h"
#include "util/debug.h"

void FieldAux::throwNotAllowed(const char* what)
{
  throw Util::Error(fstring("'", what, "' operation "
                            "not allowed for that field"));
}

void FieldAux::throwBadCast()
{
  throw Util::Error("couldn't cast FieldContainer to target type");
}

FieldImpl::~FieldImpl() {}

///////////////////////////////////////////////////////////////////////
//
// FieldMap
//
///////////////////////////////////////////////////////////////////////

class FieldMap::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  typedef std::map<fstring, const Field*> MapType;
  MapType itsNameMap;
  const Field* const itsIoBegin;
  const Field* const itsIoEnd;

  const FieldMap* itsParent;

  Impl(const Field* begin, const Field* end,
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

void FieldMap::init(const Field* begin, const Field* end,
                    const FieldMap* parent)
{
  Assert(itsImpl == 0);
  itsImpl = new Impl(begin, end, parent);
}

FieldMap::~FieldMap()
{
  delete itsImpl;
}

const FieldMap* FieldMap::emptyFieldMap()
{
  static const FieldMap* emptyMap = 0;
  if (emptyMap == 0)
    emptyMap = new FieldMap((Field*)0, (Field*)0, (FieldMap*)0);
  return emptyMap;
}

bool FieldMap::hasParent() const
{
  return (itsImpl->itsParent != 0);
}

const FieldMap* FieldMap::parent() const
{
  return itsImpl->itsParent;
}

const Field& FieldMap::field(const fstring& name) const
{
  Impl::MapType::const_iterator itr = itsImpl->itsNameMap.find(name);

  if (itr != itsImpl->itsNameMap.end())
    return *((*itr).second);

  if (hasParent())
    {
      return parent()->field(name);
    }
  else
    {
      throw Util::Error(fstring("no such field: '", name.c_str(), "'"));
    }
}

FieldMap::Iterator FieldMap::ioFields() const
{
  return Iterator(itsImpl->itsIoBegin, itsImpl->itsIoEnd);
}



///////////////////////////////////////////////////////////////////////
//
// FieldContainer
//
///////////////////////////////////////////////////////////////////////

FieldContainer::FieldContainer(Util::Signal* sig) :
  itsFieldMap(FieldMap::emptyFieldMap()),
  itsSignal(sig)
{}

FieldContainer::~FieldContainer() {}

void FieldContainer::setFieldMap(const FieldMap& fields)
{
  itsFieldMap = &fields;
}

Tcl::ObjPtr FieldContainer::getField(const fstring& name) const
{
  return getField(itsFieldMap->field(name));
}

Tcl::ObjPtr FieldContainer::getField(const Field& field) const
{
  return field.getValue(this);
}

void FieldContainer::setField(const fstring& name, const Tcl::ObjPtr& new_val)
{
  setField(itsFieldMap->field(name), new_val);
}

void FieldContainer::setField(const Field& field, const Tcl::ObjPtr& new_val)
{
  field.setValue(this, new_val);
  if (itsSignal)
    itsSignal->emit();
}

void FieldContainer::readFieldsFrom(IO::Reader* reader,
                                    const FieldMap& fields)
{
DOTRACE("FieldContainer::readFieldsFrom");

  for (FieldMap::Iterator itr(fields.ioFields()); itr.isValid(); ++itr)
    {
      if (!itr->isTransient())
        itr->readValueFrom(this, reader, itr->name());
    }

  if (itsSignal)
    itsSignal->emit();
}

void FieldContainer::writeFieldsTo(IO::Writer* writer,
                                   const FieldMap& fields) const
{
DOTRACE("FieldContainer::writeFieldsTo");

  for (FieldMap::Iterator itr(fields.ioFields()); itr.isValid(); ++itr)
    {
      if (!itr->isTransient())
        itr->writeValueTo(this, writer, itr->name());
    }
}

FieldContainer* FieldContainer::child() const
{
DOTRACE("FieldContainer::child");
  return 0;
}

static const char vcid_fields_cc[] = "$Header$";
#endif // !FIELDS_CC_DEFINED
