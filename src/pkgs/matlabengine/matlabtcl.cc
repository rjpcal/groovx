///////////////////////////////////////////////////////////////////////
//
// matlabtcl.cc
//
// Copyright (c) 2001-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jul 10 12:16:44 2001
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

#ifndef MATLABTCL_CC_DEFINED
#define MATLABTCL_CC_DEFINED

#include "pkgs/mtx/mtxobj.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/error.h"
#include "util/object.h"
#include "util/objfactory.h"
#include "util/ref.h"

#include "util/trace.h"

#ifdef WITH_MATLAB

#include <engine.h>

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

  virtual ~MatlabEngine() throw()
  {
    delete [] itsBuf;
    engClose(itsEngine);
  }

  const char* evalString(const char* cmd)
  {
    int result = engEvalString(itsEngine, cmd);
    return (result == 0) ? (itsBuf+2) : "";
  }

  Util::Ref<MtxObj> getMtx(const char* name)
  {
    mxArray* arr = engGetVariable(itsEngine, name);
    if (arr == 0)
      {
        throw Util::Error(fstring("no such MATLAB variable: '", name, "'"));
      }

    Util::Ref<MtxObj> mtx(new MtxObj(arr, mtx::COPY));

    mxDestroyArray(arr);

    return mtx;
  }

  void putArray(const mxArray* arr, const char* name)
  {
    int result = engPutVariable(itsEngine, name, arr);
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

#else // !WITH_MATLAB

class MatlabEngine : public Util::Object
{
public:
  virtual ~MatlabEngine() throw() {}

  static MatlabEngine* make() { return new MatlabEngine; }

  void noSupport()
  {
    throw Util::Error("matlab is not supported in this build");
  }

  const char* evalString(const char*)
  {
    noSupport();
    return "can't happen";
  }

  Util::Ref<MtxObj> getMtx(const char*)
  {
    noSupport();
    return Util::Ref<MtxObj>(new MtxObj(0,0));
  }
};

#endif

extern "C"
int Matlabengine_Init(Tcl_Interp* interp)
{
DOTRACE("Matlabengine_Init");

  PKG_CREATE(interp, "MatlabEngine", "$Revision$");
  Tcl::defGenericObjCmds<MatlabEngine>(pkg);

  pkg->def( "eval", "engine_id command", &MatlabEngine::evalString );
  pkg->def( "get", "engine_id mtx_name", &MatlabEngine::getMtx );

  pkg->eval("proc meval {args} { return [eval MatlabEngine::eval $args] }");
  pkg->eval("proc getMtx {args} { return [eval MatlabEngine::get $args] }");

  Util::ObjFactory::theOne().registerCreatorFunc(&MatlabEngine::make);

  PKG_RETURN;
}

static const char vcid_matlabtcl_cc[] = "$Header$";
#endif // !MATLABTCL_CC_DEFINED
