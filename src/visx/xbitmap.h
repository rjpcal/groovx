///////////////////////////////////////////////////////////////////////
//
// xitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:39:09 1999
// written: Mon Sep 20 11:56:13 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_H_DEFINED
#define XBITMAP_H_DEFINED

#ifndef BITMAP_H_DEFINED
#include "bitmap.h"
#endif

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

struct _XImage;
typedef _XImage XImage;

class XBitmap : public Bitmap {
public:
  // Creators
  XBitmap();
  XBitmap(const char* filename);
  XBitmap(istream& is, IOFlag flag);

  private: void init();
  
public:
  virtual ~XBitmap();

  virtual void serialize(ostream& os, IOFlag flag) const;
  virtual void deserialize(istream& is, IOFlag flag);

  virtual int charCount() const;

  // Actions
  protected: virtual void bytesChangeHook(unsigned char* theBytes,
														int width,
														int height,
														int bits_per_pixel,
														int byte_alignment);

  protected: virtual void doUndraw(int winRasterX,
											  int winRasterY,
											  int winWidthX,
											  int winHeightY) const;

  protected: virtual void doRender(unsigned char* /* bytes */,
											  double /* x_pos */,
											  double /* y_pos */,
											  int /* width */,
											  int /* height */,
											  int /* bits_per_pixel */,
											  int /* byte_alignment */,
											  double /* zoom_x */,
											  double /* zoom_y */) const;

private:
  XImage* itsImage;
};

static const char vcid_xbitmap_h[] = "$Header$";
#endif // !XBITMAP_H_DEFINED
