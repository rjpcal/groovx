///////////////////////////////////////////////////////////////////////
//
// matlabtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jul 10 12:16:44 2001
// written: Wed Jul 11 11:03:23 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MATLABTCL_CC_DEFINED
#define MATLABTCL_CC_DEFINED

#include "mtxobj.h"

#include "tcl/genericobjpkg.h"

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
  class EvalCmd;
  class GetMtxCmd;
  class MatlabPkg;
}

class MatlabTcl::EvalCmd : public Tcl::TclItemCmd<MatlabEngine> {
public:
  EvalCmd(Tcl::CTclItemPkg<MatlabEngine>* pkg, const char* cmd_name) :
    Tcl::TclItemCmd<MatlabEngine>(pkg, cmd_name, "engine command",
                                  pkg->itemArgn()+2) {}

protected:
  virtual void invoke() {
    MatlabEngine* eng = getItem();
    const char* cmd = getCstringFromArg(2);
    returnVal(eng->evalString(cmd));
  }
};

class MatlabTcl::GetMtxCmd : public Tcl::TclItemCmd<MatlabEngine> {
public:
  GetMtxCmd(Tcl::CTclItemPkg<MatlabEngine>* pkg, const char* cmd_name) :
    Tcl::TclItemCmd<MatlabEngine>(pkg, cmd_name, "engine name",
                                  pkg->itemArgn()+2) {}

protected:
  virtual void invoke() {
    MatlabEngine* eng = getItem();
    const char* name = getCstringFromArg(2);
    returnVal(eng->getMtx(name)->id());
  }
};

class MatlabTcl::MatlabPkg : public Tcl::GenericObjPkg<MatlabEngine> {
public:
  MatlabPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<MatlabEngine>(interp, "MatlabEngine", "$Revision$")
  {
    addCommand( new EvalCmd (this, "meval") );
    addCommand( new GetMtxCmd (this, "getMtx") );
  }
};

extern "C"
int Matlab_Init(Tcl_Interp* interp) {
DOTRACE("Matlab_Init");

  Tcl::TclPkg* pkg = new MatlabTcl::MatlabPkg(interp);

  Util::ObjFactory::theOne().registerCreatorFunc(&MatlabEngine::make);

  return pkg->initStatus();
}

static const char vcid_matlabtcl_cc[] = "$Header$";
#endif // !MATLABTCL_CC_DEFINED
