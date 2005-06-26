///////////////////////////////////////////////////////////////////////
//
// fieldpkg.cc
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Nov 13 09:58:16 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef GROOVX_IO_FIELDPKG_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_FIELDPKG_CC_UTC20050626084021_DEFINED

#include "io/fieldpkg.h"

#include "io/fields.h"

#include "nub/ref.h"

#include "tcl/tcllistobj.h"

#include "rutz/iter.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace Tcl
{
  Tcl::ObjPtr getField(const Field& field,
                       Nub::Ref<FieldContainer> item)
  {
    return item->getField(field);
  }

  void setField(const Field& field, Nub::Ref<FieldContainer> item,
                const Tcl::ObjPtr& newValue)
  {
    return item->setField(field, newValue);
  }

  struct FieldsLister
  {
    const FieldMap& itsFields;
    Tcl::List itsFieldList;
    bool isItRecursive;
    bool isItInited;

    FieldsLister(const FieldMap& fields, bool recurse) :
      itsFields(fields),
      itsFieldList(),
      isItRecursive(recurse),
      isItInited(false)
    {}

    ~FieldsLister() throw() {}

    typedef void retn_t;

    void operator()(Tcl::Context& ctx);
  };
}


void Tcl::FieldsLister::operator()(Tcl::Context& ctx)
{
GVX_TRACE("Tcl::FieldsLister::operator()");
  if (!isItInited)
    {
      for (const FieldMap* fmap = &itsFields;
           fmap != 0;
           fmap = isItRecursive ? fmap->parent() : 0)
        {
          for (FieldMap::Iterator itr(fmap->ioFields()); itr.is_valid(); ++itr)
            {
              const Field& field = *itr;

              Tcl::List sub_list;

              sub_list.append(field.name());           // property name
              sub_list.append(field.min());            // min value
              sub_list.append(field.max());            // max value
              sub_list.append(field.res());            // resolution value

              Tcl::List flags;
              if (field.startsNewGroup()) flags.append("NEW_GROUP");
              if (field.isTransient())    flags.append("TRANSIENT");
              if (field.isString())       flags.append("STRING");
              if (field.isMultiValued())  flags.append("MULTI");
              if (field.isChecked())      flags.append("CHECKED");
              if (!field.allowGet())      flags.append("NO_GET");
              if (!field.allowSet())      flags.append("NO_SET");
              if (field.isPrivate())      flags.append("PRIVATE");
              if (field.isBoolean())      flags.append("BOOLEAN");

              sub_list.append(flags);

              itsFieldList.append(sub_list);
            }
        }

      isItInited = true;
    }

  ctx.setResult(itsFieldList);
}

///////////////////////////////////////////////////////////////////////
//
// Free function definitions
//
///////////////////////////////////////////////////////////////////////

void Tcl::defField(Tcl::Pkg* pkg, const Field& field,
                   const rutz::file_pos& src_pos)
{
GVX_TRACE("Tcl::defField");

  const unsigned int keyarg = 1;

  pkg->defVec( field.name().c_str(), "objref(s)",
               rutz::bind_first(getField, field), keyarg, src_pos );
  pkg->defVec( field.name().c_str(), "objref(s) new_val(s)",
               rutz::bind_first(setField, field), keyarg, src_pos );
}

void Tcl::defAllFields(Tcl::Pkg* pkg, const FieldMap& fieldmap,
                       const rutz::file_pos& src_pos)
{
GVX_TRACE("Tcl::defAllFields");

  for (const FieldMap* fmap = &fieldmap; fmap != 0; fmap = fmap->parent())
    {
      for (FieldMap::Iterator itr(fmap->ioFields()); itr.is_valid(); ++itr)
        {
          defField(pkg, *itr, src_pos);
        }
    }

  pkg->defRaw("fields", 0, "",
              FieldsLister(fieldmap, false), src_pos);

  pkg->defRaw("allFields", 0, "",
              FieldsLister(fieldmap, true), src_pos);
}

static const char vcid_groovx_io_fieldpkg_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_FIELDPKG_CC_UTC20050626084021_DEFINED
