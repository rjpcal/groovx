///////////////////////////////////////////////////////////////////////
//
// bitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 15 11:30:24 1999
// written: Fri Jan 14 17:03:23 2000
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

template <class V> class Rect;

class BitmapRep;

class BmapRenderer;

///////////////////////////////////////////////////////////////////////
/**
 *
 * Bitmap is an abstract subclass of GrObj that manages bitmap data,
 * but does not implement a method for rendering the bitmap data to
 * the screen, since there is typically more than one way to do
 * this. Subclasses of Bitmap can implement a particular rendering
 * method by overriding doRender() and doUndraw() appropriately.
 *
 * @short Abstract subclass of GrObj for rendering bitmaps.
 **/
///////////////////////////////////////////////////////////////////////

class Bitmap : public GrObj {
  // Creators
public:
  ///
  Bitmap(BmapRenderer* renderer);
  /** Create a bitmap from a graphics file. Currently the only
		supported file format is PBM (Portable BitMap). */
  Bitmap(BmapRenderer* renderer, const char* filename);
  /// Construct a Bitmap by deserializing from a stream.
  Bitmap(BmapRenderer* renderer, istream& is, IOFlag flag);

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
  void loadPbmFile(istream& is);
  ///
  void loadPbmGzFile(const char* filename);
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
      delegates the work to itsRenderer; therefore, subclasses of
      Bitmap should not override grRender(), but should instead
      provide a specialized renderer. */
  virtual void grRender(Canvas& canvas) const;

  /** Implements the GrObj undrawing operation. This function
      delegates the work to itsRenderer; therefore, subclasses of
      Bitmap should not override grUnRender(), but should instead
      provide a specialized renderer. */
  virtual void grUnRender(Canvas& canvas) const;

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

private:
  BitmapRep* const itsImpl;
};

static const char vcid_bitmap_h[] = "$Header$";
#endif // !BITMAP_H_DEFINED
