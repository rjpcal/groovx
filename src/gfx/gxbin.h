/** @file gfx/gxbin.h graphic object class for single-item containers */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 13 12:31:35 2002
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

#ifndef GROOVX_GFX_GXBIN_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXBIN_H_UTC20050626084023_DEFINED

#include "gfx/gxnode.h"

#include "nub/ref.h"

namespace geom
{
  class txform;
}

//  #######################################################
//  =======================================================

/// GxBin is a GxNode subclass, and is a base class for single-item containers.

class GxBin : public GxNode
{
private:
  nub::ref<GxNode> itsChild;

public:
  /// Default constructor.
  GxBin();

  /// Construct with a given child node.
  GxBin(nub::ref<GxNode> child);

  /// Virtual destructor.
  virtual ~GxBin() throw();

  /// Get the bin's child node.
  const nub::ref<GxNode>& child() const { return itsChild; }

  /// Set the bin's child node.
  void setChild(const nub::ref<GxNode>& child);
};

static const char __attribute__((used)) vcid_groovx_gfx_gxbin_h_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXBIN_H_UTC20050626084023_DEFINED
