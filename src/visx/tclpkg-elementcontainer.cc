/** @file visx/tclpkg-elementcontainer.cc tcl interface package for
    class ElementContainer */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue May 27 16:02:04 2003
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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
  void addElementIds(nub::ref<ElementContainer> container,
                     const tcl::list& trial_ids, unsigned int repeat)
  {
    for (const nub::uid objid: trial_ids.view_of<nub::uid>())
      {
        container->addElement(nub::ref<Element>(objid), repeat);
      }
  }
}

extern "C"
int Elementcontainer_Init(Tcl_Interp* interp)
{
GVX_TRACE("Elementcontainer_Init");

  return tcl::pkg::init
    (interp, "ElementContainer", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("Element");
      tcl::def_basic_type_cmds<Element>(pkg, SRC_POS);

      pkg->def("addElement", "objref element_id repeat=1",
               &ElementContainer::addElement, SRC_POS, 1u);

      pkg->def("addElements", "objref element_id(s) repeat=1",
               &addElementIds, SRC_POS, 1u);

      pkg->def_getter("currentElement", &ElementContainer::currentElement, SRC_POS);
      pkg->def_getter("isComplete", &ElementContainer::isComplete, SRC_POS);
      pkg->def_getter("numCompleted", &ElementContainer::numCompleted, SRC_POS);
      pkg->def_getter("numElements", &ElementContainer::numElements, SRC_POS);
      pkg->def_action("clearElements", &ElementContainer::clearElements, SRC_POS);
      pkg->def_vec("shuffle", "objref(s) rand_seed", &ElementContainer::shuffle, 1, SRC_POS);
      pkg->def_getter("elements", &ElementContainer::getElements, SRC_POS);
    });
}
