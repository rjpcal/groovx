///////////////////////////////////////////////////////////////////////
//
// feedbackmap.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 16:58:49 2001
// written: Wed Jun 26 11:26:50 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FEEDBACKMAP_H_DEFINED
#define FEEDBACKMAP_H_DEFINED


#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLCODE_H_DEFINED)
#include "tcl/tclcode.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLLISTOBJ_H_DEFINED)
#include "tcl/tcllistobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLOBJPTR_H_DEFINED)
#include "tcl/tclobjptr.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLSAFEINTERP_H_DEFINED)
#include "tcl/tclsafeinterp.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(MINIVEC_H_DEFINED)
#include "util/minivec.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H_DEFINED)
#include "util/strings.h"
#endif

/// FeedbackMap associates feedbacks with different boolean conditions.
class FeedbackMap
{
public:
  FeedbackMap() :
    itsUseFeedback(true), itsRep(), itsItems(), isItDirty(true) {}

  const fstring& rep() const { return itsRep; }

  void set(const fstring& new_rep)
  {
    itsRep = new_rep;
    isItDirty = true;
    update();
  }

  void giveFeedback(const Tcl::Interp& intp, int response) const
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
            throw Util::Error("\"pair\" did not have length 2 "
                              "in FeedbackMap::update");
          }

        itsItems.push_back(Item(current_pair[0], current_pair[1]));
      }

    isItDirty = false;
  }

  class Item
  {
  public:
    Item(Tcl_Obj* cond, Tcl_Obj* res) :
      itsCondition(cond),
      itsResultCmd(res, Tcl::Code::THROW_EXCEPTION)
    {}

    bool invokeIfTrue(const Tcl::Interp& safeInterp)
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
  fstring itsRep;
  mutable minivec<Item> itsItems;
  mutable bool isItDirty;
};

static const char vcid_feedbackmap_h[] = "$Header$";
#endif // !FEEDBACKMAP_H_DEFINED
