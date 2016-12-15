/** @file io/fields.cc FieldContainer allows serializable objects to
    have "fields", or named object properties */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Sat Nov 11 15:24:47 2000
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "io/fields.h"

#include "io/reader.h"
#include "io/writer.h"

#include "nub/signal.h"

#include "rutz/error.h"
#include "rutz/iter.h"
#include "rutz/sfmt.h"

#include <map>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

void FieldAux::throwNotAllowed(const char* what,
                               const rutz::file_pos& pos)
{
  throw rutz::error(rutz::sfmt("'%s' operation not allowed "
                               "for that field", what), pos);
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
  GVX_ASSERT(rep == nullptr);
  rep = new Impl(begin, end, parent);
}

FieldMap::~FieldMap()
{
  delete rep;
}

const FieldMap* FieldMap::emptyFieldMap()
{
  static const FieldMap* emptyMap = nullptr;
  if (emptyMap == nullptr)
    emptyMap = new FieldMap(static_cast<Field*>(0),
                            static_cast<Field*>(0),
                            static_cast<FieldMap*>(0));
  return emptyMap;
}

bool FieldMap::hasParent() const noexcept
{
  return (rep->parent != nullptr);
}

const FieldMap* FieldMap::parent() const noexcept
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
      throw rutz::error(rutz::sfmt("no such field: '%s'", name.c_str()),
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

FieldContainer::FieldContainer(nub::signal<>* sig) :
  itsFieldMap(FieldMap::emptyFieldMap()),
  itsSignal(sig)
{}

FieldContainer::~FieldContainer() noexcept {}

void FieldContainer::setFieldMap(const FieldMap& fields)
{
  itsFieldMap = &fields;
}

const Field& FieldContainer::field(const rutz::fstring& name) const
{
  return itsFieldMap->field(name);
}

void FieldContainer::touch() const
{
  if (itsSignal)
    itsSignal->emit();
}

void FieldContainer::readFieldsFrom(io::reader& reader,
                                    const FieldMap& fields)
{
GVX_TRACE("FieldContainer::readFieldsFrom");

  const io::version_id svid = reader.input_version_id();

  for (FieldMap::Iterator itr(fields.ioFields()); itr.is_valid(); ++itr)
    {
      if (!itr->isTransient() && itr->shouldSerialize(svid))
        itr->readValueFrom(this, reader);
    }

  if (itsSignal)
    itsSignal->emit();
}

void FieldContainer::writeFieldsTo(io::writer& writer,
                                   const FieldMap& fields,
                                   io::version_id svid) const
{
GVX_TRACE("FieldContainer::writeFieldsTo");

  for (FieldMap::Iterator itr(fields.ioFields()); itr.is_valid(); ++itr)
    {
      if (!itr->isTransient() && itr->shouldSerialize(svid))
        itr->writeValueTo(this, writer);
    }
}

FieldContainer* FieldContainer::child() const
{
GVX_TRACE("FieldContainer::child");
  return 0;
}
