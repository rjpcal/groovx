///////////////////////////////////////////////////////////////////////
//
// matlabtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jul 10 12:16:44 2001
// written: Fri Jul 13 17:34:15 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MATLABTCL_CC_DEFINED
#define MATLABTCL_CC_DEFINED

#include "mtxobj.h"

#include "tcl/genericobjpkg.h"
#include "tcl/objfunctor.h"

#include "util/error.h"
#include "util/object.h"
#include "util/objfactory.h"
#include "util/ref.h"

#include <engine.h>

#include "util/trace.h"

class MatlabEngine : public Util::Object {
private:
  MatlabEngine(const MatlabEngine&);
  MatlabEngine& operator=(const MatlabEngine&);

protected:
  MatlabEngine() :
    itsEngine(engOpen("matlab -nosplash")),
    itsBufSize(512),
    itsBuf(0)
  {
    if (itsEngine == 0)
      {
        throw ErrorWithMsg("couldn't open MATLAB engine");
      }

    itsBuf = new char[itsBufSize];
    engOutputBuffer(itsEngine, itsBuf, itsBufSize-1);
  }

public:
  static MatlabEngine* make() { return new MatlabEngine; }

  ~MatlabEngine()
  {
    delete [] itsBuf;
    engClose(itsEngine);
  }

  const char* evalString(const char* cmd)
  {
    int result = engEvalString(itsEngine, cmd);
    return (result == 0) ? (itsBuf+2) : "";
  }

  // NOTE! This returns a *new* mxArray, which must be freed by the caller
  Ref<MtxObj> getMtx(const char* name)
  {
    mxArray* arr = engGetArray(itsEngine, name);
    if (arr == 0)
      {
        ErrorWithMsg err("no such MATLAB variable: ");
        err.appendMsg("'",name,"'");
        throw err;
      }

    Ref<MtxObj> mtx(new MtxObj(arr, Mtx::COPY));

    mxDestroyArray(arr);

    return mtx;
  }

  void putArray(const mxArray* arr)
  {
    int result = engPutArray(itsEngine, arr);
    if (result != 0)
      {
        throw ErrorWithMsg("error while putting mxArray into MATLAB engine");
      }
  }

private:
  Engine* itsEngine;
  const int itsBufSize;
  char* itsBuf;
};

namespace MatlabTcl
{
  class MatlabPkg;
}

class MatlabTcl::MatlabPkg : public Tcl::GenericObjPkg<MatlabEngine> {
public:
  MatlabPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<MatlabEngine>(interp, "MatlabEngine", "$Revision$")
  {
    Tcl::def( this, &MatlabEngine::evalString,
              "meval", "engine_id command" );
    Tcl::def( this, &MatlabEngine::getMtx,
              "getMtx", "engine_id mtx_name" );
  }
};

extern "C"
int Matlab_Init(Tcl_Interp* interp)
{
DOTRACE("Matlab_Init");

  Tcl::TclPkg* pkg = new MatlabTcl::MatlabPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&MatlabEngine::make);

  return pkg->initStatus();
}

static const char vcid_matlabtcl_cc[] = "$Header$";
#endif // !MATLABTCL_CC_DEFINED
