///////////////////////////////////////////////////////////////////////
//
// bitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Mon Sep 20 16:36:45 1999
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

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
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

  /////////////
  // actions //
  /////////////

  public:    void loadPbmFile(const char* filename);

  public:    void grabScreenRect(int left, int top, int right, int bottom);
  public:    void grabWorldRect(double left, double top,
										  double right, double bottom);
  // These routines grab pixels from a rectanglur area of the screen
  // buffer into the Bitmap's pixel array. Two functions are provided
  // so that the coordinates of the rectangle may be specified in
  // pixel values, or in OpenGL coordinate values.

  protected: virtual void bytesChangeHook(unsigned char* /* theBytes */,
														int /* width */,
														int /* height */,
														int /* bits_per_pixel */,
														int /* byte_alignment */) {}

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
											  int /* byte_alignment */,
											  double /* zoom_x */,
											  double /* zoom_y */) const = 0;

  public:    virtual void grUnRender() const;
  protected: virtual void doUndraw(
					  int /*winRasterX*/, int /*winRasterY*/,
					  int /*winWidthX*/, int /*winHeightY*/) const;

  ///////////////
  // accessors //
  ///////////////

public:
  virtual bool getBoundingBox(double& left, double& top,
										double& right, double& bottom) const;

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

  //////////////////
  // manipulators //
  //////////////////

  virtual void setCategory(int) {}

  void setRasterX(double val);
  void setRasterY(double val);
  void setZoomX(double val);
  void setZoomY(double val);

  void setUsingZoom(bool val);
  
protected:
  unsigned char* theBytes() const
	 { return const_cast<unsigned char*>(&(itsBytes[0])); }

private:
  string itsFilename;
  double itsRasterX;
  double itsRasterY;
  double itsZoomX;
  double itsZoomY;
  bool itsUsingZoom;
  bool itsContrastFlip;
  bool itsVerticalFlip;

  vector<unsigned char> itsBytes;
  int itsHeight;
  int itsWidth;
  int itsBitsPerPixel;
  int itsByteAlignment;
};

static const char vcid_bitmap_h[] = "$Header$";
#endif // !BITMAP_H_DEFINED
