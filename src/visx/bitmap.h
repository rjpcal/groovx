///////////////////////////////////////////////////////////////////////
//
// bitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Thu Jul  1 13:49:20 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_H_DEFINED
#define BITMAP_H_DEFINED

#ifndef GROBJ_H_DEFINED
#include "grobj.h"
#endif

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

class Bitmap : public GrObj {
public:
  Bitmap();
  Bitmap(const char* filename);
  Bitmap(istream& is, IOFlag flag);

  virtual ~Bitmap();

  virtual void serialize(ostream& os, IOFlag flag) const;
  virtual void deserialize(istream& is, IOFlag flag);

  virtual int charCount() const;

  void loadPbmFile(const char* filename);

  void flipContrast();

  void flipVertical();

  virtual int getCategory() const { return -1; }
  virtual void setCategory(int) {}

  int byteCount() const;
  int bytesPerRow() const;

  double getRasterX() const;
  double getRasterY() const;
  double getZoomX() const;
  double getZoomY() const;
  bool getUsingGlBitmap() const;

  void setRasterX(double val);
  void setRasterY(double val);
  void setZoomX(double val);
  void setZoomY(double val);
  void setUsingGlBitmap(bool val);
  
  void center();

  virtual void undraw() const;

protected:
  virtual void grRender() const;

private:
  void init();
  void doFlipContrast();
  void doFlipVertical();

  string itsFilename;
  double itsRasterX;
  double itsRasterY;
  double itsZoomX;
  double itsZoomY;
  bool itsContrastFlip;
  bool itsVerticalFlip;
  bool itsUsingGlBitmap;

  unsigned char* itsBytes;
  int itsHeight;
  int itsWidth;
  int itsBitsPerPixel;
};

static const char vcid_bitmap_h[] = "$Header$";
#endif // !BITMAP_H_DEFINED
