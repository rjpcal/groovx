///////////////////////////////////////////////////////////////////////
//
// gxpointset.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jul  5 12:43:20 2002
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

#ifndef GXPOINTSET_H_DEFINED
#define GXPOINTSET_H_DEFINED

#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

#include "util/arrayvalue.h"

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

static const char vcid_gxpointset_h[] = "$Header$";
#endif // !GXPOINTSET_H_DEFINED
