///////////////////////////////////////////////////////////////////////
//
// gtext.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul  1 11:54:47 1999
// written: Fri Nov 10 17:03:52 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXT_H_DEFINED
#define GTEXT_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H_DEFINED)
#include "util/strings.h"
#endif

/**
 *
 * Exception class for Gtext.
 *
 **/

class GtextError : public ErrorWithMsg {
public:
  /// Construct with an informative message \a msg.
  GtextError(const char* msg);
  /// Virtual destructor.
  virtual ~GtextError();
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Gtext is a \c GrObj subclass that renders text as a series of
 * vector graphics in OpenGL. There is only one font currently
 * available.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Gtext : public GrObj {
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

  /// Change the text string to \a text.
  void setText(const char* text);

  /// Get the current text string.
  const char* getText() const;

  /// Change the stroke width of the font to \a width.
  void setStrokeWidth(int width);

  /// Get the current stroke width of the font.
  int getStrokeWidth() const;

protected:
  virtual void grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  virtual bool grHasBoundingBox() const;

  virtual void grRender(GWT::Canvas& canvas) const;

private:
  fixed_string itsText;
  int itsStrokeWidth;
  mutable unsigned int itsListBase;
};

static const char vcid_gtext_h[] = "$Header$";
#endif // !GTEXT_H_DEFINED
