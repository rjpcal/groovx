///////////////////////////////////////////////////////////////////////
//
// gtext.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 11:54:47 1999
// written: Mon Dec  6 14:00:05 1999
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
 * Gtext is a subclass that renders text as a series of vector
 * graphics in OpenGL. There is only one font currently available.
 *
 * @short Subclass of GrObj for rendering text in a simple vector font.
 **/
///////////////////////////////////////////////////////////////////////

class Gtext : public GrObj {
public:
  ///
  Gtext(const char* text=0);
  ///
  Gtext(istream& is, IOFlag flag);
  ///
  virtual ~Gtext();

  ///
  virtual void serialize(ostream &os, IOFlag flag) const;
  ///
  virtual void deserialize(istream &is, IOFlag flag);
  ///
  virtual int charCount() const;

  ///
  virtual void readFrom(Reader* reader);
  ///
  virtual void writeTo(Writer* writer) const;

  ///
  void setText(const char* text);
  ///
  const char* getText() const;

  ///
  void setStrokeWidth(int width);
  ///
  int getStrokeWidth() const;

protected:
  ///
  virtual bool grGetBoundingBox(Rect<double>& bbox,
										  int& border_pixels) const;

  ///
  virtual void grRender() const;

private:
  string itsText;
  int itsStrokeWidth;
  mutable unsigned int itsListBase;
};

static const char vcid_gtext_h[] = "$Header$";
#endif // !GTEXT_H_DEFINED
