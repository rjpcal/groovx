///////////////////////////////////////////////////////////////////////
//
// fieldpkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov 13 09:58:16 2000
// written: Wed Jul 11 10:48:06 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_CC_DEFINED
#define FIELDPKG_CC_DEFINED

#include "tcl/fieldpkg.h"

#include "io/fields.h"

#include "tcl/tclvalue.h"

#include "util/arrays.h"
#include "util/minivec.h"
#include "util/ref.h"
#include "util/strings.h"

#include <tcl.h>

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace Tcl {

///////////////////////////////////////////////////////////////////////
//
// FieldVecCmd class definition
//
///////////////////////////////////////////////////////////////////////

class FieldVecCmd : public TclCmd {
private:
  TclItemPkg* itsPkg;
  const FieldInfo& itsFinfo;
  int itsItemArgn;
  int itsValArgn;
  int itsObjcGet;
  int itsObjcSet;

  FieldVecCmd(const FieldVecCmd&);
  FieldVecCmd& operator=(const FieldVecCmd&);

public:
  FieldVecCmd(TclItemPkg* pkg, const FieldInfo& finfo);
  virtual ~FieldVecCmd();

protected:
  virtual void invoke();
};

///////////////////////////////////////////////////////////////////////
//
// FieldsCmd class definition
//
///////////////////////////////////////////////////////////////////////

class FieldsCmd : public TclCmd {
private:
  Tcl_Interp* itsInterp;
  const FieldMap& itsFields;
  Tcl_Obj* itsFieldList;

  FieldsCmd(const FieldsCmd&);
  FieldsCmd& operator=(const FieldsCmd&);

public:
  FieldsCmd(TclPkg* pkg, const FieldMap& fields);
  virtual ~FieldsCmd();

protected:
  virtual void invoke();
};

} // end namespace Tcl


///////////////////////////////////////////////////////////////////////
//
// FieldVecCmd member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::FieldVecCmd::FieldVecCmd(TclItemPkg* pkg, const FieldInfo& finfo) :
  TclCmd(pkg->interp(), pkg->makePkgCmdName(finfo.name().c_str()),
         (pkg->itemArgn() ? "item_id(s) ?new_value(s)?" : "?new_value?"),
         pkg->itemArgn()+1, pkg->itemArgn()+2, false),
  itsPkg(pkg),
  itsFinfo(finfo),
  itsItemArgn(pkg->itemArgn()),
  itsValArgn(pkg->itemArgn()+1),
  itsObjcGet(pkg->itemArgn()+1),
  itsObjcSet(pkg->itemArgn()+2)
{}

Tcl::FieldVecCmd::~FieldVecCmd() {}

void Tcl::FieldVecCmd::invoke() {
DOTRACE("Tcl::FieldVecCmd::invoke");

  DebugEvalNL(getCstringFromArg(0));

  // Fetch the item ids
  dynamic_block<int> ids(1);

  if (itsItemArgn) {
    unsigned int num_ids = getSequenceLengthOfArg(itsItemArgn);
    DebugEvalNL(num_ids);
    ids.resize(num_ids);
    getSequenceFromArg(itsItemArgn, &ids[0], (int*) 0);
  }
  else {
    // -1 is the cue to use the default item
    ids.at(0) = -1;
  }

  // If we are getting...
  if (TclCmd::objc() == itsObjcGet) {
    if ( ids.size() == 0 )
      /* return an empty Tcl result since the list of ids was empty */
      return;
    else if ( ids.size() == 1 )
      {
        Ref<FieldContainer> item(ids[0]);
        returnVal<const Value&>( *(item->field(itsFinfo).value()) );
      }
    else
      for (size_t i = 0; i < ids.size(); ++i) {
        Ref<FieldContainer> item(ids[i]);
        lappendVal( *(item->field(itsFinfo).value()) );
      }

  }
  // ... or if we are setting
  else if (TclCmd::objc() == itsObjcSet) {

    if (ids.size() == 1)
      {
        TclValue val = getValFromArg(itsValArgn, (TclValue*)0);

        Ref<FieldContainer> item(ids[0]);
        item->field(itsFinfo).setValue(val);
      }
    else
      {
        Tcl::ListIterator<Tcl::TclValue>
          val_itr = beginOfArg(itsValArgn, (TclValue*)0),
          val_end = endOfArg(itsValArgn, (TclValue*)0);

        TclValue val = *val_itr;

        for (size_t i = 0; i < ids.size(); ++i) {
          Ref<FieldContainer> item(ids[i]);
          item->field(itsFinfo).setValue(val);

          // Only fetch a new value if there are more values to
          // get... if we run out of values before we run out of ids,
          // then we just keep on using the last value in the sequence
          // of values.
          if (val_itr != val_end)
            if (++val_itr != val_end)
              val = *val_itr;
        }
      }
  }
  // ... or ... "can't happen"
  else {  Assert(0);  }
}


///////////////////////////////////////////////////////////////////////
//
// FieldsCmd member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::FieldsCmd::FieldsCmd(TclPkg* pkg, const FieldMap& fields) :
  TclCmd(pkg->interp(), pkg->makePkgCmdName("fields"), NULL, 1, 1),
  itsInterp(pkg->interp()),
  itsFields(fields),
  itsFieldList(0)
{}

Tcl::FieldsCmd::~FieldsCmd() {}

void Tcl::FieldsCmd::invoke() {
DOTRACE("Tcl::FieldsCmd::invoke");
  if (itsFieldList == 0) {

    minivec<Tcl_Obj*> elements;

    for (FieldMap::IoIterator
           itr = itsFields.ioBegin(),
           end = itsFields.ioEnd();
         itr != end;
         ++itr)
      {
        fixed_block<Tcl_Obj*> sub_elements(5);

        const FieldInfo& finfo = *itr;

        // property name
        sub_elements.at(0) = Tcl_NewStringObj(finfo.name().c_str(), -1);

        // min value
        TclValue min(finfo.min());
        sub_elements.at(1) = min.getObj();

        // max value
        TclValue max(finfo.max());
        sub_elements.at(2) = max.getObj();

        // resolution value
        TclValue res(finfo.res());
        sub_elements.at(3) = res.getObj();

        // start new group flag
        sub_elements.at(4) = Tcl_NewBooleanObj(finfo.startsNewGroup());

        elements.push_back(Tcl_NewListObj(sub_elements.size(),
                                          &(sub_elements[0])));
      }

    itsFieldList = Tcl_NewListObj(elements.size(), &(elements[0]));

    Tcl_IncrRefCount(itsFieldList);
  }

  returnVal<const Value&>(TclValue(itsFieldList));
}

///////////////////////////////////////////////////////////////////////
//
// Free function definitions
//
///////////////////////////////////////////////////////////////////////

void Tcl::declareField(Tcl::TclItemPkg* pkg, const FieldInfo& finfo) {
DOTRACE("Tcl::declareField");
  pkg->addCommand( new FieldVecCmd(pkg, finfo) );
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
