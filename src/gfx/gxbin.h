///////////////////////////////////////////////////////////////////////
//
// gxbin.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 12:31:35 2002
// written: Wed Mar 19 12:46:03 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXBIN_H_DEFINED
#define GXBIN_H_DEFINED

#include "gfx/gxnode.h"

#include "util/ref.h"

namespace Gfx
{
  class Txform;
}

//  ###################################################################
//  ===================================================================

/// GxBin is a GxNode subclass, and is a base class for single-item containers.

class GxBin : public GxNode
{
private:
  Util::Ref<GxNode> itsChild;

public:
  GxBin();

  GxBin(Util::Ref<GxNode> child);

  virtual ~GxBin();

  const Util::Ref<GxNode>& child() const { return itsChild; }

  void setChild(const Util::Ref<GxNode>& child);
};

static const char vcid_gxbin_h[] = "$Header$";
#endif // !GXBIN_H_DEFINED
