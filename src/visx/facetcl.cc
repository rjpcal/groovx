///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Sat Feb  9 12:21:56 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "visx/cloneface.h"
#include "visx/face.h"

#include "tcl/fieldpkg.h"

#include "util/trace.h"

class InnerFace : public Face
{
public:
  static InnerFace* make()
  {
    InnerFace* p = new InnerFace;
    p->setField("partsMask", 1);
    return p;
  }
};

extern "C"
int Face_Init(Tcl_Interp* interp)
{
DOTRACE("Face_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Face", "$Revision$");
  Tcl::defFieldContainer<Face>(pkg);
  Tcl::defCreator<Face>(pkg);

  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "CloneFace", "$Revision$");
  Tcl::defFieldContainer<CloneFace>(pkg2);
  Tcl::defCreator<CloneFace>(pkg2);

  Tcl::Pkg* pkg3 = new Tcl::Pkg(interp, "InnerFace", "$Revision$");
  Tcl::defFieldContainer<InnerFace>(pkg3);
  Tcl::defCreator<InnerFace>(pkg3);

  return Tcl::Pkg::initStatus(pkg, pkg2, pkg3);
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
