///////////////////////////////////////////////////////////////////////
//
// bitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Sat Nov 20 20:00:51 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_H_DEFINED
#define BITMAP_H_DEFINED

#ifndef GROBJ_H_DEFINED
#include "grobj.h"
#endif

#ifndef RECT_H_DEFINED
#include "rect.h"
#endif

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * Bitmap is an abstract subclass of GrObj that manages bitmap data,
 * but does not implement a method for rendering the bitmap data to
 * the screen, since there is typically more than one way to do
 * this. Subclasses of Bitmap can implement a particular rendering
 * method by overriding doRender() and doUndraw() appropriately.
 *
 * @memo Abstract subclass of GrObj for rendering bitmaps.
 **/
///////////////////////////////////////////////////////////////////////

class Bitmap : public GrObj {
  // Creators
public:
  ///
  Bitmap();
  /** Create a bitmap from a graphics file. Currently the only
		supported file format is PBM (Portable BitMap). */
  Bitmap(const char* filename);
  /// Construct a Bitmap by deserializing from a stream.
  Bitmap(istream& is, IOFlag flag);

  private: void init();

public:
  ///
  virtual ~Bitmap();

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

  /////////////
  // actions //
  /////////////

public:
  ///
  void loadPbmFile(const char* filename);
  ///
  void writePbmFile(const char* filename) const;

  /** These routines grab pixels from a rectanglur area of the screen
		buffer into the Bitmap's pixel array. The coordinates of the
		rectangle may be specified in pixel values (ScreenRect) or in
		OpenGL coordinate values (WorldRect). */
  //@{
  ///
  void grabScreenRect(int left, int top, int right, int bottom);
  ///
  void grabScreenRect(const Rect<int>& rect);
  ///
  void grabWorldRect(double left, double top,
							double right, double bottom);
  ///
  void grabWorldRect(const Rect<double>& rect);
  //@}

  protected:
  /** This operation is a hook that is called after every time that
      the bitmap data change. The default implementation here is a
      no-op, but subclasses may override if they need to perform
      specific actions when that bitmap data change. */
  virtual void bytesChangeHook(unsigned char* /* theBytes */,
										 int /* width */,
										 int /* height */,
										 int /* bits_per_pixel */,
										 int /* byte_alignment */) {}

  public:
  /** Flips the luminance contrast of the bitmap data, in a way that
      may depend on the format of the bitmap data. */
  void flipContrast();

  protected:
  void doFlipContrast();

  public:
  /// Vertically inverts the image.
  void flipVertical();

  protected:
  void doFlipVertical();

  public:
  /** Centers the image so that its center coincides with the origin
      in the graphics environment when it is rendered. */
  void center();

  protected:
  /** Implements the GrObj rendering operation. This function
      delegates the work to doRender(); therefore, subclasses of
      Bitmap should not override grRender(), but should instead
      override doRender(). */
  virtual void grRender() const;

  protected:
  /** This is the operation that subclasses must implement to do the
		actual rendering of the bitmap data. All of the information
		needed to do the rendering is passed as arguments to the
		function. */
  virtual void doRender(unsigned char* /* bytes */,
											  double /* x_pos */,
											  double /* y_pos */,
											  int /* width */,
											  int /* height */,
											  int /* bits_per_pixel */,
											  int /* byte_alignment */,
											  double /* zoom_x */,
											  double /* zoom_y */) const = 0;

  public:
  /** Implements the GrObj undrawing operation. This function
      delegates the work to doUndraw(); therefore, subclasses of
      Bitmap should not override grUnRender(), but should instead
      override doUndraw(). */
  virtual void grUnRender() const;

  protected:
  /** This is the operation that subclasses should override if they
      need to provide a special unrendering method. A default no-op
      implementation is provided. */
  virtual void doUndraw(
					  int /*winRasterX*/, int /*winRasterY*/,
					  int /*winWidthX*/, int /*winHeightY*/) const;

  ///////////////
  // accessors //
  ///////////////

  protected:
  ///
  virtual bool grGetBoundingBox(Rect<double>& bounding_box,
														 int& border_pixels) const;

public:
  /// Get the number of bytes of image data.
  int byteCount() const;
  /// Get the number of bytes per row in the image data.
  int bytesPerRow() const;

  /// Get the image's width in pixels.
  int width() const;
  /// Get the image's height in pixels.
  int height() const;

  /** Manipulate the coordinates of the image's raster position (this
      specifies where the lower left corner of the image will
      fall). */
  //@{
  ///
  double getRasterX() const;
  ///
  double getRasterY() const;
  ///
  void setRasterX(double val);
  ///
  void setRasterY(double val);
  //@}

  /** Manipulate the zoom factors of the image. (NOTE: not all
      subclasses may be able to support zooming.) */
  //@{
  ///
  double getZoomX() const;
  ///
  double getZoomY() const;
  ///
  void setZoomX(double val);
  ///
  void setZoomY(double val);
  //@}

  /// Manipulate whether zooming should be used.
  //@{
  ///
  bool getUsingZoom() const;
  ///
  void setUsingZoom(bool val);
  //@}

protected:
  /// Return a pointer to the raw bitmap data.
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
