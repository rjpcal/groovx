///////////////////////////////////////////////////////////////////////
//
// gtext.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 11:54:47 1999
// written: Fri Feb 18 09:06:16 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTEXT_H_DEFINED
#define GTEXT_H_DEFINED

#ifndef GROBJ_H_DEFINED
#include "grobj.h"
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

#ifndef STRING_DEFINED
#include <string>
#endif

/**
 *
 * Exception class for Gtext.
 *
 **/

class GtextError : public ErrorWithMsg {
public:
  GtextError(const string& str = "") : ErrorWithMsg(str) {}
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
public:
  /// Construct with an initial text string \a text.
  Gtext(const char* text=0);

  /// Construct from an \c istream using \c deserialize().
  Gtext(istream& is, IOFlag flag);

  /// Virtual destructor.
  virtual ~Gtext();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

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

  virtual void grRender(Canvas& canvas) const;

private:
  string itsText;
  int itsStrokeWidth;
  mutable unsigned int itsListBase;
};

static const char vcid_gtext_h[] = "$Header$";
#endif // !GTEXT_H_DEFINED
