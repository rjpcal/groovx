///////////////////////////////////////////////////////////////////////
//
// hooktcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct  5 13:51:43 2000
// written: Fri Jan 18 16:07:01 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOOKTCL_CC_DEFINED
#define HOOKTCL_CC_DEFINED

#include "tcl/tclpkg.h"

#include "util/trace.h"

namespace
{
  size_t TOTAL = 0;
}

#if 0
#include <cstdlib>
#include <cstdio>

void* operator new(size_t bytes)
{
  TOTAL += bytes;
  printf("%d bytes  ", (int) bytes);
  Util::Trace::printStackTrace();
  return malloc(bytes);
}

void operator delete(void* space)
{
  free(space);
}
#endif

#define LOCAL_DEBUG
#include "util/debug.h"

#include "util/dlink_list.h"

namespace HookTcl
{
  void hook()
  {
    DebugEvalNL((void*)vcid_dlink_list_h);
  }

  size_t memUsage() { return TOTAL; }
}

#include "visx/grobj.h"
#include "util/objdb.h"
#include <tcl.h>

static int categoryCmd(ClientData clientData, Tcl_Interp* interp,
                       int objc, Tcl_Obj *const objv[])
{
  try
    {
      if (objc < 2 || objc > 3)
        {
          return TCL_ERROR;
        }

      int id;
      if (Tcl_GetIntFromObj(interp, objv[1], &id) != TCL_OK)
        {
          return TCL_ERROR;
        }

      Ref<FieldContainer> fc(id);

      static const Field& categoryfield = GrObj::classFields().field("category");

      if (objc == 2)
        {
          Tcl::ObjPtr category = categoryfield.getValue(fc.get());
          Tcl_SetObjResult(interp, category.obj());
        }
      else if (objc == 3)
        {
          categoryfield.setValue(fc.get(), Tcl::ObjPtr(objv[2]));
        }
    }
  catch (...)
    {
      return TCL_ERROR;
    }

  return TCL_OK;
}

extern "C"
int Hook_Init(Tcl_Interp* interp)
{
  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Hook", "$Revision$");

  Tcl_CreateObjCommand(interp, "hook::category", categoryCmd, 0, 0);

  pkg->def( "::hook", "", HookTcl::hook );
  pkg->def( "::memUsage", 0, HookTcl::memUsage );

  return pkg->initStatus();
}

static const char vcid_hooktcl_cc[] = "$Header$";
#endif // !HOOKTCL_CC_DEFINED
