///////////////////////////////////////////////////////////////////////
//
// fieldpkg.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Nov 13 09:58:16 2000
// written: Wed Mar 19 12:45:46 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_CC_DEFINED
#define FIELDPKG_CC_DEFINED

#include "tcl/fieldpkg.h"

#include "io/fields.h"

#include "tcl/tcllistobj.h"

#include "util/iter.h"
#include "util/ref.h"

#include "util/trace.h"
#include "util/debug.h"

namespace Tcl
{
  Tcl::ObjPtr getField(const Field& field, Ref<FieldContainer> item)
  {
    return item->getField(field);
  }

  void setField(const Field& field, Ref<FieldContainer> item,
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

    typedef void Retn_t;

    void operator()(Tcl::Context& ctx);
  };
}


void Tcl::FieldsLister::operator()(Tcl::Context& ctx)
{
DOTRACE("Tcl::FieldsLister::operator()");
  if (!isItInited)
    {
      for (const FieldMap* fmap = &itsFields;
           fmap != 0;
           fmap = isItRecursive ? fmap->parent() : 0)
        {
          for (FieldMap::Iterator itr(fmap->ioFields()); itr.isValid(); ++itr)
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

void Tcl::defField(Tcl::Pkg* pkg, const Field& field)
{
DOTRACE("Tcl::defField");

  pkg->defVec( field.name().c_str(), "item_id(s)",
               Util::bindFirst(getField, field) );
  pkg->defVec( field.name().c_str(), "item_id(s) new_val(s)",
               Util::bindFirst(setField, field) );
}

void Tcl::defAllFields(Tcl::Pkg* pkg, const FieldMap& fieldmap)
{
DOTRACE("Tcl::defAllFields");

  for (const FieldMap* fmap = &fieldmap; fmap != 0; fmap = fmap->parent())
    {
      for (FieldMap::Iterator itr(fmap->ioFields()); itr.isValid(); ++itr)
        {
          defField(pkg, *itr);
        }
    }

  pkg->defRaw( "fields", 0, "", FieldsLister(fieldmap, false) );

  pkg->defRaw( "allFields", 0, "", FieldsLister(fieldmap, true) );
}

static const char vcid_fieldpkg_cc[] = "$Header$";
#endif // !FIELDPKG_CC_DEFINED
