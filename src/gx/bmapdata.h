///////////////////////////////////////////////////////////////////////
//
// bmapdata.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jan 19 17:25:51 2000
// written: Wed Aug  8 10:37:37 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPDATA_H_DEFINED
#define BMAPDATA_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ARRAYS_H_DEFINED)
#include "util/arrays.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

template <class V> class Point;

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

class BmapData {
private:
  BmapData(const BmapData& other); // not implemented
  BmapData& operator=(const BmapData& other); // not implemented

public:
  /// Nested class in allows \c BmapData objects to be updated lazily.
  class UpdateFunc {
  public:
    /// Virtual destructor ensures correct destruction of subclasses.
    virtual ~UpdateFunc();

    /** To be overridden by subclasses to provide a way to update the
        \c BmapData. */
    virtual void update(BmapData& update_me) = 0;
  };

  //---------------------------------------------------------------------
  //
  // Creators
  //
  //---------------------------------------------------------------------

  /// Construct with empty image data.
  BmapData();

  /// Construct with the given image data specifications.
  BmapData(const Point<int>& extent, int bits_per_pixel, int byte_alignment);

  /// Destructor frees Impl*.
  ~BmapData();

  //---------------------------------------------------------------------
  //
  // Accessors
  //
  //---------------------------------------------------------------------

  /// Returns a pointer to the raw image data.
  unsigned char* bytesPtr() const;

  /// Returns a reference to the container of the raw image data.
  dynamic_block<unsigned char>& bytesVec();

  /// Returns a reference to the container of the raw image data.
  const dynamic_block<unsigned char>& bytesVec() const;

  /// Returns the bitmap's width in pixels.
  int width() const;

  /// Returns the bitmap's height in pixels.
  int height() const;

  /// Returns the bitmap's extent (x-width, y-height) in pixels.
  Point<int> extent() const;

  /// Returns the number of bits used per pixel in the bitmap.
  int bitsPerPixel() const;

  /** Returns the byte alignment of the bitmap data. Each image row in
      the data will begin on a multiple of this number of bytes. */
  int byteAlignment() const;

  /** Returns the number of bytes used by the bitmap data. Some of
      these bytes may be 'filler bytes' needed to meet the storage
      alignment requirements. */
  int byteCount() const;

  /** Returns the number of bytes used per image row in the bitmap data. */
  int bytesPerRow() const;


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

  /// Swaps the internal representation with the given arguments.
  void swap(dynamic_block<unsigned char>& bytes, int& width, int& height,
            int& bits_per_pixel, int& byte_alignment);

  /** Queues the update given by \a updater. The \c update() function
      will be called only when the bitmap data must be accessed. */
  void queueUpdate(shared_ptr<UpdateFunc> updater) const;


  /// Forces any pending update to be called.
  void updateIfNeeded() const;

  /// Cancels any pending update.
  void clearQueuedUpdate() const;


private:
  class Impl;
  Impl* const itsImpl;
};

static const char vcid_bmapdata_h[] = "$Header$";
#endif // !BMAPDATA_H_DEFINED
