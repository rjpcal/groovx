///////////////////////////////////////////////////////////////////////
//
// glbitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep  8 11:02:30 1999
// written: Sat Nov 20 22:03:29 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_H_DEFINED
#define GLBITMAP_H_DEFINED

#ifndef BITMAP_H_DEFINED
#include "bitmap.h"
#endif

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

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

  protected:
  ///
  virtual void doRender(unsigned char* /* bytes */,
								double /* x_pos */,
								double /* y_pos */,
								int /* width */,
								int /* height */,
								int /* bits_per_pixel */,
								int /* byte_alignment */,
								double /* zoom_x */,
								double /* zoom_y */) const;
  
  public:
  ///
  bool getUsingGlBitmap() const;
  ///
  void setUsingGlBitmap(bool val);
  
private:
  bool itsUsingGlBitmap;
};

static const char vcid_glbitmap_h[] = "$Header$";
#endif // !GLBITMAP_H_DEFINED
