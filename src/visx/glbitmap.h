///////////////////////////////////////////////////////////////////////
//
// glbitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep  8 11:02:30 1999
// written: Wed Dec  1 17:57:32 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_H_DEFINED
#define GLBITMAP_H_DEFINED

#ifndef BITMAP_H_DEFINED
#include "bitmap.h"
#endif

class GLBmapRenderer;

///////////////////////////////////////////////////////////////////////
/**
 *
 * GLBitmap is a subclass of Bitmap that renders the bitmap data using
 * OpenGL's glDrawPixels() or glBitmap().
 *
 * @memo Bitmap subclass for rendering using OpenGL's glDrawPixels().
 **/
///////////////////////////////////////////////////////////////////////

class GLBitmap : public Bitmap {
public:
  // Creators

  ///
  GLBitmap();
  ///
  GLBitmap(const char* filename);
  ///
  GLBitmap(istream& is, IOFlag flag);

  private: void init();

public:
  ///
  virtual ~GLBitmap();

  ///
  virtual void serialize(ostream& os, IOFlag flag) const;
  ///
  virtual void deserialize(istream& is, IOFlag flag);

  ///
  virtual int charCount() const;

  ///
  virtual void readFrom(Reader* reader);
  ///
  virtual void writeTo(Writer* writer) const;

  public:
  ///
  bool getUsingGlBitmap() const;
  ///
  void setUsingGlBitmap(bool val);

private:
  GLBmapRenderer* itsRenderer;
};

static const char vcid_glbitmap_h[] = "$Header$";
#endif // !GLBITMAP_H_DEFINED
