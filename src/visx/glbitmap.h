///////////////////////////////////////////////////////////////////////
//
// glbitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep  8 11:02:30 1999
// written: Mon Sep 20 17:56:58 1999
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

class GLBitmap : public Bitmap {
public:
  // Creators

  GLBitmap();
  GLBitmap(const char* filename);
  GLBitmap(istream& is, IOFlag flag);

  private: void init();

public:
  virtual ~GLBitmap();

  virtual void serialize(ostream& os, IOFlag flag) const;
  virtual void deserialize(istream& is, IOFlag flag);

  virtual int charCount() const;

  // Actions
  protected: virtual void doRender(unsigned char* /* bytes */,
											  double /* x_pos */,
											  double /* y_pos */,
											  int /* width */,
											  int /* height */,
											  int /* bits_per_pixel */,
											  int /* byte_alignment */,
											  double /* zoom_x */,
											  double /* zoom_y */) const;

  // Accessors/Manipulators
  public:    bool getUsingGlBitmap() const;
  public:    void setUsingGlBitmap(bool val);
  
private:
  bool itsUsingGlBitmap;
};

static const char vcid_glbitmap_h[] = "$Header$";
#endif // !GLBITMAP_H_DEFINED
