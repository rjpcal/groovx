///////////////////////////////////////////////////////////////////////
//
// bitmaprep.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec  1 20:18:32 1999
// written: Thu Jan 20 01:28:23 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPREP_H_DEFINED
#define BITMAPREP_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef BMAPDATA_H_DEFINED
#include "bmapdata.h"
#endif

class BmapRenderer;
class Canvas;
class istream;
class ostream;
class Reader;
class Writer;
template <class V> class Rect;

///////////////////////////////////////////////////////////////////////
/**
 *
 * BitmapRep
 *
 * @short A class that maintains data associated with a bitmap.
 **/
///////////////////////////////////////////////////////////////////////

class BitmapRep {
public:
  ///
  BitmapRep(BmapRenderer* renderer) :
	 itsRenderer(renderer)
	 { init(); }

  ///
  BitmapRep(BmapRenderer* renderer, const char* filename) :
	 itsRenderer(renderer),
	 itsFilename(filename)
  {
	 init();
	 loadPbmFile(filename);
  }

  ///
  ~BitmapRep() {}

private:
  void init();

public:

  ///
  void serialize(ostream& os, int flag) const;
  ///
  void deserialize(istream& is, int flag);

  ///
  int charCount() const;

  ///
  void readFrom(Reader* reader);
  ///
  void writeTo(Writer* writer) const;

  /////////////
  // actions //
  /////////////

public:
  ///
  void loadPbmFile(const char* filename);
  /** Queues a pbm file for loading. The file will not actually be
      loaded until such time as the data from the file is needed. */
  void queuePbmFile(const char* filename);
  ///
  void loadPbmFile(istream& is);
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

  /** Flips the luminance contrast of the bitmap data, in a way that
      may depend on the format of the bitmap data. */
  void flipContrast();

  void doFlipContrast() { itsData.flipVertical(); }

  /// Vertically inverts the image.
  void flipVertical();

  void doFlipVertical() { itsData.flipVertical(); }

  /** Centers the image so that its center coincides with the origin
      in the graphics environment when it is rendered. */
  void center();

  /** Implements the rendering operation. This function delegates the
      work to itsRenderer. */
  void grRender(Canvas& canvas) const;

  /** Implements the undrawing operation. This function delegates the
      work to itsRenderer. */
  void grUnRender(Canvas& canvas) const;

  ///////////////
  // accessors //
  ///////////////

  ///
  bool grGetBoundingBox(Rect<double>& bounding_box,
								int& border_pixels) const;

  /// Get the number of bytes of image data.
  int byteCount() const
	 { return itsData.byteCount(); }
  /// Get the number of bytes per row in the image data.
  int bytesPerRow() const
	 { return itsData.bytesPerRow(); }

  /// Get the image's width in pixels.
  int width() const
	 { return itsData.width(); }
  /// Get the image's height in pixels.
  int height() const
	 { return itsData.height(); }

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
  void clearBytes();

  BmapRenderer* itsRenderer;

  string itsFilename;
  double itsRasterX;
  double itsRasterY;
  double itsZoomX;
  double itsZoomY;
  bool itsUsingZoom;
  bool itsContrastFlip;
  bool itsVerticalFlip;

  BmapData itsData;
};

static const char vcid_bitmaprep_h[] = "$Header$";
#endif // !BITMAPREP_H_DEFINED
