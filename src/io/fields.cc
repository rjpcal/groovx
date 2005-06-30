///////////////////////////////////////////////////////////////////////
//
// fields.cc
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Nov 11 15:24:47 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_IO_FIELDS_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_FIELDS_CC_UTC20050626084021_DEFINED

#include "io/fields.h"

#include "io/reader.h"
#include "io/writer.h"

#include "nub/signal.h"

#include "rutz/error.h"
#include "rutz/iter.h"

#include <map>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

void FieldAux::throwNotAllowed(const char* what,
                               const rutz::file_pos& pos)
{
  throw rutz::error(fstring("'", what, "' operation "
                            "not allowed for that field"), pos);
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
  MapType nameMap;
  const Field* const ioBegin;
  const Field* const ioEnd;

  const FieldMap* parent;

  Impl(const Field* begin, const Field* end,
       const FieldMap* par) :
    nameMap(),
    ioBegin(begin),
    ioEnd(end),
    parent(par)
  {
    while (begin != end)
      {
        nameMap.insert(MapType::value_type(begin->name(), begin));
        ++begin;
      }
  }
};

void FieldMap::init(const Field* begin, const Field* end,
                    const FieldMap* parent)
{
  GVX_ASSERT(rep == 0);
  rep = new Impl(begin, end, parent);
}

FieldMap::~FieldMap()
{
  delete rep;
}

const FieldMap* FieldMap::emptyFieldMap()
{
  static const FieldMap* emptyMap = 0;
  if (emptyMap == 0)
    emptyMap = new FieldMap(static_cast<Field*>(0),
                            static_cast<Field*>(0),
                            static_cast<FieldMap*>(0));
  return emptyMap;
}

bool FieldMap::hasParent() const throw()
{
  return (rep->parent != 0);
}

const FieldMap* FieldMap::parent() const throw()
{
  return rep->parent;
}

const Field& FieldMap::field(const fstring& name) const
{
  Impl::MapType::const_iterator itr = rep->nameMap.find(name);

  if (itr != rep->nameMap.end())
    return *((*itr).second);

  if (hasParent())
    {
      return parent()->field(name);
    }
  else
    {
      throw rutz::error(fstring("no such field: '", name.c_str(), "'"),
                        SRC_POS);
    }
}

FieldMap::Iterator FieldMap::ioFields() const
{
  return Iterator(rep->ioBegin, rep->ioEnd);
}



///////////////////////////////////////////////////////////////////////
//
// FieldContainer
//
///////////////////////////////////////////////////////////////////////

FieldContainer::FieldContainer(nub::signal0* sig) :
  itsFieldMap(FieldMap::emptyFieldMap()),
  itsSignal(sig)
{}

FieldContainer::~FieldContainer() throw() {}

void FieldContainer::setFieldMap(const FieldMap& fields)
{
  itsFieldMap = &fields;
}

Tcl::Obj FieldContainer::getField(const fstring& name) const
{
  return getField(itsFieldMap->field(name));
}

Tcl::Obj FieldContainer::getField(const Field& field) const
{
  return field.getValue(this);
}

void FieldContainer::setField(const fstring& name, const Tcl::Obj& new_val)
{
  setField(itsFieldMap->field(name), new_val);
}

void FieldContainer::setField(const Field& field, const Tcl::Obj& new_val)
{
  field.setValue(this, new_val);
  if (itsSignal)
    itsSignal->emit();
}

void FieldContainer::readFieldsFrom(IO::Reader& reader,
                                    const FieldMap& fields)
{
GVX_TRACE("FieldContainer::readFieldsFrom");

  const IO::VersionId svid = reader.readSerialVersionId();

  for (FieldMap::Iterator itr(fields.ioFields()); itr.is_valid(); ++itr)
    {
      if (!itr->isTransient() && itr->shouldSerialize(svid))
        itr->readValueFrom(this, reader, itr->name());
    }

  if (itsSignal)
    itsSignal->emit();
}

void FieldContainer::writeFieldsTo(IO::Writer& writer,
                                   const FieldMap& fields,
                                   IO::VersionId svid) const
{
GVX_TRACE("FieldContainer::writeFieldsTo");

  for (FieldMap::Iterator itr(fields.ioFields()); itr.is_valid(); ++itr)
    {
      if (!itr->isTransient() && itr->shouldSerialize(svid))
        itr->writeValueTo(this, writer, itr->name());
    }
}

FieldContainer* FieldContainer::child() const
{
GVX_TRACE("FieldContainer::child");
  return 0;
}

static const char vcid_groovx_io_fields_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_FIELDS_CC_UTC20050626084021_DEFINED
