///////////////////////////////////////////////////////////////////////
//
// bitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Wed Sep  8 13:14:11 1999
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
  // Creators
public:
  Bitmap();
  Bitmap(const char* filename);
  Bitmap(istream& is, IOFlag flag);

  private: void init();

public:
  virtual ~Bitmap();

  virtual void serialize(ostream& os, IOFlag flag) const;
  virtual void deserialize(istream& is, IOFlag flag);

  virtual int charCount() const;

  // Actions

  public:    void loadPbmFile(const char* filename);

  protected: virtual void bytesChangeHook(unsigned char* /* theBytes */,
														int /* width */,
														int /* height */) {}

  public:    void flipContrast();
  protected: void doFlipContrast();

  public:    void flipVertical();
  protected: void doFlipVertical();

  public:    void center();

  protected: virtual void grRender() const;
  protected: virtual void doRender(unsigned char* /* bytes */,
											  double /* x_pos */,
											  double /* y_pos */,
											  int /* width */,
											  int /* height */,
											  int /* bits_per_pixel */,
											  double /* zoom_x */,
											  double /* zoom_y */) const = 0;

  public:    virtual void undraw() const;
  protected: virtual void doUndraw(
					  int /*winRasterX*/, int /*winRasterY*/,
					  int /*winWidthX*/, int /*winHeightY*/) const = 0;

  // Accessors
public:
  virtual int getCategory() const { return -1; }

  int byteCount() const;
  int bytesPerRow() const;

  int width() const;
  int height() const;

  double getRasterX() const;
  double getRasterY() const;
  double getZoomX() const;
  double getZoomY() const;

  bool getUsingZoom() const;

  // Manipulators

  virtual void setCategory(int) {}

  void setRasterX(double val);
  void setRasterY(double val);
  void setZoomX(double val);
  void setZoomY(double val);

  void setUsingZoom(bool val);
  
protected:
  unsigned char* theBytes() const { return itsBytes; }

private:
  string itsFilename;
  double itsRasterX;
  double itsRasterY;
  double itsZoomX;
  double itsZoomY;
  bool itsUsingZoom;
  bool itsContrastFlip;
  bool itsVerticalFlip;

  unsigned char* itsBytes;
  int itsHeight;
  int itsWidth;
  int itsBitsPerPixel;
};

static const char vcid_bitmap_h[] = "$Header$";
#endif // !BITMAP_H_DEFINED
