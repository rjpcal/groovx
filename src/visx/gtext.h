///////////////////////////////////////////////////////////////////////
//
// gtext.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 11:54:47 1999
// written: Wed Sep 25 19:02:35 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXT_H_DEFINED
#define GTEXT_H_DEFINED

#include "util/strings.h"

#include "visx/grobj.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Gtext is a \c GrObj subclass that renders text as a series of
 * vector graphics in OpenGL. There is only one font currently
 * available.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Gtext : public GrObj
{
protected:
  /// Construct with an initial text string \a text.
  Gtext(const char* text=0);

public:
  /// Default creator.
  static Gtext* make();

  /// Virtual destructor.
  virtual ~Gtext();

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

protected:
  virtual Gfx::Rect<double> grGetBoundingBox(Gfx::Canvas& canvas) const;

  virtual void grRender(Gfx::Canvas& canvas) const;

private:
  fstring itsText;
  int itsStrokeWidth;
  mutable unsigned int itsListBase;
};

static const char vcid_gtext_h[] = "$Header$";
#endif // !GTEXT_H_DEFINED
