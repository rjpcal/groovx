///////////////////////////////////////////////////////////////////////
//
// bitmap.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:30:24 1999
// written: Thu Aug  9 18:01:36 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_H_DEFINED
#define BITMAP_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GROBJ_H_DEFINED)
#include "grobj.h"
#endif

template <class T> class shared_ptr;

template <class V> class Point;
template <class V> class Rect;

class BitmapRep;

class BmapRenderer;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Bitmap is an subclass of \c GrObj that manages bitmap data, but
 * does not implement a method for rendering the bitmap data to the
 * screen, since there is typically more than one way to do this. A
 * rendering method must be provided by passing a suitable \c
 * BmapRenderer to a \c Bitmap constructor.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Bitmap : public GrObj {
  // Creators
public:
  /// Create an empty bitmap.
  Bitmap(shared_ptr<BmapRenderer> renderer);

  /** Create a bitmap from a graphics file. Currently the only
      supported file format is PBM (Portable BitMap). */
  Bitmap(shared_ptr<BmapRenderer> renderer, const char* filename);

public:
  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~Bitmap();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  /////////////
  // actions //
  /////////////

public:
  /// Loads PBM bitmap data from the PBM file at \a filename.
  void loadPbmFile(const char* filename);

  /// Writes PBM bitmap data to the file \a filename.
  void savePbmFile(const char* filename) const;

  /** Grabs pixels from a rectangulur area of the screen buffer into
      the Bitmap's pixel array. The coordinates of the rectangle are
      specified in pixel values. */
  void grabScreenRect(const Rect<int>& rect);

  /** Grabs pixels from a rectangulur area of the screen buffer into
      the Bitmap's pixel array. The coordinates of the rectangle are
      specified in OpenGL coordinates. */
  void grabWorldRect(const Rect<double>& rect);

  /** Flips the luminance contrast of the bitmap data, in a way that
      may depend on the format of the bitmap data. The polarity of the
      contrast relative to its original value is computed and stored,
      so that it can be maintained throughout IO operations. */
  void flipContrast();

  /** Vertically inverts the image. The polarity of the orientation
      relative to its original value is computed and stored, so that
      it can be maintained throughout IO operations. */
  void flipVertical();

  /** Centers the image so that its center coincides with the origin
      in the graphics environment when it is rendered. */
  void center();

protected:
  /** Implements the \c GrObj rendering operation. The rendering is
      delegated to the \c BmapRenderer passed to the
      constructor. Therefore, subclasses of \c Bitmap should not
      override \c grRender(), but should instead provide a specialized
      \c BmapRenderer. */
  virtual void grRender(GWT::Canvas& canvas, DrawMode mode) const;

  ///////////////
  // accessors //
  ///////////////

protected:
  virtual Rect<double> grGetBoundingBox() const;

public:
  /// Get the number of bytes of image data.
  int byteCount() const;

  /// Get the number of bytes per row in the image data.
  int bytesPerRow() const;

  /// Get the image's size (x-width, y-height) in pixels.
  Point<int> size() const;

  /// Get the location of the image's lower left corner.
  Point<double> getRasterPos() const;

  /// Get the (x,y) factors by which the bitmap will be scaled.
  Point<double> getZoom() const;

  /// Query whether zooming is currently to be used.
  bool getUsingZoom() const;

  //////////////////
  // manipulators //
  //////////////////

  /// Change the location of the image's lower left corner to \a pos.
  void setRasterPos(Point<double> pos);

  /// Change the (x,y) factor by which the bitmap will be scaled.
  void setZoom(Point<double> zoom);

  /// Change whether zooming will be used.
  void setUsingZoom(bool val);

private:
  Bitmap(const Bitmap&);
  Bitmap& operator=(const Bitmap&);

  BitmapRep* const itsImpl;
};

static const char vcid_bitmap_h[] = "$Header$";
#endif // !BITMAP_H_DEFINED
