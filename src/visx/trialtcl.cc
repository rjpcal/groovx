///////////////////////////////////////////////////////////////////////
//
// trialtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 09:51:54 1999
// written: Sat Aug 25 22:02:27 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALTCL_CC_DEFINED
#define TRIALTCL_CC_DEFINED

#include "trial.h"

#include "responsehandler.h"
#include "timinghdlr.h"

#include "gfx/gxnode.h"

#include "tcl/fieldpkg.h"
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
  pkg->defGetter("description", &Trial::description);
  pkg->defGetter("lastResponse", &Trial::lastResponse);
  pkg->defAction("nextNode", &Trial::trNextNode);
  pkg->defGetter("numResponses", &Trial::numResponses);
  pkg->defAttrib("responseHdlr",
                 &Trial::getResponseHandler, &Trial::setResponseHandler);
  pkg->defAttrib("timingHdlr",
                 &Trial::getTimingHdlr, &Trial::setTimingHdlr);
  pkg->defAttrib("type", &Trial::trialType, &Trial::setType);
  pkg->defAction("undoLastResponse", &Trial::undoLastResponse);

  Util::ObjFactory::theOne().registerCreatorFunc(&Trial::make);

  return pkg->initStatus();
}

static const char vcid_trialtcl_cc[] = "$Header$";
#endif // !TRIALTCL_CC_DEFINED
