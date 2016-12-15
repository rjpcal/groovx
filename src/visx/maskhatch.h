/** @file visx/maskhatch.h graphic object for cross-hatched visual masks */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Sep 23 15:49:58 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_VISX_MASKHATCH_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_MASKHATCH_H_UTC20050626084016_DEFINED

#include "gfx/gxshapekit.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c MaskHatch is a subclass of \c GxShapeKit for rendering a visual mask
 * consisting of hatched lines. The pattern will contain vertical,
 * horizontal, and diagonal lines at 45-degrees left, and 45-degrees
 * right.
 *
 **/
///////////////////////////////////////////////////////////////////////

class MaskHatch : public GxShapeKit
{
protected:
  /// Default constructor.
  MaskHatch();

public:
  /// Default creator.
  static MaskHatch* make();

  /// Virtual destructor.
  virtual ~MaskHatch() noexcept;

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  static const FieldMap& classFields();

  ////////////////
  // properties //
  ////////////////

private:
  /// The number of lines that will be rendered in each direction.
  int itsNumLines;

  /// The pixel-width of each line.
  int itsLineWidth;

  // To connect to Signal's.
  void update();

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const override;

  virtual void grRender(Gfx::Canvas& canvas) const override;
};

#endif // !GROOVX_VISX_MASKHATCH_H_UTC20050626084016_DEFINED
