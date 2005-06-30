///////////////////////////////////////////////////////////////////////
//
// trialtcl.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun 21 09:51:54 1999
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

#ifndef GROOVX_VISX_TCLPKG_TRIAL_CC_UTC20050628171009_DEFINED
#define GROOVX_VISX_TCLPKG_TRIAL_CC_UTC20050628171009_DEFINED

#include "visx/tclpkg-trial.h"

#include "io/fieldpkg.h"

#include "gfx/gxnode.h"

#include "nub/objfactory.h"

#include "tcl/itertcl.h"
#include "tcl/tracertcl.h"

#include "visx/response.h"
#include "visx/responsehandler.h"
#include "visx/timinghdlr.h"
#include "visx/trial.h"

#include "rutz/trace.h"

extern "C"
int Trial_Init(Tcl_Interp* interp)
{
GVX_TRACE("Trial_Init");

  GVX_PKG_CREATE(pkg, interp, "Trial", "4.$Revision$");
  pkg->inheritPkg("Element");
  Tcl::defTracing(pkg, Trial::tracer);

  Tcl::defFieldContainer<Trial>(pkg, SRC_POS);
  Tcl::defCreator<Trial>(pkg);

  pkg->defSetter("addNode", &Trial::addNode, SRC_POS);
  pkg->defGetter("avgResponse", &Trial::avgResponse, SRC_POS);
  pkg->defGetter("avgRespTime", &Trial::avgRespTime, SRC_POS);
  pkg->defAction("clearObjs", &Trial::clearObjs, SRC_POS);
  pkg->defAction("clearResponses", &Trial::clearResponses, SRC_POS);
  pkg->defAttrib("correctResponse",
                 &Trial::getCorrectResponse,
                 &Trial::setCorrectResponse,
                 SRC_POS);
  pkg->defAttrib("currentNode",
                 &Trial::getCurrentNode,
                 &Trial::setCurrentNode,
                 SRC_POS);
  pkg->defAttrib("info", &Trial::vxInfo, &Trial::setInfo, SRC_POS);
  pkg->defGetter("lastResponse", &Trial::lastResponse, SRC_POS);
  pkg->defAction("nextNode", &Trial::trNextNode, SRC_POS);
  pkg->defGetter("nodes", &Trial::nodes, SRC_POS);
  pkg->defGetter("numResponses", &Trial::numResponses, SRC_POS);
  pkg->defGetter("responses", &Trial::responses, SRC_POS);
  pkg->defAttrib("responseHdlr",
                 &Trial::getResponseHandler,
                 &Trial::setResponseHandler,
                 SRC_POS);
  pkg->defGetter("stdInfo", &Trial::stdInfo, SRC_POS);
  pkg->defAttrib("timingHdlr",
                 &Trial::getTimingHdlr,
                 &Trial::setTimingHdlr,
                 SRC_POS);
  // FIXME this is redundant with Element::trialType
  pkg->defAttrib("type", &Trial::trialType, &Trial::setType, SRC_POS);

  nub::obj_factory::instance().register_creator(&Trial::make);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_visx_tclpkg_trial_cc_utc20050628171009[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TCLPKG_TRIAL_CC_UTC20050628171009_DEFINED
