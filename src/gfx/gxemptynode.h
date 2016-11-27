/** @file gfx/gxemptynode.h empty graphic object */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Nov 14 16:59:04 2002
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

#ifndef GROOVX_GFX_GXEMPTYNODE_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXEMPTYNODE_H_UTC20050626084023_DEFINED

#include "gfx/gxnode.h"

//  #######################################################
//  =======================================================

/// GxEmptyNode is a totally empty GxNode subclass for use as "null object".

class GxEmptyNode : public GxNode
{
private:
  GxEmptyNode();
public:
  virtual ~GxEmptyNode() noexcept;

  virtual void read_from(io::reader&) override;
  virtual void write_to(io::writer&) const override;

  virtual void getBoundingCube(Gfx::Bbox&) const override;

  virtual void draw(Gfx::Canvas&) const override;

  /// Factory function.
  static GxEmptyNode* make();
};

#endif // !GROOVX_GFX_GXEMPTYNODE_H_UTC20050626084023_DEFINED
