///////////////////////////////////////////////////////////////////////
//
// fieldpkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov 13 09:58:16 2000
// written: Mon Jul 16 06:42:04 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_CC_DEFINED
#define FIELDPKG_CC_DEFINED

#include "tcl/fieldpkg.h"

#include "io/fields.h"

#include "tcl/tclvalue.h"
#include "tcl/tclveccmds.h"

#include "util/pointers.h"
#include "util/ref.h"
#include "util/strings.h"

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace Tcl
{

  class FieldContainerFetcher : public Tcl::ItemFetcher {
  public:
    FieldContainerFetcher() {}

    virtual void* getItemFromContext(Tcl::Context& ctx)
    {
      int id = ctx.getIntFromArg(1);
      Ref<FieldContainer> item(id);
      return static_cast<void*>(item.get());
    }
  };

  class FieldAttrib : public Getter<TclValue>, public Setter<TclValue> {
  private:
    const FieldInfo& itsFinfo;

  public:
    FieldAttrib(const FieldInfo& finfo) : itsFinfo(finfo) {}

    virtual TclValue get(void* vitem) const
    {
      FieldContainer* item = static_cast<FieldContainer*>(vitem);
      return *(item->field(itsFinfo).value());
    }

    virtual void set(void* vitem, TclValue val)
    {
      FieldContainer* item = static_cast<FieldContainer*>(vitem);
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

void Tcl::declareField(Tcl::TclItemPkg* pkg, const FieldInfo& finfo) {
DOTRACE("Tcl::declareField");

  static shared_ptr<FieldContainerFetcher>
    fetcher1(new FieldContainerFetcher);

  shared_ptr<FieldAttrib> attrib(new FieldAttrib(finfo));

  pkg->addCommand( new TVecGetterCmd<TclValue>(
                         pkg->interp(),
                         fetcher1.get(),
                         pkg->makePkgCmdName(finfo.name().c_str()),
                         attrib,
                         0 /* for default usage string */) );

  pkg->addCommand( new TVecSetterCmd<TclValue>(
                         pkg->interp(),
                         fetcher1.get(),
                         pkg->makePkgCmdName(finfo.name().c_str()),
                         attrib,
                         0 /* for default usage string */) );
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
