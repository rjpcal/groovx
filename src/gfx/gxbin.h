///////////////////////////////////////////////////////////////////////
//
// gxbin.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 12:31:35 2002
// written: Wed Nov 13 12:33:50 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXBIN_H_DEFINED
#define GXBIN_H_DEFINED

#include "gfx/gxnode.h"

#include "util/ref.h"

//  ###################################################################
//  ===================================================================

/// GxBin is a GxNode subclass, and is a base class for single-item containers.

class GxBin : public GxNode
{
private:
  Util::SoftRef<GxNode> itsChild;

public:
  GxBin();

  GxBin(Util::SoftRef<GxNode> child);

  virtual ~GxBin();

  const Util::SoftRef<GxNode>& child() const { return itsChild; }

  void setChild(Util::SoftRef<GxNode> child) { itsChild = child; }
};

static const char vcid_gxbin_h[] = "$Header$";
#endif // !GXBIN_H_DEFINED
