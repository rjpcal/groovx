///////////////////////////////////////////////////////////////////////
//
// gxemptynode.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Nov 14 16:59:58 2002
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

#ifndef GXEMPTYNODE_CC_DEFINED
#define GXEMPTYNODE_CC_DEFINED

#include "gxemptynode.h"

GxEmptyNode::GxEmptyNode() {}

GxEmptyNode::~GxEmptyNode() throw() {}

void GxEmptyNode::readFrom(IO::Reader&) {}
void GxEmptyNode::writeTo(IO::Writer&) const {}

void GxEmptyNode::getBoundingCube(Gfx::Bbox&) const {}

void GxEmptyNode::draw(Gfx::Canvas&) const {}

GxEmptyNode* GxEmptyNode::make()
{
  static GxEmptyNode* p = 0;

  if (p == 0)
    {
      p = new GxEmptyNode;
      p->incrRefCount();
    }

  return p;
}

static const char vcid_gxemptynode_cc[] = "$Id$ $URL$";
#endif // !GXEMPTYNODE_CC_DEFINED
