///////////////////////////////////////////////////////////////////////
//
// bmapdata.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jan 19 17:25:51 2000
// written: Thu Jan 20 01:51:46 2000
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

class BmapData {
private:
  BmapData(const BmapData& other); // not implemented
  BmapData& operator=(const BmapData& other); // not implemented

public:
  class UpdateFunc {
  public:
	 virtual ~UpdateFunc();
	 virtual void update(BmapData& update_me) = 0;
  };

  BmapData() { clear(); }

  BmapData(int width, int height, int bits_per_pixel, int byte_alignment);

  // Accessors
  unsigned char* bytesPtr() const;

  vector<unsigned char>& bytesVec();
  const vector<unsigned char>& bytesVec() const;

  int width() const;
  int height() const;
  int bitsPerPixel() const { return itsBitsPerPixel; }
  int byteAlignment() const { return itsByteAlignment; }

  int byteCount() const;
  int bytesPerRow() const;

  // Manipulators
  void flipContrast();

  void flipVertical();

  void clear();

  void swap(BmapData& other);

  void swap(vector<unsigned char>& bytes, int& width, int& height,
				int& bits_per_pixel, int& byte_alignment);

  // Updating

  void queueUpdate(auto_ptr<UpdateFunc> updater) const
	 { itsUpdater = updater; }

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
