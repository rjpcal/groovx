///////////////////////////////////////////////////////////////////////
//
// responsemap.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 19 16:35:06 2001
// written: Thu Jul 19 16:40:08 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RESPONSEMAP_H_DEFINED
#define RESPONSEMAP_H_DEFINED

#include "response.h"
#include "util/minivec.h"
#include "util/strings.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclobjptr.h"
#include "tcl/tclregexp.h"

class ResponseMap {
public:
  ResponseMap(const char* rep) :
    itsStringRep(rep),
    itsItems(),
    isItDirty(true) {}

  void set(const fixed_string& new_rep)
  {
    itsStringRep = new_rep;;
    isItDirty = true;
  }

  const fixed_string& rep() const { return itsStringRep; }

  // Returns the response value associated with the first regexp that
  // matches the given character string, or returns
  // Response::INVALID_VALUE if no match is found.
  int valueFor(const char* response_string) const
  {
    update();

    for (size_t i = 0; i < itsItems.size(); ++i)
      {
        if (itsItems[i].regExp.matchesString(response_string))
          return itsItems[i].responseValue;
      }

    return Response::INVALID_VALUE;
  }

private:

  // Recompiles the internal table of regexps to correspond with the
  // list of regexps and response values stored in the string
  // itsResponseMap.itsStringRep.

  void update() const
  {
    if (!isItDirty) return;

    Tcl::List pairs_list(Tcl::ObjPtr(itsStringRep.c_str()));

    itsItems.clear();

    for (unsigned int i = 0; i < pairs_list.length(); ++i)
      {
        Tcl::List current_pair(pairs_list[i]);

        if (current_pair.length() != 2)
          {
            throw ErrorWithMsg("\"pair\" did not have length 2 "
                               "in ResponseMap::update");
          }

        Tcl::ObjPtr patternObj = current_pair[0];
        int response_val = current_pair.get<int>(1);

        itsItems.push_back(MapItem(patternObj, response_val));
      }

    isItDirty = false;
  }

  class MapItem {
  public:
    MapItem(Tcl::ObjPtr obj, int rv) : regExp(obj), responseValue(rv) {}

    Tcl::RegExp regExp;
    int responseValue;
  };

  fixed_string itsStringRep;
  mutable bool isItDirty;
  mutable minivec<MapItem> itsItems;
};

static const char vcid_responsemap_h[] = "$Header$";
#endif // !RESPONSEMAP_H_DEFINED
