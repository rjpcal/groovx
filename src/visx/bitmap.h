///////////////////////////////////////////////////////////////////////
//
// bitmap.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 11:30:24 1999
// written: Wed Sep 25 19:01:02 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAP_H_DEFINED
#define BITMAP_H_DEFINED

#include "visx/grobj.h"

template <class T> class shared_ptr;

namespace Gfx
{
  template <class V> class Rect;
  template <class V> class Vec2;
}

class BitmapImpl;

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

class Bitmap : public GrObj
{
public:

  //////////////
  // creators //
  //////////////

  /// Create an empty bitmap.
  Bitmap(shared_ptr<BmapRenderer> renderer);

  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~Bitmap();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  /////////////
  // actions //
  /////////////

  /// Loads bitmap data from the image file at \a filename.
  /** The file must be in one of the supported image formats. */
  void loadImage(const char* filename);

  /// Queues the image file \a filename for loading.
  /** The image will not actually be retrieved from the file until it is
      needed. */
  void queueImage(const char* filename);

  /// Writes bitmap data to the file \a filename.
  /** The image file format is inferred from the given filename. */
  void saveImage(const char* filename) const;

  /// Grabs pixels from a rectangular area of the screen buffer.
  /** The pixels are loaded into the Bitmap's pixel array. The coordinates
      of the rectangle are specified in screen pixel coordinates. */
  void grabScreenRect(const Gfx::Rect<int>& rect);

  /// Grabs pixels from a rectangular area of the screen buffer.
  /** The pixels are loaded into the Bitmap's pixel array. The coordinates
      of the rectangle are specified in OpenGL coordinates. */
  void grabWorldRect(const Gfx::Rect<double>& rect);

  /// Flips the luminance contrast of the bitmap data.
  /** The actual math depends on the format of the bitmap data. The
      polarity of the contrast relative to its original value is computed
      and stored, so that it can be maintained throughout IO operations. */
  void flipContrast();

  /// Vertically inverts the image.
  /** The total number of vertical flips in the object's history is
      computed and stored, so that the proper orientation polarity can be
      maintained across IO operations. */
  void flipVertical();

  ///////////////
  // accessors //
  ///////////////

  /// Get the image's size (x-width, y-height) in pixels.
  Gfx::Vec2<int> size() const;

  /// Get the (x,y) factors by which the bitmap will be scaled.
  Gfx::Vec2<double> getZoom() const;

  /// Query whether zooming is currently to be used.
  bool getUsingZoom() const;

  /// Query whether the image data are purgeable.
  /** If the image data are purgeable, they will be unloaded after every
      render, and then re-queued. */
  bool isPurgeable() const;

  /// Get the name (if any) of the file associated with this bitmap image.
  const char* filename() const;

  //////////////////
  // manipulators //
  //////////////////

  /// Change the (x,y) factor by which the bitmap will be scaled.
  void setZoom(Gfx::Vec2<double> zoom);

  /// Change whether zooming will be used.
  void setUsingZoom(bool val);

  /// Change whether the image data are purgeable.
  void setPurgeable(bool val);

protected:
  /// Implements the \c GrObj rendering operation.
  /** The rendering is delegated to the \c BmapRenderer passed to the
      constructor. Therefore, subclasses of \c Bitmap should not override
      \c grRender(), but should instead provide a specialized \c
      BmapRenderer. */
  virtual void grRender(Gfx::Canvas& canvas) const;

  virtual Gfx::Rect<double> grGetBoundingBox(Gfx::Canvas& canvas) const;

private:
  Bitmap(const Bitmap&);
  Bitmap& operator=(const Bitmap&);

  BitmapImpl* const itsImpl;
};

static const char vcid_bitmap_h[] = "$Header$";
#endif // !BITMAP_H_DEFINED
