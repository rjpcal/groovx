/** @file gfx/gxtext.h GxShapeKit subclass for rendering text */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Jul  1 11:54:47 1999 (as gtext.h)
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

#ifndef GROOVX_GFX_GXTEXT_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GXTEXT_H_UTC20050626084024_DEFINED

#include "gfx/gxshapekit.h"

#include "rutz/fstring.h"
#include "rutz/shared_ptr.h"

class GxFont;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c GxText is a \c GxShapeKit subclass that renders text as a series
 * of vector graphics in OpenGL.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GxText : public GxShapeKit
{
protected:
  /// Construct with an initial text string \a text.
  GxText(const char* text=0);

public:
  /// Default creator.
  static GxText* make();

  /// Virtual destructor.
  virtual ~GxText() throw();

  virtual io::version_id class_version_id() const;
  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

  /// Get GxText's fields.
  static const FieldMap& classFields();

  /// Change the text string to \a text.
  void setText(const rutz::fstring& text);

  /// Get the current text string.
  const rutz::fstring& getText() const;

  /// Change the stroke width of the font to \a width.
  void setStrokeWidth(int width);

  /// Get the current stroke width of the font.
  int getStrokeWidth() const;

  /// Set the font used to render this text.
  void setFont(rutz::fstring name);

  /// Get the name of the font used to render this text.
  rutz::fstring getFont() const;

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const;

  virtual void grRender(Gfx::Canvas& canvas) const;

private:
  rutz::shared_ptr<GxFont> itsFont;
  rutz::fstring itsText;
  int itsStrokeWidth;
};

static const char __attribute__((used)) vcid_groovx_gfx_gxtext_h_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GXTEXT_H_UTC20050626084024_DEFINED
