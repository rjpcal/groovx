///////////////////////////////////////////////////////////////////////
//
// morphyfacetcl.cc
// Rob Peters 
// created: Wed Sep  8 15:42:36 1999
// written: Thu Sep 30 10:55:19 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include <tcl.h>
#include <fstream.h>
#include <strstream.h>
#include <cstring>
#include <cctype>
#include <vector>

#include "iomgr.h"
#include "objlist.h"
#include "listitempkg.h"
#include "morphyface.h"
#include "tclcmd.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace MorphyFaceTcl {
  class MorphyFacePkg;
}

///////////////////////////////////////////////////////////////////////
//
// MorphyFacePkg class definition
//
///////////////////////////////////////////////////////////////////////

class MorphyFaceTcl::MorphyFacePkg : public ListItemPkg<MorphyFace, ObjList> {
public:
  MorphyFacePkg(Tcl_Interp* interp) :
	 ListItemPkg<MorphyFace, ObjList>(interp, ObjList::theObjList(),
												 "MorphyFace", "2.5")
  {
	 declareAllProperties();
  }
};
  
extern "C" Tcl_PackageInitProc Morphyface_Init;

int Morphyface_Init(Tcl_Interp* interp) {
DOTRACE("Morphyface_Init");

  new MorphyFaceTcl::MorphyFacePkg(interp);

  FactoryRegistrar<IO, MorphyFace> registrar(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_morphyfacetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
