///////////////////////////////////////////////////////////////////////
//
// bmapdata.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jan 19 17:25:51 2000
// written: Mon Jan 20 12:56:21 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPDATA_H_DEFINED
#define BMAPDATA_H_DEFINED

#include "util/arrays.h"
#include "util/pointers.h"

namespace Gfx
{
  class BmapData;
  template <class V> class Vec2;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c BmapData handles the low-level details of manipulating bitmap
 * data. It keeps track of the width, height, bits-per-pixel, and
 * storage alignemnt requirements of a chunk of image data. In
 * addition, \c BmapData provides for lazy updates via the \c
 * UpdateFunc class. Clients may subclass \c UpdateFunc to implement
 * \c UpdateFunc::update(), then pass this type of object to \c
 * queueUpdate(). Then the \c update() function will be called only
 * when the bitmap data need to be accessed.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Gfx::BmapData
{
public:
  /// Nested class in allows \c BmapData objects to be updated lazily.
  class UpdateFunc
  {
  public:
    /// Virtual destructor ensures correct destruction of subclasses.
    virtual ~UpdateFunc();

    /** To be overridden by subclasses to provide a way to update the
        \c BmapData. */
    virtual void update(BmapData& update_me) = 0;
  };

  /// Specifies the order in which the rows are physically arranged in memory.
  enum RowOrder
    {
      TOP_FIRST,
      BOTTOM_FIRST
    };

  //---------------------------------------------------------------------
  //
  // Creators
  //
  //---------------------------------------------------------------------

  /// Construct with empty image data.
  BmapData();

  /// Construct with the given image data specifications.
  BmapData(const Gfx::Vec2<int>& extent, int bits_per_pixel, int byte_alignment);

  /// Copy constructor.
  BmapData(const BmapData& other);

  /// Assignment operator
  BmapData& operator=(const BmapData& other)
  {
    BmapData copy(other);
    this->swap(copy);
    return *this;
  }

  /// Destructor frees Impl*.
  ~BmapData();

  //---------------------------------------------------------------------
  //
  // Accessors
  //
  //---------------------------------------------------------------------

  /// Returns a pointer to the raw image data.
  unsigned char* bytesPtr() const;

  /// Returns a pointer to the beginning of the specified row.
  /** This takes into account whether the current RowOrder is TOP_FIRST or
      BOTTOM_FIRST. */
  unsigned char* rowPtr(unsigned int row) const;

  /// Returns a checksum of all the bytes in the image data.
  long int checkSum() const;

  /// Returns the bitmap's width in pixels.
  int width() const;

  /// Returns the bitmap's height in pixels.
  int height() const;

  /// Returns the bitmap's size (x-width, y-height) in pixels.
  Gfx::Vec2<int> size() const;

  /// Returns the number of bits used per pixel in the bitmap.
  int bitsPerPixel() const;

  /// Returns the byte alignment of the bitmap data.
  /** Each image row in the data will begin on a multiple of this number of
      bytes. */
  int byteAlignment() const;

  /// Returns the number of bytes used by the bitmap data.
  /** Some of these bytes may be 'filler bytes' needed to meet the storage
      alignment requirements. */
  unsigned int byteCount() const;

  /// Returns the number of bytes used per image row in the bitmap data.
  unsigned int bytesPerRow() const;

  /// Get the current row order.
  RowOrder rowOrder() const;

  //---------------------------------------------------------------------
  //
  // Manipulators
  //
  //---------------------------------------------------------------------

  /// Flips the image contrast of the bitmap data.
  void flipContrast();

  /// Flips the image vertically.
  void flipVertical();

  /// Clears the bitmap data, erasing the current image.
  void clear();

  /// Swaps the internal representation with that of \a other.
  void swap(BmapData& other);

  /// Queues the update given by \a updater.
  /** The \c update() function will be called only when the bitmap data
      must be accessed. */
  void queueUpdate(shared_ptr<UpdateFunc> updater) const;

  /// Forces any pending update to be called.
  void updateIfNeeded() const;

  /// Cancels any pending update.
  void clearQueuedUpdate() const;

  /// Set the row order, swapping rows around in memory if necessary.
  /** This is a logical "const" operation since the image being represented
      stays the same; only its representation changes. */
  void setRowOrder(RowOrder order) const;

  /// Specify the row order but leave the actual memory untouched.
  void specifyRowOrder(RowOrder order) const;

private:
  class Impl;
  Impl* rep;
};

static const char vcid_bmapdata_h[] = "$Header$";
#endif // !BMAPDATA_H_DEFINED
