///////////////////////////////////////////////////////////////////////
//
// gxpixmap.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jun 15 11:30:24 1999 (as bitmap.h)
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GXPIXMAP_H_DEFINED
#define GXPIXMAP_H_DEFINED

#include "gfx/gxshapekit.h"

template <class T> class shared_ptr;

namespace Gfx
{
  class BmapData;
  template <class V> class Rect;
  template <class V> class Vec2;
}

class GxPixmapImpl;

//  ###################################################################
//  ===================================================================

/// \c GxPixmap is a subclass of \c GxShapeKit represents bitmap images.

class GxPixmap : public GxShapeKit
{
  //////////////
  // creators //
  //////////////

protected:
  /// Create an empty bitmap.
  GxPixmap();

public:
  /// Default creator.
  static GxPixmap* make();

  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~GxPixmap() throw();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader& reader);
  virtual void writeTo(IO::Writer& writer) const;

  /////////////
  // actions //
  /////////////

  /// Loads bitmap data from the image file at \a filename.
  /** The file must be in one of the supported image formats. */
  void loadImage(const char* filename);

  /// Reload bitmap data from the image file most recently loaded.
  /** This has the effect of reverting any changes that might have been
      made to the image data since loading. The underlying image file is
      not affected. */
  void reload();

  /// Queues the image file \a filename for loading.
  /** The image will not actually be retrieved from the file until it is
      needed. */
  void queueImage(const char* filename);

  /// Writes bitmap data to the file \a filename.
  /** The image file format is inferred from the given filename. */
  void saveImage(const char* filename) const;

  /// Grabs pixels from a rectangular area of the screen buffer.
  /** The pixels are loaded into the GxPixmap's pixel array. The rectangle
      is specified in screen coordinates. */
  void grabScreenRect(const Gfx::Rect<int>& rect);

  /// Grabs pixels from a rectangular area of the screen buffer.
  /** The pixels are loaded into the GxPixmap's pixel array. The rectangle
      is specified in world coordinates. */
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

  /// Query whether the image will be rendered as 1-bit bitmap.
  bool getAsBitmap() const;

  /// Get a checksum of all the bytes in the image data.
  long int checkSum() const;

  //////////////////
  // manipulators //
  //////////////////

  /// Mutable access to internal bitmap data representation.
  Gfx::BmapData& data();

  /// Change the (x,y) factor by which the bitmap will be scaled.
  void setZoom(Gfx::Vec2<double> zoom);

  /// Zoom to a given size.
  void zoomTo(Gfx::Vec2<int> sz);

  /// Change whether zooming will be used.
  void setUsingZoom(bool val);

  /// Change whether the image data are purgeable.
  void setPurgeable(bool val);

  /// Change whether the image will be rendered as a 1-bit bitmap.
  void setAsBitmap(bool val);

  void scramble(int numsubcols, int numsubrows, int seed,
                bool allowMoveSubparts = true,
                bool allowFlipLeftRight = true,
                bool allowFlipTopBottom = true);

protected:
  /// Implements the \c GxShapeKit rendering operation.
  virtual void grRender(Gfx::Canvas& canvas) const;

  virtual void grGetBoundingBox(Gfx::Bbox& bbox) const;

private:
  GxPixmap(const GxPixmap&);
  GxPixmap& operator=(const GxPixmap&);

  GxPixmapImpl* const rep;
};

static const char vcid_gxpixmap_h[] = "$Header$";
#endif // !GXPIXMAP_H_DEFINED
