///////////////////////////////////////////////////////////////////////
//
// fieldpkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov 13 09:58:16 2000
// written: Tue Aug 14 18:48:36 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_CC_DEFINED
#define FIELDPKG_CC_DEFINED

#include "tcl/fieldpkg.h"

#include "io/fields.h"

#include "tcl/tcllistobj.h"
#include "tcl/tclvalue.h"
#include "tcl/tclveccmd.h"

#include "util/ref.h"

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace Tcl
{
  struct FieldGetter
  {
    FieldGetter(const FieldInfo& finfo) : itsFinfo(finfo) {}

    const FieldInfo& itsFinfo;

    void operator()(Tcl::Context& ctx)
    {
      Ref<FieldContainer> item(ctx.getValFromArg(1, TypeCue<Util::UID>()));
      ctx.setResult<const Value&>(*(item->getField(itsFinfo)));
    }
  };

  struct FieldSetter
  {
    FieldSetter(const FieldInfo& finfo) : itsFinfo(finfo) {}

    const FieldInfo& itsFinfo;

    void operator()(Tcl::Context& ctx)
    {
      Ref<FieldContainer> item(ctx.getValFromArg(1, TypeCue<Util::UID>()));
      TclValue val = ctx.getValFromArg(2, TypeCue<TclValue>());
      item->setField(itsFinfo, val);
    }
  };

  ///////////////////////////////////////////////////////////////////////
  //
  // FieldsCmd class definition
  //
  ///////////////////////////////////////////////////////////////////////

  struct FieldsLister
  {
    const FieldMap& itsFields;
    Tcl::List itsFieldList;
    bool isItInited;

    FieldsLister(const FieldMap& fields) :
      itsFields(fields),
      itsFieldList(),
      isItInited(false)
    {}

    void operator()(Tcl::Context& ctx);
  };

} // end namespace Tcl



///////////////////////////////////////////////////////////////////////
//
// FieldsLister member definitions
//
///////////////////////////////////////////////////////////////////////

void Tcl::FieldsLister::operator()(Tcl::Context& ctx)
{
DOTRACE("Tcl::FieldsLister::operator()");
  if (!isItInited)
    {
      for (FieldMap::IoIterator
             itr = itsFields.ioBegin(),
             end = itsFields.ioEnd();
           itr != end;
           ++itr)
        {
          const FieldInfo& finfo = *itr;

          Tcl::List sub_list;

          sub_list.append(finfo.name());           // property name
          sub_list.append<TclValue>(finfo.min());  // min value
          sub_list.append<TclValue>(finfo.max());  // max value
          sub_list.append<TclValue>(finfo.res());  // resolution value
          sub_list.append(finfo.startsNewGroup()); // start new group flag

          itsFieldList.append(sub_list);
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

void Tcl::defField(Tcl::Pkg* pkg, const FieldInfo& finfo)
{
DOTRACE("Tcl::defField");

  pkg->defVecRaw( finfo.name().c_str(), 1, "item_id(s)",
                  FieldGetter(finfo) );
  pkg->defVecRaw( finfo.name().c_str(), 2, "item_id(s) new_val(s)",
                  FieldSetter(finfo) );
}

void Tcl::defAllFields(Tcl::Pkg* pkg, const FieldMap& fmap)
{
DOTRACE("Tcl::defAllFields");
  for (FieldMap::Iterator itr = fmap.begin(), end = fmap.end();
       itr != end;
       ++itr)
    {
      defField(pkg, *itr);
    }

  pkg->defRaw( "fields", 0, "", FieldsLister(fmap) );
}

static const char vcid_fieldpkg_cc[] = "$Header$";
#endif // !FIELDPKG_CC_DEFINED
