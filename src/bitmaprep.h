///////////////////////////////////////////////////////////////////////
//
// bitmaprep.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 20:18:32 1999
// written: Wed Aug  8 10:45:18 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPREP_H_DEFINED
#define BITMAPREP_H_DEFINED

#ifdef PRESTANDARD_IOSTREAMS
class istream;
class ostream;
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#endif

template <class T> class shared_ptr;

class BmapRenderer;

template <class V> class Point;
template <class V> class Rect;

namespace GWT
{
  class Canvas;
}

namespace IO
{
  class Reader;
  class Writer;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c BitmapRep maintains data associated with a bitmap. \c BitmapRep
 * loosely implements the \c GrObj and \c IO interfaces, but does not
 * actually inherit from these classes. In fact, \c BitmapRep is used
 * as an implementation class for \c Bitmap, which delegates most of
 * its functionality to \c BitmapRep. \c BitmapRep objects must be
 * initialized with a \c BmapRenderer object, which handles the actual
 * rendering of the bitmao data onto the screen.
 *
 **/
///////////////////////////////////////////////////////////////////////

class BitmapRep {
public:
  /// Construct with empty bitmap data.
  BitmapRep(shared_ptr<BmapRenderer> renderer);

  /// Construct and load bitmap data from \a filename.
  BitmapRep(shared_ptr<BmapRenderer> renderer, const char* filename);

  /// Non-virtual destructor implies this class is not polymorphic.
  ~BitmapRep();

private:
  void init();

public:

  /// Conforms to the \c IO interface.
  void readFrom(IO::Reader* reader);
  /// Conforms to the \c IO interface.
  void writeTo(IO::Writer* writer) const;

  /////////////
  // actions //
  /////////////

public:
  /// Loads PBM bitmap data from the PBM file \a filename.
  void loadPbmFile(const char* filename);

  /** Queues the PBM file \a filename for loading. The PBM bitmap data
      will not actually be retrieved from the file until it is
      needed. */
  void queuePbmFile(const char* filename);

  /// Loads PBM bitmap data from an STD_IO::istream.
  void loadPbmData(STD_IO::istream& is);

  /// Writes PBM bitmap data to the file \a filename.
  void writePbmFile(const char* filename) const;

  /** Grabs pixels from a rectangulur area of the screen buffer into
      the Bitmap's pixel array. The coordinates of the rectangle are
      specified in pixel values. */
  void grabScreenRect(int left, int top, int right, int bottom);

  /** Grabs pixels from a rectangulur area of the screen buffer into
      the Bitmap's pixel array. The coordinates of the rectangle are
      specified in pixel values. */
  void grabScreenRect(const Rect<int>& rect);

  /** Grabs pixels from a rectangulur area of the screen buffer into
      the Bitmap's pixel array. The coordinates of the rectangle are
      specified in OpenGL coordinates. */
  void grabWorldRect(double left, double top,
                     double right, double bottom);

  /** Grabs pixels from a rectangulur area of the screen buffer into
      the Bitmap's pixel array. The coordinates of the rectangle are
      specified in OpenGL coordinates. */
  void grabWorldRect(const Rect<double>& rect);

  /** Flips the luminance contrast of the bitmap data, in a way that
      may depend on the format of the bitmap data. The polarity of the
      contrast relative to its original value is computed and stored,
      so that it can be maintained throughout IO operations. */
  void flipContrast();

  /// Does the actual bit-twiddling to flip the contrast.
  void doFlipContrast();

  /** Vertically inverts the image. The polarity of the orientation
      relative to its original value is computed and stored, so that
      it can be maintained throughout IO operations. */
  void flipVertical();

  /// Does the actual bit-twiddling to vertically flip the image.
  void doFlipVertical();

  /** Centers the image so that its center coincides with the origin
      in the graphics environment when it is rendered. */
  void center();

  /** Implements the rendering operation. This function delegates the
      work to itsRenderer. */
  void render(GWT::Canvas& canvas) const;

  /** Implements the unrendering operation. This function delegates the
      work to itsRenderer. */
 void unRender(GWT::Canvas& canvas) const;

  ///////////////
  // accessors //
  ///////////////

  /// Conforms to the \c GrObj interface.
  virtual Rect<double> grGetBoundingBox() const;

  /// Get the number of bytes of image data.
  int byteCount() const;

  /// Get the number of bytes per row in the image data.
  int bytesPerRow() const;

  /// Get the image's width in pixels.
  int width() const;

  /// Get the image's height in pixels.
  int height() const;

  /// Get the image's extent (x-width, y-height) in pixels.
  Point<int> extent() const;

  /// Get the x value of the location of the image's lower left corner.
  double getRasterX() const;

  /// Get the y value of the location of the image's lower left corner.
  double getRasterY() const;

  /** Change the x value of the location of the image's lower left
      corner to \a val. */
  void setRasterX(double val);

  /** Change the y value of the location of the image's lower left
      corner to \a val. */
  void setRasterY(double val);

  /// Get the factor by which the bitmap will be scaled in the x direction.
  double getZoomX() const;

  /// Get the factor by which the bitmap will be scaled in the y direction.
  double getZoomY() const;

  /** Change the factor by which the bitmap will be scaled in the x
      direction to \a val. */
  void setZoomX(double val);

  /** Change the factor by which the bitmap will be scaled in the y
      direction to \a val. */
  void setZoomY(double val);

  /// Query whether zooming is currently to be used.
  bool getUsingZoom() const;

  /// Change whether zooming will be used.
  void setUsingZoom(bool val);

private:
  void clearBytes();

  BitmapRep(const BitmapRep&);
  BitmapRep& operator=(const BitmapRep&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_bitmaprep_h[] = "$Header$";
#endif // !BITMAPREP_H_DEFINED
