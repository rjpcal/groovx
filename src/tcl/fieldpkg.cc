///////////////////////////////////////////////////////////////////////
//
// fieldpkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov 13 09:58:16 2000
// written: Thu Jul 12 17:36:55 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_CC_DEFINED
#define FIELDPKG_CC_DEFINED

#include "tcl/fieldpkg.h"

#include "io/fields.h"

#include "tcl/tclvalue.h"
#include "tcl/tclveccmds.h"

#include "util/ref.h"
#include "util/strings.h"

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace Tcl
{

  class FieldContainerFetcher : public Tcl::ItemFetcher {
  public:
    FieldContainerFetcher(int item_argn) : itsItemArgn(item_argn) {}

    virtual void* getItemFromContext(Tcl::Context& ctx)
    {
      int id = ctx.getIntFromArg(itsItemArgn);
      Ref<FieldContainer> item(id);
      return static_cast<void*>(item.get());
    }

  private:
    int itsItemArgn;
  };

  class FieldAttrib : public Getter<TclValue>, public Setter<TclValue> {
  private:
    const FieldInfo& itsFinfo;

  public:
    FieldAttrib(const FieldInfo& finfo) : itsFinfo(finfo) {}

    const char* name() const { return itsFinfo.name().c_str(); }

    static shared_ptr<FieldAttrib> make(const FieldInfo& finfo)
    {
      return make_shared(new FieldAttrib(finfo));
    }

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
// FieldVecCmd class definition
//
///////////////////////////////////////////////////////////////////////

  class FieldVecCmd : public TVecAttribCmd<TclValue> {
  private:
    static FieldContainerFetcher* getFetcher0()
    {
      static shared_ptr<FieldContainerFetcher> theFetcher0;
      if (theFetcher0.get() == 0)
        {
          theFetcher0.reset(new FieldContainerFetcher(0));
        }
      return theFetcher0.get();
    }

    static FieldContainerFetcher* getFetcher1()
    {
      static shared_ptr<FieldContainerFetcher> theFetcher1;
      if (theFetcher1.get() == 0)
        {
          theFetcher1.reset(new FieldContainerFetcher(1));
        }
      return theFetcher1.get();
    }

    static FieldContainerFetcher* getFetcher(int item_argn)
    {
      return (item_argn==0) ? getFetcher0() : getFetcher1();
    }

  public:
    FieldVecCmd(TclItemPkg* pkg, shared_ptr<FieldAttrib> attrib) :
      TVecAttribCmd<TclValue>(pkg->interp(),
                              getFetcher(pkg->itemArgn()),
                              pkg->makePkgCmdName(attrib->name()),
                              attrib,
                              attrib,
                              0, /* for default usage string */
                              pkg->itemArgn())
    {}
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
  pkg->addCommand( new FieldVecCmd(pkg, FieldAttrib::make(finfo)) );
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
