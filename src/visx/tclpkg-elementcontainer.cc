///////////////////////////////////////////////////////////////////////
//
// elementcontainertcl.cc
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue May 27 16:02:04 2003
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

#ifndef GROOVX_VISX_TCLPKG_ELEMENTCONTAINER_CC_UTC20050628171008_DEFINED
#define GROOVX_VISX_TCLPKG_ELEMENTCONTAINER_CC_UTC20050628171008_DEFINED

#include "visx/tclpkg-elementcontainer.h"

#include "tcl/itertcl.h"
#include "tcl/objpkg.h"
#include "tcl/list.h"
#include "tcl/pkg.h"

#include "rutz/iter.h"

#include "visx/elementcontainer.h"

#include "rutz/trace.h"

namespace
{
  void addElementIds(Nub::Ref<ElementContainer> container,
                     Tcl::List trial_ids, int repeat)
  {
    for (Tcl::List::Iterator<Nub::UID>
           itr = trial_ids.begin<Nub::UID>(),
           end = trial_ids.end<Nub::UID>();
         itr != end;
         ++itr)
      {
        container->addElement(Nub::Ref<Element>(*itr), repeat);
      }
  }
}

extern "C"
int Elementcontainer_Init(Tcl_Interp* interp)
{
GVX_TRACE("Elementcontainer_Init");

  GVX_PKG_CREATE(pkg, interp, "ElementContainer", "4.$Revision$");
  pkg->inheritPkg("Element");
  Tcl::defGenericObjCmds<Element>(pkg, SRC_POS);

  pkg->def("addElement", "objref element_id",
           rutz::bind_last(rutz::mem_func
                           (&ElementContainer::addElement), 1),
           SRC_POS);

  pkg->def("addElements", "objref element_id(s)",
           rutz::bind_last(&addElementIds, 1), SRC_POS);

  pkg->def("addElements", "objref element_id(s) repeat=1",
           &addElementIds, SRC_POS);

  pkg->defGetter("currentElement", &ElementContainer::currentElement, SRC_POS);
  pkg->defGetter("isComplete", &ElementContainer::isComplete, SRC_POS);
  pkg->defGetter("numCompleted", &ElementContainer::numCompleted, SRC_POS);
  pkg->defGetter("numElements", &ElementContainer::numElements, SRC_POS);
  pkg->defAction("clearElements", &ElementContainer::clearElements, SRC_POS);
  pkg->defVec("shuffle", "objref(s) rand_seed", &ElementContainer::shuffle, 1, SRC_POS);
  pkg->defGetter("elements", &ElementContainer::getElements, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_visx_tclpkg_elementcontainer_cc_utc20050628171008[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TCLPKG_ELEMENTCONTAINER_CC_UTC20050628171008_DEFINED
