///////////////////////////////////////////////////////////////////////
//
// trialtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 09:51:54 1999
// written: Mon Jul 16 09:43:10 2001
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

    defSetter("addNode", &Trial::addNode);
    defGetter("avgResponse", &Trial::avgResponse);
    defGetter("avgRespTime", &Trial::avgRespTime);
    defAction("clearObjs", &Trial::clearObjs);
    defAction("clearResponses", &Trial::clearResponses);
    defAttrib("correctResponse",
              &Trial::getCorrectResponse, &Trial::setCorrectResponse);
    defAttrib("currentNode",
              &Trial::getCurrentNode, &Trial::setCurrentNode);
    defGetter("description", &Trial::description);
    defGetter("lastResponse", &Trial::lastResponse);
    defAction("nextNode", &Trial::trNextNode);
    defGetter("numResponses", &Trial::numResponses);
    defAttrib("responseHdlr",
              &Trial::getResponseHandler, &Trial::setResponseHandler);
    defAttrib("timingHdlr",
              &Trial::getTimingHdlr, &Trial::setTimingHdlr);
    defAttrib("type", &Trial::trialType, &Trial::setType);
    defAction("undoLastResponse", &Trial::undoLastResponse);
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
