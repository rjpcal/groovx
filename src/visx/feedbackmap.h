///////////////////////////////////////////////////////////////////////
//
// feedbackmap.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Jul 19 16:58:49 2001
// written: Sat Mar 29 12:55:03 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef FEEDBACKMAP_H_DEFINED
#define FEEDBACKMAP_H_DEFINED

#include "tcl/tcllistobj.h"
#include "tcl/tclobjptr.h"
#include "tcl/tclsafeinterp.h"

#include "util/error.h"
#include "util/minivec.h"
#include "util/strings.h"

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

  void giveFeedback(Tcl::Interp& intp, int response) const
  {
    if (!itsUseFeedback) return;

    update();

    intp.setGlobalVar("resp_val", Tcl::toTcl(response));

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

    Tcl::List pairs_list(Tcl::toTcl(itsRep));

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

  /// Represents a condition+feedback pairing.
  class Item
  {
  public:
    Item(Tcl_Obj* cond, Tcl_Obj* res) :
      itsCondition(cond),
      itsResultCmd(res)
    {}

    bool invokeIfTrue(Tcl::Interp& interp)
    {
      if (interp.evalBooleanExpr(itsCondition))
        {
          interp.eval(itsResultCmd);
          return true;
        }
      return false;
    }

  private:
    Tcl::ObjPtr itsCondition;
    Tcl::ObjPtr itsResultCmd;
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
