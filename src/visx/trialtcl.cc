///////////////////////////////////////////////////////////////////////
//
// trialtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 09:51:54 1999
// written: Fri Jul 13 18:35:22 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALTCL_CC_DEFINED
#define TRIALTCL_CC_DEFINED

#include "trial.h"

#include "responsehandler.h"
#include "timinghdlr.h"

#include "tcl/fieldpkg.h"
#include "tcl/objfunctor.h"
#include "tcl/tracertcl.h"

#include "util/objfactory.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace TrialTcl
{
  class TrialPkg;
}

///////////////////////////////////////////////////////////////////////
//
// TrialPkg class definition
//
///////////////////////////////////////////////////////////////////////

class TrialTcl::TrialPkg : public Tcl::FieldCntrPkg<Trial> {
public:
  TrialPkg(Tcl_Interp* interp) :
    Tcl::FieldCntrPkg<Trial>(interp, "Trial", "$Revision$")
  {
    Tcl::addTracing(this, Trial::tracer);

    Tcl::def( this, &Trial::add, "Trial::add", "trialid objid posid" );

    declareCSetter("addNode", &Trial::addNode);
    declareCGetter("avgResponse", &Trial::avgResponse);
    declareCGetter("avgRespTime", &Trial::avgRespTime);
    declareCAction("clearObjs", &Trial::clearObjs);
    declareCAction("clearResponses", &Trial::clearResponses);
    declareCAttrib("correctResponse",
                   &Trial::getCorrectResponse, &Trial::setCorrectResponse);
    declareCAttrib("currentNode",
                   &Trial::getCurrentNode, &Trial::setCurrentNode);
    declareCGetter("description", &Trial::description);
    declareCGetter("lastResponse", &Trial::lastResponse);
    declareCAction("nextNode", &Trial::trNextNode);
    declareCGetter("numResponses", &Trial::numResponses);
    declareCAttrib("responseHdlr",
                   &Trial::getResponseHandler, &Trial::setResponseHandler);
    declareCAttrib("timingHdlr",
                  &Trial::getTimingHdlr, &Trial::setTimingHdlr);
    declareCAttrib("type", &Trial::trialType, &Trial::setType);
    declareCAction("undoLastResponse", &Trial::undoLastResponse);
  }
};

//---------------------------------------------------------------------
//
// Trial_Init --
//
//---------------------------------------------------------------------

extern "C"
int Trial_Init(Tcl_Interp* interp)
{
DOTRACE("Trial_Init");

  Tcl::TclPkg* pkg = new TrialTcl::TrialPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&Trial::make);

  return pkg->initStatus();
}

static const char vcid_trialtcl_cc[] = "$Header$";
#endif // !TRIALTCL_CC_DEFINED
