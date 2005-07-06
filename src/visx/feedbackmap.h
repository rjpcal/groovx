/** @file visx/feedbackmap.h trigger user-feedback events upon certain
    conditions in a psychophysics experiment*/

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Jul 19 16:58:49 2001
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_FEEDBACKMAP_H_UTC20050626084017_DEFINED
#define GROOVX_VISX_FEEDBACKMAP_H_UTC20050626084017_DEFINED

#include "tcl/list.h"
#include "tcl/obj.h"
#include "tcl/interp.h"

#include "rutz/error.h"
#include "rutz/fstring.h"

#include <vector>

/// FeedbackMap associates feedbacks with different boolean conditions.
class FeedbackMap
{
public:
  /// Default constructor.
  FeedbackMap() :
    itsUseFeedback(true), itsRep(), itsItems(), isItDirty(true) {}

  /// Get the string representation for serialization.
  const rutz::fstring& rep() const { return itsRep; }

  /// Set from a string representation for serialization.
  void set(const rutz::fstring& new_rep)
  {
    itsRep = new_rep;
    isItDirty = true;
    update();
  }

  /// Generate any feedback associated with the given response.
  void giveFeedback(tcl::interpreter& intp, int response) const
  {
    if (!itsUseFeedback) return;

    update();

    intp.set_global_var("resp_val", tcl::convert_from(response));

    bool feedbackGiven = false;
    for (size_t i = 0; i<itsItems.size() && !feedbackGiven; ++i)
      {
        feedbackGiven = itsItems[i].invokeIfTrue(intp);
      }

    intp.unset_global_var("resp_val");
  }

private:

  /// Regenerate from the string representation if needed.
  void update() const
  {
    if (!isItDirty) return;

    tcl::list pairs_list(tcl::convert_from(itsRep));

    itsItems.clear();

    for (unsigned int i = 0; i < pairs_list.length(); ++i)
      {
        tcl::list current_pair(pairs_list[i]);

        if (current_pair.length() != 2)
          {
            throw rutz::error("\"pair\" did not have length 2 "
                              "in FeedbackMap::update", SRC_POS);
          }

        itsItems.push_back(Item(current_pair[0], current_pair[1]));
      }

    isItDirty = false;
  }

  /// Represents a condition+feedback pairing.
  class Item
  {
  public:
    /// Construct with a boolean condition and an associated result script.
    Item(Tcl_Obj* cond, Tcl_Obj* res) :
      itsCondition(cond),
      itsResultCmd(res)
    {}

    /// Invoke the associated feedback script if the Item's condition is true.
    bool invokeIfTrue(tcl::interpreter& interp)
    {
      if (interp.eval_boolean_expr(itsCondition))
        {
          interp.eval(itsResultCmd);
          return true;
        }
      return false;
    }

  private:
    tcl::obj itsCondition;
    tcl::obj itsResultCmd;
  };

public:
  bool itsUseFeedback; ///< Whether any feedback should be used at all

private:
  rutz::fstring itsRep;
  mutable std::vector<Item> itsItems;
  mutable bool isItDirty;
};

static const char vcid_groovx_visx_feedbackmap_h_utc20050626084017[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_FEEDBACKMAP_H_UTC20050626084017_DEFINED
