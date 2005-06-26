///////////////////////////////////////////////////////////////////////
//
// matlabtcl.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jul 10 12:16:44 2001
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef MATLABTCL_CC_DEFINED
#define MATLABTCL_CC_DEFINED

#include "pkgs/matlabengine/matlabtcl.h"

#include "pkgs/mtx/matlabinterface.h"
#include "pkgs/mtx/mtxobj.h"

#include "nub/object.h"
#include "nub/objfactory.h"
#include "nub/ref.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/error.h"

#include "util/trace.h"

#ifdef WITH_MATLAB

#include <engine.h>

class MatlabEngine : public Nub::Object
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
        throw rutz::error("couldn't open MATLAB engine", SRC_POS);
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

  Nub::Ref<MtxObj> getMtx(const char* name)
  {
    mxArray* arr = engGetVariable(itsEngine, name);
    if (arr == 0)
      {
        throw rutz::error(rutz::fstring("no such MATLAB variable: '",
                                        name, "'"), SRC_POS);
      }

    Nub::Ref<MtxObj> m(new MtxObj(make_mtx(arr, mtx::COPY)));

    mxDestroyArray(arr);

    return m;
  }

  void putArray(const mxArray* arr, const char* name)
  {
    int result = engPutVariable(itsEngine, name, arr);
    if (result != 0)
      {
        throw rutz::error("error while putting mxArray "
                          "into MATLAB engine", SRC_POS);
      }
  }

private:
  Engine* itsEngine;
  const int itsBufSize;
  char* itsBuf;
};

#else // !WITH_MATLAB

class MatlabEngine : public Nub::Object
{
public:
  virtual ~MatlabEngine() throw() {}

  static MatlabEngine* make() { return new MatlabEngine; }

  void noSupport()
  {
    throw rutz::error("matlab is not supported in this build", SRC_POS);
  }

  const char* evalString(const char*)
  {
    noSupport();
    return "can't happen";
  }

  Nub::Ref<MtxObj> getMtx(const char*)
  {
    noSupport();
    return Nub::Ref<MtxObj>(new MtxObj(mtx::empty_mtx()));
  }
};

#endif

extern "C"
int Matlabengine_Init(Tcl_Interp* interp)
{
DOTRACE("Matlabengine_Init");

  PKG_CREATE(interp, "MatlabEngine", "4.$Revision$");
  Tcl::defGenericObjCmds<MatlabEngine>(pkg, SRC_POS);

  pkg->def( "eval", "engine_id command", &MatlabEngine::evalString, SRC_POS );
  pkg->def( "get", "engine_id mtx_name", &MatlabEngine::getMtx, SRC_POS );

  pkg->eval("proc meval {args} { return [eval MatlabEngine::eval $args] }");
  pkg->eval("proc getMtx {args} { return [eval MatlabEngine::get $args] }");

  Nub::ObjFactory::theOne().register_creator(&MatlabEngine::make);

  PKG_RETURN;
}

static const char vcid_matlabtcl_cc[] = "$Id$ $URL$";
#endif // !MATLABTCL_CC_DEFINED
