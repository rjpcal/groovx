/** @file gfx/gxpointset.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jul  5 12:43:20 2002
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

#ifndef GROOVX_GFX_GXPOINTSET_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXPOINTSET_H_UTC20050626084024_DEFINED

#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

#include "rutz/arrayvalue.h"

//  #######################################################
//  =======================================================

/// GxPointSet is a GxNode subclass for drawing sets pf points.

class GxPointSet : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxPointSet();

public:
  /// Default creator.
  static GxPointSet* make();

  /// Virtual destructor.
  virtual ~GxPointSet() throw();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  ////////////
  // fields //
  ////////////

  /// The array of vertices that will be drawn onscreen.
  rutz::array_value<GbVec3<double> > itsPoints;

private:
  int itsStyle;

public:
  /// Get GxPointSet's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Get the bounding cube for the pointset.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /// Draw the pointset.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxPointSet(const GxPointSet&);
  GxPointSet& operator=(const GxPointSet&);
};

static const char vcid_groovx_gfx_gxpointset_h_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXPOINTSET_H_UTC20050626084024_DEFINED
