///////////////////////////////////////////////////////////////////////
//
// gxemptynode.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Nov 14 16:59:04 2002
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

#ifndef GXEMPTYNODE_H_DEFINED
#define GXEMPTYNODE_H_DEFINED

#include "gfx/gxnode.h"

//  #######################################################
//  =======================================================

/// GxEmptyNode is a totally empty GxNode subclass for use as "null object".

class GxEmptyNode : public GxNode
{
private:
  GxEmptyNode();
public:
  virtual ~GxEmptyNode() throw();

  virtual void readFrom(IO::Reader&);
  virtual void writeTo(IO::Writer&) const;

  virtual void getBoundingCube(Gfx::Bbox&) const;

  virtual void draw(Gfx::Canvas&) const;

  /// Factory function.
  static GxEmptyNode* make();
};

static const char vcid_gxemptynode_h[] = "$Header$";
#endif // !GXEMPTYNODE_H_DEFINED
