///////////////////////////////////////////////////////////////////////
//
// fieldpkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov 13 09:58:16 2000
// written: Mon Jul 16 09:23:45 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_CC_DEFINED
#define FIELDPKG_CC_DEFINED

#include "tcl/fieldpkg.h"

#include "io/fields.h"

#include "tcl/tclvalue.h"
#include "tcl/tclveccmd.h"

#include "util/ref.h"

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace Tcl
{
  class FieldGetterCmd : public Tcl::VecCmd {
  private:
    const FieldInfo& itsFinfo;
  public:
    FieldGetterCmd(Tcl::TclItemPkg* pkg, const FieldInfo& finfo) :
      VecCmd(pkg->interp(), pkg->makePkgCmdName(finfo.name().c_str()),
             "item_id(s)", 1, 2),
      itsFinfo(finfo)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      Ref<FieldContainer> item(ctx.getValFromArg(1, TypeCue<Util::UID>()));
      ctx.setResult<const Value&>(*(item->field(itsFinfo).value()));
    }
  };

  class FieldSetterCmd : public Tcl::VecCmd {
  private:
    const FieldInfo& itsFinfo;
  public:
    FieldSetterCmd(Tcl::TclItemPkg* pkg, const FieldInfo& finfo) :
      VecCmd(pkg->interp(), pkg->makePkgCmdName(finfo.name().c_str()),
             "item_id(s) new_val(s)", 1, 3),
      itsFinfo(finfo)
    {}

  protected:
    virtual void invoke(Tcl::Context& ctx)
    {
      Ref<FieldContainer> item(ctx.getValFromArg(1, TypeCue<Util::UID>()));
      TclValue val = ctx.getValFromArg(2, TypeCue<TclValue>());
      item->field(itsFinfo).setValue(val);
    }
  };

///////////////////////////////////////////////////////////////////////
//
// FieldsCmd class definition
//
///////////////////////////////////////////////////////////////////////

  class FieldsCmd : public TclCmd {
  private:
    const FieldMap& itsFields;
    Tcl::List itsFieldList;
    bool isItInited;

    FieldsCmd(const FieldsCmd&);
    FieldsCmd& operator=(const FieldsCmd&);

  public:
    FieldsCmd(TclPkg* pkg, const FieldMap& fields);
    virtual ~FieldsCmd();

  protected:
    virtual void invoke(Tcl::Context& ctx);
  };

} // end namespace Tcl



///////////////////////////////////////////////////////////////////////
//
// FieldsCmd member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::FieldsCmd::FieldsCmd(TclPkg* pkg, const FieldMap& fields) :
  TclCmd(pkg->interp(), pkg->makePkgCmdName("fields"), NULL, 1, 1),
  itsFields(fields),
  itsFieldList(),
  isItInited(false)
{}

Tcl::FieldsCmd::~FieldsCmd() {}

void Tcl::FieldsCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::FieldsCmd::invoke");
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

void Tcl::declareField(Tcl::TclItemPkg* pkg, const FieldInfo& finfo)
{
DOTRACE("Tcl::declareField");

  pkg->addCommand( new FieldGetterCmd(pkg, finfo) );
  pkg->addCommand( new FieldSetterCmd(pkg, finfo) );
}

void Tcl::declareAllFields(Tcl::TclItemPkg* pkg, const FieldMap& fmap){
DOTRACE("Tcl::declareAllFields");
  for (FieldMap::Iterator itr = fmap.begin(), end = fmap.end();
       itr != end;
       ++itr)
    {
      declareField(pkg, *itr);
    }

  pkg->addCommand( new FieldsCmd(pkg, fmap) );
}


static const char vcid_fieldpkg_cc[] = "$Header$";
#endif // !FIELDPKG_CC_DEFINED
