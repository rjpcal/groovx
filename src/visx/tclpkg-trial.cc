/** @file visx/tclpkg-trial.cc tcl interface package for class Trial */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Jun 21 09:51:54 1999
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

#include "visx/tclpkg-trial.h"

#include "gfx/gxnode.h"

#include "nub/objfactory.h"

#include "tcl/itertcl.h"
#include "tcl/tracertcl.h"

#include "tcl-io/fieldpkg.h"

#include "visx/response.h"
#include "visx/responsehandler.h"
#include "visx/timinghdlr.h"
#include "visx/trial.h"

#include "rutz/trace.h"

namespace tcl
{
  template <>
  struct help_convert<Response> : public help_convert<rutz::value> {};
}

extern "C"
int Trial_Init(Tcl_Interp* interp)
{
GVX_TRACE("Trial_Init");

  return tcl::pkg::init
    (interp, "Trial", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("Element");
      tcl::def_tracing(pkg, Trial::tracer);

      tcl::defFieldContainer<Trial>(pkg, SRC_POS);
      tcl::def_creator<Trial>(pkg);

      pkg->def_setter("addNode", &Trial::addNode, SRC_POS);
      pkg->def_getter("avgResponse", &Trial::avgResponse, SRC_POS);
      pkg->def_getter("avgRespTime", &Trial::avgRespTime, SRC_POS);
      pkg->def_action("clearObjs", &Trial::clearObjs, SRC_POS);
      pkg->def_action("clearResponses", &Trial::clearResponses, SRC_POS);
      pkg->def_get_set("correctResponse",
                       &Trial::getCorrectResponse,
                       &Trial::setCorrectResponse,
                       SRC_POS);
      pkg->def_get_set("currentNode",
                       &Trial::getCurrentNode,
                       &Trial::setCurrentNode,
                       SRC_POS);
      pkg->def_get_set("info", &Trial::vxInfo, &Trial::setInfo, SRC_POS);
      pkg->def_getter("lastResponse", &Trial::lastResponse, SRC_POS);
      pkg->def_action("nextNode", &Trial::trNextNode, SRC_POS);
      pkg->def_getter("nodes", &Trial::nodes, SRC_POS);
      pkg->def_getter("numResponses", &Trial::numResponses, SRC_POS);
      pkg->def_getter("responses", &Trial::responses, SRC_POS);
      pkg->def_get_set("responseHdlr",
                       &Trial::getResponseHandler,
                       &Trial::setResponseHandler,
                       SRC_POS);
      pkg->def_getter("stdInfo", &Trial::stdInfo, SRC_POS);
      pkg->def_get_set("timingHdlr",
                       &Trial::getTimingHdlr,
                       &Trial::setTimingHdlr,
                       SRC_POS);
      // FIXME this is redundant with Element::trialType
      pkg->def_get_set("type", &Trial::trialType, &Trial::setType, SRC_POS);
    });
}
