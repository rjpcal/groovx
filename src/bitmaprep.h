///////////////////////////////////////////////////////////////////////
//
// bitmaprep.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 20:18:32 1999
// written: Fri Aug 10 12:19:09 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BITMAPREP_H_DEFINED
#define BITMAPREP_H_DEFINED

template <class T> class shared_ptr;

class BmapRenderer;

template <class V> class Point;
template <class V> class Rect;

namespace Gfx
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

  /// Non-virtual destructor implies this class is not polymorphic.
  ~BitmapRep();

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

  /** Implements the rendering operation. This function delegates the
      work to itsRenderer. */
  void render(Gfx::Canvas& canvas) const;

  ///////////////
  // accessors //
  ///////////////

  /// Conforms to the \c GrObj interface.
  Rect<double> grGetBoundingBox() const;

  /// Get the image's size (x-width, y-height) in pixels.
  Point<int> size() const;

  /// Get the (x,y) factors by which the bitmap will be scaled.
  Point<double> getZoom() const;

  /// Query whether zooming is currently to be used.
  bool getUsingZoom() const;

  //////////////////
  // manipulators //
  //////////////////

  /** Change the (x,y) factors by which the bitmap will be scaled. */
  void setZoom(Point<double> zoom);

  /// Change whether zooming will be used.
  void setUsingZoom(bool val);

private:
  BitmapRep(const BitmapRep&);
  BitmapRep& operator=(const BitmapRep&);

  class Impl;
  Impl* const itsImpl;
};

static const char vcid_bitmaprep_h[] = "$Header$";
#endif // !BITMAPREP_H_DEFINED
