///////////////////////////////////////////////////////////////////////
//
// gxaligner.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Nov 13 12:58:27 2002
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

#ifndef GXALIGNER_H_DEFINED
#define GXALIGNER_H_DEFINED

#include "geom/vec2.h"

#include "gfx/gxbin.h"

//  #######################################################
//  =======================================================

/// Controls where its child is drawn relative to the origin.
class GxAligner : public GxBin
{
public:

  ///////////////////////////////////////////////////////////
  //
  // Alignment modes
  //
  ///////////////////////////////////////////////////////////

  /** The symbolic constants of type \c Mode provide several ways to
      position an object with respect to the OpenGL coordinate
      system. The current mode can be get/set with \c getMode() and \c
      setMode(). The default alignment mode is \c NATIVE_ALIGNMENT. */
  typedef int Mode;

  /** This is the default alignment mode. No additional translations
      are performed beyond whatever happens in the subclass's \c
      grRender() implementation. */
  static const Mode NATIVE_ALIGNMENT      = 1;

  /// The center of the object is aligned with the origin.
  static const Mode CENTER_ON_CENTER      = 2;

  /// The NorthWest corner of the object is aligned with the origin.
  static const Mode NW_ON_CENTER          = 3;

  /// The NorthEast corner of the object is aligned with the origin.
  static const Mode NE_ON_CENTER          = 4;

  /// The SouthWest corner of the object is aligned with the origin.
  static const Mode SW_ON_CENTER          = 5;

  /// The SouthEast corner of the object is aligned with the origin.
  static const Mode SE_ON_CENTER          = 6;

  /// The location of the center of the object may be set arbitrarily.
  static const Mode ARBITRARY_ON_CENTER   = 7;


  /// Construct with a given child object.
  GxAligner(Nub::SoftRef<GxNode> child);

  /// Get the center after aligning the given rect.
  geom::vec2<double> getCenter(const geom::rect<double>& bounds) const;

  /// Get the alignment mode.
  Mode getMode() const        { return itsMode; }

  /// Set the alignment mode.
  void setMode(Mode new_mode) { itsMode = new_mode; }

  virtual void readFrom(IO::Reader& /*reader*/) {};
  virtual void writeTo(IO::Writer& /*writer*/) const {};

  virtual void draw(Gfx::Canvas& canvas) const;

  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

private:
  void doAlignment(Gfx::Canvas& canvas,
                   const geom::rect<double>& native) const;

public:
  Mode itsMode;                 ///< Current alignment mode.
  geom::vec2<double> itsCenter; ///< Current center.
};


static const char vcid_gxaligner_h[] = "$Header$";
#endif // !GXALIGNER_H_DEFINED
