///////////////////////////////////////////////////////////////////////
//
// trialtcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun 21 09:51:54 1999
// written: Wed Mar 19 12:46:27 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALTCL_CC_DEFINED
#define TRIALTCL_CC_DEFINED

#include "visx/trial.h"

#include "visx/responsehandler.h"
#include "visx/timinghdlr.h"

#include "gfx/gxnode.h"

#include "tcl/fieldpkg.h"
#include "tcl/itertcl.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"

#include "util/trace.h"

extern "C"
int Trial_Init(Tcl_Interp* interp)
{
DOTRACE("Trial_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Trial", "$Revision$");
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
  pkg->defGetter("description", &Trial::status);
  pkg->defGetter("lastResponse", &Trial::lastResponse);
  pkg->defAction("nextNode", &Trial::trNextNode);
  pkg->defGetter("nodes", &Trial::nodes);
  pkg->defGetter("numResponses", &Trial::numResponses);
  pkg->defAttrib("responseHdlr",
                 &Trial::getResponseHandler, &Trial::setResponseHandler);
  pkg->defAttrib("timingHdlr",
                 &Trial::getTimingHdlr, &Trial::setTimingHdlr);
  pkg->defAttrib("type", &Trial::trialType, &Trial::setType);
  pkg->defAction("undoLastResponse", &Trial::vxUndo);

  Util::ObjFactory::theOne().registerCreatorFunc(&Trial::make);

  return pkg->initStatus();
}

static const char vcid_trialtcl_cc[] = "$Header$";
#endif // !TRIALTCL_CC_DEFINED
