///////////////////////////////////////////////////////////////////////
//
// gxaligner.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Nov 13 12:58:27 2002
// written: Wed Mar 19 17:56:05 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXALIGNER_H_DEFINED
#define GXALIGNER_H_DEFINED

#include "gfx/gxbin.h"

#include "gx/vec2.h"

//  ###################################################################
//  ===================================================================

/// Controls how its child is aligned relative to the origin when it is drawn.
class GxAligner : public GxBin
{
public:

  ///////////////////////////////////////////////////////////////////////
  //
  // Alignment modes
  //
  ///////////////////////////////////////////////////////////////////////

  /** The symbolic constants of type \c Mode provide several ways to
      position an object with respect to the OpenGL coordinate system. The
      current mode can be get/set with \c getMode() and \c setMode(). The
      default alignment mode is \c NATIVE_ALIGNMENT. */
  typedef int Mode;

  /** This is the default alignment mode. No additional translations
      are performed beyond whatever happens in the subclass's \c
      grRender() implementation. */
  static const Mode NATIVE_ALIGNMENT      = 1;

  /// The center of the object is aligned with OpenGL's origin.
  static const Mode CENTER_ON_CENTER      = 2;

  /// The NorthWest corner of the object is aligned with OpenGL's origin.
  static const Mode NW_ON_CENTER          = 3;

  /// The NorthEast corner of the object is aligned with OpenGL's origin.
  static const Mode NE_ON_CENTER          = 4;

  /// The SouthWest corner of the object is aligned with OpenGL's origin.
  static const Mode SW_ON_CENTER          = 5;

  /// The SouthEast corner of the object is aligned with OpenGL's origin.
  static const Mode SE_ON_CENTER          = 6;

  /// The location of the center of the object may be set arbitrarily.
  static const Mode ARBITRARY_ON_CENTER   = 7;



  GxAligner(Util::SoftRef<GxNode> child);

  Gfx::Vec2<double> getCenter(const Gfx::Rect<double>& bounds) const;

  Mode getMode() const        { return itsMode; }

  void setMode(Mode new_mode) { itsMode = new_mode; }

  virtual void readFrom(IO::Reader* /*reader*/) {};
  virtual void writeTo(IO::Writer* /*writer*/) const {};

  virtual void draw(Gfx::Canvas& canvas) const;

  virtual void getBoundingCube(Gfx::Bbox& bbox) const;

private:
  void doAlignment(Gfx::Canvas& canvas, const Gfx::Rect<double>& native) const;

public:
  Mode itsMode;
  Gfx::Vec2<double> itsCenter;
};


static const char vcid_gxaligner_h[] = "$Header$";
#endif // !GXALIGNER_H_DEFINED
