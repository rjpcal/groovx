///////////////////////////////////////////////////////////////////////
//
// trialtcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun 21 09:51:54 1999
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

#ifndef TRIALTCL_CC_DEFINED
#define TRIALTCL_CC_DEFINED

#include "io/fieldpkg.h"

#include "gfx/gxnode.h"

#include "tcl/itertcl.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"

#include "visx/trial.h"

#include "visx/responsehandler.h"
#include "visx/timinghdlr.h"

#include "util/trace.h"

extern "C"
int Trial_Init(Tcl_Interp* interp)
{
DOTRACE("Trial_Init");

  PKG_CREATE(interp, "Trial", "$Revision$");
  pkg->inheritPkg("Element");
  Tcl::defTracing(pkg, Trial::tracer);

  Tcl::defFieldContainer<Trial>(pkg);
  Tcl::defCreator<Trial>(pkg);

  pkg->defSetter("addNode", &Trial::addNode);
  pkg->defGetter("avgResponse", &Trial::avgResponse);
  pkg->defGetter("avgRespTime", &Trial::avgRespTime);
  pkg->defAction("clearObjs", &Trial::clearObjs);
  pkg->defAction("clearResponses", &Trial::clearResponses);
  pkg->defAttrib("correctResponse",
                 &Trial::getCorrectResponse, &Trial::setCorrectResponse);
  pkg->defAttrib("currentNode",
                 &Trial::getCurrentNode, &Trial::setCurrentNode);
  pkg->defAttrib("info", &Trial::vxInfo, &Trial::setInfo);
  pkg->defGetter("lastResponse", &Trial::lastResponse);
  pkg->defAction("nextNode", &Trial::trNextNode);
  pkg->defGetter("nodes", &Trial::nodes);
  pkg->defGetter("numResponses", &Trial::numResponses);
  pkg->defAttrib("responseHdlr",
                 &Trial::getResponseHandler, &Trial::setResponseHandler);
  pkg->defGetter("stdInfo", &Trial::stdInfo);
  pkg->defAttrib("timingHdlr",
                 &Trial::getTimingHdlr, &Trial::setTimingHdlr);
  // FIXME this is redundant with Element::trialType
  pkg->defAttrib("type", &Trial::trialType, &Trial::setType);

  Util::ObjFactory::theOne().registerCreatorFunc(&Trial::make);

  PKG_RETURN;
}

static const char vcid_trialtcl_cc[] = "$Header$";
#endif // !TRIALTCL_CC_DEFINED
