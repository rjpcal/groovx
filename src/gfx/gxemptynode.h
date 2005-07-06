/** @file gfx/gxemptynode.h empty graphic object */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Nov 14 16:59:04 2002
// commit: $Id$
// $HeadURL$
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
  virtual ~GxEmptyNode() throw();

  virtual void readFrom(IO::Reader&);
  virtual void writeTo(IO::Writer&) const;

  virtual void getBoundingCube(Gfx::Bbox&) const;

  virtual void draw(Gfx::Canvas&) const;

  /// Factory function.
  static GxEmptyNode* make();
};

static const char vcid_groovx_gfx_gxemptynode_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXEMPTYNODE_H_UTC20050626084023_DEFINED
