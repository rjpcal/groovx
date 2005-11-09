/** @file gfx/gxline.h GxNode subclass for drawing lines */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Apr 17 15:53:53 2002
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

#ifndef GROOVX_GFX_GXLINE_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXLINE_H_UTC20050626084024_DEFINED

#include "gfx/gbvec.h"
#include "gfx/gxnode.h"

#include "io/fields.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * The GxLine class offers a simple way to render a line onscreen.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GxLine : public GxNode, public FieldContainer
{
protected:
  /// Default constructor.
  GxLine();

public:
  /// Default creator.
  static GxLine* make();

  /// Virtual destructor.
  virtual ~GxLine() throw();

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

  ////////////
  // fields //
  ////////////

  /// The start point.
  GbVec3<double> start;

  /// The stop point.
  GbVec3<double> stop;

private:
  /// If itsWidth is less than zero, then we use the current default width.
  int itsWidth;

public:
  /// Get GxLine's FieldMap.
  static const FieldMap& classFields();

  /////////////
  // actions //
  /////////////

  /// Extend the bounding cube for this line.
  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

  /// Draw the line.
  virtual void draw(Gfx::Canvas&) const;

private:
  GxLine(const GxLine&);
  GxLine& operator=(const GxLine&);
};

static const char vcid_groovx_gfx_gxline_h_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXLINE_H_UTC20050626084024_DEFINED
