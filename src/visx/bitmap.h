///////////////////////////////////////////////////////////////////////
//
// bitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Wed Jun 16 12:41:48 1999
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

  virtual int getCategory() const { return -1; }
  virtual void setCategory(int) {}

  void setRasterX(double val);
  void setRasterY(double val);
  
  double getRasterX() const;
  double getRasterY() const;

  void setZoomX(double val);
  void setZoomY(double val);
  
  double getZoomX() const;
  double getZoomY() const;

  virtual void undraw() const;

protected:
  virtual void grRecompile() const;

private:
  string itsFilename;
  double itsRasterX;
  double itsRasterY;
  double itsZoomX;
  double itsZoomY;
  auto_ptr<unsigned char> itsBytes;

  int itsHeight;
  int itsWidth;
  int itsBitsPerPixel;
};

static const char vcid_bitmap_h[] = "$Header$";
#endif // !BITMAP_H_DEFINED
