///////////////////////////////////////////////////////////////////////
//
// matlabtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jul 10 12:16:44 2001
// written: Fri Sep 14 14:06:30 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MATLABTCL_CC_DEFINED
#define MATLABTCL_CC_DEFINED

#include "pkgs/mtx/mtxobj.h"

#include "tcl/tclpkg.h"

#include "util/error.h"
#include "util/object.h"
#include "util/objfactory.h"
#include "util/ref.h"

#include <engine.h>

#include "util/trace.h"

class MatlabEngine : public Util::Object
{
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
        throw Util::Error("couldn't open MATLAB engine");
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

  Ref<MtxObj> getMtx(const char* name)
  {
    mxArray* arr = engGetArray(itsEngine, name);
    if (arr == 0)
      {
        throw Util::Error(fstring("no such MATLAB variable: '", name, "'"));
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
        throw Util::Error("error while putting mxArray into MATLAB engine");
      }
  }

private:
  Engine* itsEngine;
  const int itsBufSize;
  char* itsBuf;
};


extern "C"
int Matlabengine_Init(Tcl_Interp* interp)
{
DOTRACE("Matlabengine_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "MatlabEngine", "$Revision$");
  Tcl::defGenericObjCmds<MatlabEngine>(pkg);

  pkg->def( "eval", "engine_id command", &MatlabEngine::evalString );
  pkg->def( "get", "engine_id mtx_name", &MatlabEngine::getMtx );

  pkg->eval("proc meval {args} { return [eval MatlabEngine::eval $args] }");
  pkg->eval("proc getMtx {args} { return [eval MatlabEngine::get $args] }");

  Util::ObjFactory::theOne().registerCreatorFunc(&MatlabEngine::make);

  return pkg->initStatus();
}

static const char vcid_matlabtcl_cc[] = "$Header$";
#endif // !MATLABTCL_CC_DEFINED
