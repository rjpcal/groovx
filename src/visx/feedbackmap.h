///////////////////////////////////////////////////////////////////////
//
// feedbackmap.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 16:58:49 2001
// written: Thu Jul 19 21:14:29 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FEEDBACKMAP_H_DEFINED
#define FEEDBACKMAP_H_DEFINED

#include "tcl/tclcode.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclobjptr.h"
#include "tcl/tclsafeinterp.h"
#include "util/minivec.h"
#include "util/strings.h"

class FeedbackMap {
public:
  FeedbackMap() :
    itsUseFeedback(true), itsRep(), itsItems(), isItDirty(true) {}

  const fixed_string& rep() const { return itsRep; }

  void set(const fixed_string& new_rep)
  {
    itsRep = new_rep;
    isItDirty = true;
    update();
  }

  void giveFeedback(const Tcl::SafeInterp& intp, int response) const
  {
    if (!itsUseFeedback) return;

    update();

    intp.setGlobalVar("resp_val", response);

    bool feedbackGiven = false;
    for (size_t i = 0; i<itsItems.size() && !feedbackGiven; ++i)
      {
        feedbackGiven = itsItems[i].invokeIfTrue(intp);
      }

    intp.unsetGlobalVar("resp_val");
  }

private:

  void update() const
  {
    if (!isItDirty) return;

    Tcl::List pairs_list(Tcl::ObjPtr(itsRep.c_str()));

    itsItems.clear();

    for (unsigned int i = 0; i < pairs_list.length(); ++i)
      {
        Tcl::List current_pair(pairs_list[i]);

        if (current_pair.length() != 2)
          {
            throw ErrorWithMsg("\"pair\" did not have length 2 "
                               "in FeedbackMap::update");
          }

        itsItems.push_back(Item(current_pair[0], current_pair[1]));
      }

    isItDirty = false;
  }

  class Item {
  public:
    Item(Tcl_Obj* cond, Tcl_Obj* res) :
      itsCondition(cond),
      itsResultCmd(res, Tcl::Code::THROW_EXCEPTION)
    {}

    bool invokeIfTrue(const Tcl::SafeInterp& safeInterp)
    {
      if (safeInterp.evalBooleanExpr(itsCondition))
        {
          itsResultCmd.invoke(safeInterp);
          return true;
        }
      return false;
    }

  private:
    Tcl::ObjPtr itsCondition;
    Tcl::Code itsResultCmd;
  };

public:
  bool itsUseFeedback;

private:
  fixed_string itsRep;
  mutable minivec<Item> itsItems;
  mutable bool isItDirty;
};

static const char vcid_feedbackmap_h[] = "$Header$";
#endif // !FEEDBACKMAP_H_DEFINED
