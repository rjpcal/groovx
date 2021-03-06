/** @file io/fieldpkg.cc tcl interfaces for manipulating FieldContainer
    fields */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Nov 13 09:58:16 2000
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

#include "tcl-io/fieldpkg.h"

#include "io/fields.h"

#include "nub/ref.h"

#include "tcl/list.h"

#include "tcl-io/objreader.h"
#include "tcl-io/objwriter.h"

#include "rutz/iter.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  struct FieldsLister
  {
    const FieldMap& itsFields;
    tcl::list itsFieldList;
    bool isItRecursive;
    bool isItInited;

    FieldsLister(const FieldMap& fields, bool recurse) :
      itsFields(fields),
      itsFieldList(),
      isItRecursive(recurse),
      isItInited(false)
    {}

    typedef void retn_t;

    void operator()(tcl::call_context& ctx);
  };

  void FieldsLister::operator()(tcl::call_context& ctx)
  {
  GVX_TRACE("tcl::FieldsLister::operator()");
    if (!isItInited)
    {
      for (const FieldMap* fmap = &itsFields;
           fmap != nullptr;
           fmap = isItRecursive ? fmap->parent() : 0)
        {
          for (FieldMap::Iterator itr(fmap->ioFields()); itr.is_valid(); ++itr)
            {
              const Field& field = *itr;

              tcl::list sub_list;

              sub_list.append(field.name());           // property name
              sub_list.append(field.min());            // min value
              sub_list.append(field.max());            // max value
              sub_list.append(field.res());            // resolution value

              tcl::list flags;
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

    ctx.set_result(itsFieldList);
  }
}

///////////////////////////////////////////////////////////////////////
//
// Free function definitions
//
///////////////////////////////////////////////////////////////////////

void tcl::defField(tcl::pkg* pkg, const Field& field,
                   const rutz::file_pos& src_pos)
{
GVX_TRACE("tcl::defField");

  const unsigned int keyarg = 1;

  pkg->def_vec( field.name().c_str(), "objref(s)",
                [&field](nub::ref<FieldContainer> item){
                  tcl::obj_writer w;
                  field.writeValueTo(item.get(), w);
                  return w.get_obj();
                }, keyarg, src_pos );
  pkg->def_vec( field.name().c_str(), "objref(s) new_val(s)",
                [&field](nub::ref<FieldContainer> item, const tcl::obj& newVal){
                  tcl::obj_reader r(newVal);
                  field.readValueFrom(item.get(), r);
                  item->touch(); // emit a signal saying that the object has changed
                }, keyarg, src_pos );
}

void tcl::defAllFields(tcl::pkg* pkg, const FieldMap& fieldmap,
                       const rutz::file_pos& src_pos)
{
GVX_TRACE("tcl::defAllFields");

  for (const FieldMap* fmap = &fieldmap; fmap != nullptr; fmap = fmap->parent())
    {
      for (FieldMap::Iterator itr(fmap->ioFields()); itr.is_valid(); ++itr)
        {
          defField(pkg, *itr, src_pos);
        }
    }

  pkg->def_raw("fields", tcl::arg_spec(1), "",
               FieldsLister(fieldmap, false), src_pos);

  pkg->def_raw("allFields", tcl::arg_spec(1), "",
               FieldsLister(fieldmap, true), src_pos);
}
