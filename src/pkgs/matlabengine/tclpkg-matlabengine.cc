/** @file pkgs/matlabengine/tclpkg-matlabengine.cc tcl interface
    package to the matlab engine */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Jul 10 12:16:44 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "pkgs/matlabengine/tclpkg-matlabengine.h"

#include "pkgs/mtx/mtxobj.h"

#include "mtx/matlabinterface.h"

#include "nub/object.h"
#include "nub/objfactory.h"
#include "nub/ref.h"

#include "rutz/sfmt.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"

#include "rutz/error.h"

#include "rutz/trace.h"

#ifndef GVX_NO_MATLAB

#include <engine.h>

class MatlabEngine : public nub::object
{
private:
  MatlabEngine(const MatlabEngine&);
  MatlabEngine& operator=(const MatlabEngine&);

protected:
  MatlabEngine() :
    itsEngine(engOpen("matlab -nosplash")),
    itsBufSize(512),
    itsBuf(nullptr)
  {
    if (itsEngine == nullptr)
      {
        throw rutz::error("couldn't open MATLAB engine", SRC_POS);
      }

    itsBuf = new char[itsBufSize];
    engOutputBuffer(itsEngine, itsBuf, itsBufSize-1);
  }

public:
  static MatlabEngine* make() { return new MatlabEngine; }

  virtual ~MatlabEngine() noexcept
  {
    delete [] itsBuf;
    engClose(itsEngine);
  }

  const char* evalString(const char* cmd)
  {
    int result = engEvalString(itsEngine, cmd);
    return (result == 0) ? (itsBuf+2) : "";
  }

  nub::ref<MtxObj> getMtx(const char* name)
  {
    mxArray* arr = engGetVariable(itsEngine, name);
    if (arr == nullptr)
      {
        throw rutz::error(rutz::sfmt("no such MATLAB variable: '%s'",
                                     name), SRC_POS);
      }

    nub::ref<MtxObj> m(new MtxObj(make_mtx(arr, mtx::COPY)));

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

#else // defined(GVX_NO_MATLAB)

class MatlabEngine : public nub::object
{
public:
  virtual ~MatlabEngine() noexcept {}

  static MatlabEngine* make() { return new MatlabEngine; }

  const char* evalString(const char*)
  {
    throw rutz::error("matlab is not supported in this build", SRC_POS);
  }

  nub::ref<MtxObj> getMtx(const char*)
  {
    throw rutz::error("matlab is not supported in this build", SRC_POS);
  }
};

#endif

extern "C"
int Matlabengine_Init(Tcl_Interp* interp)
{
GVX_TRACE("Matlabengine_Init");

  return tcl::pkg::init
    (interp, "MatlabEngine", "4.0",
     [](tcl::pkg* pkg) {
      tcl::def_basic_type_cmds<MatlabEngine>(pkg, SRC_POS);

      pkg->def( "eval", "engine_id command", &MatlabEngine::evalString, SRC_POS );
      pkg->def( "get", "engine_id mtx_name", &MatlabEngine::getMtx, SRC_POS );

      pkg->eval("proc meval {args} { return [eval MatlabEngine::eval $args] }");
      pkg->eval("proc getMtx {args} { return [eval MatlabEngine::get $args] }");

      tcl::def_creator<MatlabEngine>(pkg);
    });
}
