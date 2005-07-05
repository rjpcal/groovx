///////////////////////////////////////////////////////////////////////
//
// matlabtcl.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Jul 10 12:16:44 2001
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_PKGS_MATLABENGINE_TCLPKG_MATLABENGINE_CC_UTC20050628170029_DEFINED
#define GROOVX_PKGS_MATLABENGINE_TCLPKG_MATLABENGINE_CC_UTC20050628170029_DEFINED

#include "pkgs/matlabengine/tclpkg-matlabengine.h"

#include "pkgs/mtx/matlabinterface.h"
#include "pkgs/mtx/mtxobj.h"

#include "nub/object.h"
#include "nub/objfactory.h"
#include "nub/ref.h"

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

  nub::ref<MtxObj> getMtx(const char* name)
  {
    mxArray* arr = engGetVariable(itsEngine, name);
    if (arr == 0)
      {
        throw rutz::error(rutz::fstring("no such MATLAB variable: '",
                                        name, "'"), SRC_POS);
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

  nub::ref<MtxObj> getMtx(const char*)
  {
    noSupport();
    return nub::ref<MtxObj>(new MtxObj(mtx::empty_mtx()));
  }
};

#endif

extern "C"
int Matlabengine_Init(Tcl_Interp* interp)
{
GVX_TRACE("Matlabengine_Init");

  GVX_PKG_CREATE(pkg, interp, "MatlabEngine", "4.$Revision$");
  tcl::def_basic_type_cmds<MatlabEngine>(pkg, SRC_POS);

  pkg->def( "eval", "engine_id command", &MatlabEngine::evalString, SRC_POS );
  pkg->def( "get", "engine_id mtx_name", &MatlabEngine::getMtx, SRC_POS );

  pkg->eval("proc meval {args} { return [eval MatlabEngine::eval $args] }");
  pkg->eval("proc getMtx {args} { return [eval MatlabEngine::get $args] }");

  nub::obj_factory::instance().register_creator(&MatlabEngine::make);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_pkgs_matlabengine_tclpkg_matlabengine_cc_utc20050628170029[] = "$Id$ $HeadURL$";
#endif // !GROOVX_PKGS_MATLABENGINE_TCLPKG_MATLABENGINE_CC_UTC20050628170029_DEFINED