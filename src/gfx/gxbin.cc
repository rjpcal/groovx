///////////////////////////////////////////////////////////////////////
//
// gxbin.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 13 12:32:03 2002
// commit: $Id$
// $HeadURL$
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

#ifndef GXBIN_CC_DEFINED
#define GXBIN_CC_DEFINED

#include "gxbin.h"

#include "gfx/gxemptynode.h"

GxBin::GxBin() : itsChild(GxEmptyNode::make()) {}

GxBin::GxBin(Nub::Ref<GxNode> child) : itsChild(child) {}

GxBin::~GxBin() throw() {}

void GxBin::setChild(const Nub::Ref<GxNode>& child)
{
  itsChild->sigNodeChanged.disconnect(this->sigNodeChanged.slot());
  itsChild = child;
  itsChild->sigNodeChanged.connect(this->sigNodeChanged.slot());
  this->sigNodeChanged.emit();
}

static const char vcid_gxbin_cc[] = "$Id$ $URL$";
#endif // !GXBIN_CC_DEFINED
