/** @file visx/tclpkg-elementcontainer.cc tcl interface package for
    class ElementContainer */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
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
  void addElementIds(nub::ref<ElementContainer> container,
                     tcl::list trial_ids, int repeat)
  {
    for (tcl::list::iterator<nub::uid>
           itr = trial_ids.begin<nub::uid>(),
           end = trial_ids.end<nub::uid>();
         itr != end;
         ++itr)
      {
        container->addElement(nub::ref<Element>(*itr), repeat);
      }
  }
}

extern "C"
int Elementcontainer_Init(Tcl_Interp* interp)
{
GVX_TRACE("Elementcontainer_Init");

  GVX_PKG_CREATE(pkg, interp, "ElementContainer", "4.$Revision$");
  pkg->inherit_pkg("Element");
  tcl::def_basic_type_cmds<Element>(pkg, SRC_POS);

  pkg->def("addElement", "objref element_id",
           rutz::bind_last(rutz::mem_func
                           (&ElementContainer::addElement), 1),
           SRC_POS);

  pkg->def("addElements", "objref element_id(s)",
           rutz::bind_last(&addElementIds, 1), SRC_POS);

  pkg->def("addElements", "objref element_id(s) repeat=1",
           &addElementIds, SRC_POS);

  pkg->def_getter("currentElement", &ElementContainer::currentElement, SRC_POS);
  pkg->def_getter("isComplete", &ElementContainer::isComplete, SRC_POS);
  pkg->def_getter("numCompleted", &ElementContainer::numCompleted, SRC_POS);
  pkg->def_getter("numElements", &ElementContainer::numElements, SRC_POS);
  pkg->def_action("clearElements", &ElementContainer::clearElements, SRC_POS);
  pkg->def_vec("shuffle", "objref(s) rand_seed", &ElementContainer::shuffle, 1, SRC_POS);
  pkg->def_getter("elements", &ElementContainer::getElements, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

static const char __attribute__((used)) vcid_groovx_visx_tclpkg_elementcontainer_cc_utc20050628171008[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TCLPKG_ELEMENTCONTAINER_CC_UTC20050628171008_DEFINED
