///////////////////////////////////////////////////////////////////////
//
// gxtext.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 11:54:47 1999 (as gtext.h)
// written: Mon Jan 13 11:01:38 2003
// $Id$
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
  virtual ~GxText();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  static const FieldMap& classFields();

  /// Change the text string to \a text.
  void setText(const fstring& text);

  /// Get the current text string.
  const fstring& getText() const;

  /// Change the stroke width of the font to \a width.
  void setStrokeWidth(int width);

  /// Get the current stroke width of the font.
  int getStrokeWidth() const;

  void setFont(fstring name);

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
