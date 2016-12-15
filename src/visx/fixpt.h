/** @file visx/fixpt.h graphic object for fixation crosses */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Jan  4 08:00:00 1999
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

#ifndef GROOVX_VISX_FIXPT_H_UTC20050626084015_DEFINED
#define GROOVX_VISX_FIXPT_H_UTC20050626084015_DEFINED

#include "gfx/gxshapekit.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c FixPt is a subclass of \c GxShapeKit for drawing a simple
 * psychophysics fixation cross. \c FixPt has attributes to control
 * the length of the bars, and the pixel-width of the bars.
 *
 **/
///////////////////////////////////////////////////////////////////////

class FixPt : public GxShapeKit
{
protected:
  /// Construct with initial values for the cross length and pixel-width.
  FixPt (double len=0.1, int wid=1);

public:
  /// Default creator.
  static FixPt* make();

  /// Virtual destructor.
  virtual ~FixPt () noexcept;

  virtual io::version_id class_version_id() const override;
  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  /// Get FixPt's fields.
  static const FieldMap& classFields();

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const override;

  virtual void grRender(Gfx::Canvas& canvas) const override;

private:
  /// Length of crosshairs in GL coordinates.
  double itsLength;

  /// Width of crosshairs in pixels.
  int itsWidth;
};

#endif // !GROOVX_VISX_FIXPT_H_UTC20050626084015_DEFINED
