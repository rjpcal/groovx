///////////////////////////////////////////////////////////////////////
//
// gxbin.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 12:31:35 2002
// written: Wed Mar 19 17:56:05 2003
// $Id$
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
