///////////////////////////////////////////////////////////////////////
//
// bmapdata.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jan 19 17:25:51 2000
// written: Wed Feb 16 08:33:20 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPDATA_H_DEFINED
#define BMAPDATA_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef MEMORY_DEFINED
#include <memory>
#define MEMORY_DEFINED
#endif

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
  BmapData() { clear(); }

  /// Construct with the given image data specifications.
  BmapData(int width, int height, int bits_per_pixel, int byte_alignment);


  //---------------------------------------------------------------------
  //
  // Accessors
  //
  //---------------------------------------------------------------------

  /// Returns a pointer to the raw image data.
  unsigned char* bytesPtr() const;

  /// Returns a reference to the container of the raw image data.
  vector<unsigned char>& bytesVec();

  /// Returns a reference to the container of the raw image data.
  const vector<unsigned char>& bytesVec() const;

  /// Returns the bitmap's width in pixels.
  int width() const;

  /// Returns the bitmap's height in pixels.
  int height() const;

  /// Returns the number of bits used per pixel in the bitmap.
  int bitsPerPixel() const { return itsBitsPerPixel; }

  /** Returns the byte alignment of the bitmap data. Each image row in
      the data will begin on a multiple of this number of bytes. */
  int byteAlignment() const { return itsByteAlignment; }

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
  void swap(vector<unsigned char>& bytes, int& width, int& height,
				int& bits_per_pixel, int& byte_alignment);

  /** Queues the update given by \a updater. The \c update() function
      will be called only when the bitmap data must be accessed. */
  void queueUpdate(auto_ptr<UpdateFunc> updater) const
	 { itsUpdater = updater; }

  
  /// Forces any pending update to be called.
  void updateIfNeeded() const
	 {
		if (itsUpdater.get() != 0)
		  {
			 // This steals the updater from itsUpdater, so that we can
			 // avoid endless recursion if updateIfNeeded is called again
			 // as part of the updating.
			 auto_ptr<UpdateFunc> tempUpdater(itsUpdater);

			 tempUpdater->update(const_cast<BmapData&>(*this));
		  }
	 }

  /// Cancels any pending update.
  void clearQueuedUpdate() const
	 { itsUpdater.reset(0); }


private:

  int itsWidth;
  int itsHeight;
  int itsBitsPerPixel;
  int itsByteAlignment;
  vector<unsigned char> itsBytes;

  mutable auto_ptr<UpdateFunc> itsUpdater;
};

static const char vcid_bmapdata_h[] = "$Header$";
#endif // !BMAPDATA_H_DEFINED
