///////////////////////////////////////////////////////////////////////
//
// gxtext.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Jul  1 11:54:47 1999 (as gtext.h)
// commit: $Id$
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

#ifndef GXTEXT_H_DEFINED
#define GXTEXT_H_DEFINED

#include "gfx/gxshapekit.h"

#include "util/pointers.h"
#include "util/strings.h"

class GxFont;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c GxText is a \c GxShapeKit subclass that renders text as a series of
 * vector graphics in OpenGL. There is only one font currently available.
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

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /// Get GxText's fields.
  static const FieldMap& classFields();

  /// Change the text string to \a text.
  void setText(const fstring& text);

  /// Get the current text string.
  const fstring& getText() const;

  /// Change the stroke width of the font to \a width.
  void setStrokeWidth(int width);

  /// Get the current stroke width of the font.
  int getStrokeWidth() const;

  /// Set the font used to render this text.
  void setFont(fstring name);

  /// Get the name of the font used to render this text.
  fstring getFont() const;

protected:
  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const;

  virtual void grRender(Gfx::Canvas& canvas) const;

private:
  shared_ptr<GxFont> itsFont;
  fstring itsText;
  int itsStrokeWidth;
};

static const char vcid_gxtext_h[] = "$Header$";
#endif // !GXTEXT_H_DEFINED
