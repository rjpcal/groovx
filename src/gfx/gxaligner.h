/** @file gfx/gxaligner.h GxBin subclass that aligns its child
    relative to the origin */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Nov 13 12:58:27 2002
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

#ifndef GROOVX_GFX_GXALIGNER_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GXALIGNER_H_UTC20050626084023_DEFINED

#include "geom/vec2.h"

#include "gfx/gbvec.h"
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
  GxAligner(nub::soft_ref<GxNode> child);

  /// Virtual nothrow destructor.
  virtual ~GxAligner() noexcept;

  /// Get the center after aligning the given rect.
  geom::vec2<double> getCenter(const geom::rect<double>& bounds) const;

  /// Get the alignment mode.
  Mode getMode() const        { return itsMode; }

  /// Set the alignment mode.
  void setMode(Mode new_mode) { itsMode = new_mode; }

  virtual void read_from(io::reader& reader) override;
  virtual void write_to(io::writer& writer) const override;

  virtual void draw(Gfx::Canvas& canvas) const override;

  virtual void getBoundingCube(Gfx::Bbox& bbox) const override;

private:
  void doAlignment(Gfx::Canvas& canvas,
                   const geom::rect<double>& native) const;

public:
  Mode itsMode;             ///< Current alignment mode.
  GbVec2<double> itsCenter; ///< Current center.
};


#endif // !GROOVX_GFX_GXALIGNER_H_UTC20050626084023_DEFINED
