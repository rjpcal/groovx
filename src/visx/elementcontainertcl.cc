///////////////////////////////////////////////////////////////////////
//
// elementcontainertcl.cc
//
// Copyright (c) 2003-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue May 27 16:02:04 2003
// commit: $Id$
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

#ifndef ELEMENTCONTAINERTCL_CC_DEFINED
#define ELEMENTCONTAINERTCL_CC_DEFINED

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "util/iter.h"

#include "visx/elementcontainer.h"

#include "util/trace.h"

namespace
{
  void addElementIds(Util::Ref<ElementContainer> container,
                     Tcl::List trial_ids, int repeat)
  {
    for (Tcl::List::Iterator<Util::UID>
           itr = trial_ids.begin<Util::UID>(),
           end = trial_ids.end<Util::UID>();
         itr != end;
         ++itr)
      {
        container->addElement(Util::Ref<Element>(*itr), repeat);
      }
  }
}

extern "C"
int Elementcontainer_Init(Tcl_Interp* interp)
{
DOTRACE("Elementcontainer_Init");

  PKG_CREATE(interp, "ElementContainer", "$Revision$");
  pkg->inheritPkg("Element");
  Tcl::defGenericObjCmds<Element>(pkg);

  pkg->def("addElement", "item_id element_id",
           Util::bindLast(Util::memFunc(&ElementContainer::addElement), 1));

  pkg->def("addElements", "item_id element_id(s)",
           Util::bindLast(&addElementIds, 1));

  pkg->def("addElements", "item_id element_id(s) repeat=1",
           &addElementIds);

  pkg->defGetter("currentElement", &ElementContainer::currentElement);
  pkg->defGetter("isComplete", &ElementContainer::isComplete);
  pkg->defGetter("numCompleted", &ElementContainer::numCompleted);
  pkg->defGetter("numElements", &ElementContainer::numElements);
  pkg->defAction("clearElements", &ElementContainer::clearElements);
  pkg->defSetter("shuffle", "item_id rand_seed", &ElementContainer::shuffle);
  pkg->defGetter("elements", &ElementContainer::getElements);

  PKG_RETURN;
}

static const char vcid_elementcontainertcl_cc[] = "$Header$";
#endif // !ELEMENTCONTAINERTCL_CC_DEFINED
