///////////////////////////////////////////////////////////////////////
//
// bmapdata.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jan 20 00:37:03 2000
// written: Tue Oct  3 13:18:29 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPDATA_CC_DEFINED
#define BMAPDATA_CC_DEFINED

#include "bmapdata.h"

#include <cstring>				  // for memcpy

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace {

template <class T>
inline void swap(T& t1, T& t2)
  {
	 T t2_copy = t2;
	 t2 = t1;
	 t1 = t2_copy;
  }

}

///////////////////////////////////////////////////////////////////////
//
// BmapData::Impl definition
//
///////////////////////////////////////////////////////////////////////

class BmapData::Impl {
public:
  Impl(int width, int height, int bits_per_pixel, int byte_alignment) :
	 itsWidth(width),
	 itsHeight(height),
	 itsBitsPerPixel(bits_per_pixel),
	 itsByteAlignment(byte_alignment),
	 itsBytes(),
	 itsUpdater(0)
	 {}
	 
  int itsWidth;
  int itsHeight;
  int itsBitsPerPixel;
  int itsByteAlignment;
  dynamic_block<unsigned char> itsBytes;

  mutable shared_ptr<UpdateFunc> itsUpdater;
};

///////////////////////////////////////////////////////////////////////
//
// BmapData member definitions
//
///////////////////////////////////////////////////////////////////////

BmapData::UpdateFunc::~UpdateFunc() {}

BmapData::BmapData() :
  itsImpl(new Impl(0, 0, 1, 1))
{
DOTRACE("BmapData::BmapData");
  clear();
}

BmapData::BmapData(int width, int height,
						 int bits_per_pixel, int byte_alignment) :
  itsImpl(new Impl(width, height, bits_per_pixel, byte_alignment))
{
DOTRACE("BmapData::BmapData");
  int num_bytes = (width/8 + 1) * height  + 1;
  Assert(num_bytes > 0);
  itsImpl->itsBytes.resize( num_bytes );
}

BmapData::~BmapData() {
  delete itsImpl;
}

unsigned char* BmapData::bytesPtr() const {
DOTRACE("BmapData::bytesPtr");
  updateIfNeeded(); 
  return const_cast<unsigned char*>(&(itsImpl->itsBytes[0]));
}

dynamic_block<unsigned char>& BmapData::bytesVec() {
  updateIfNeeded(); 
  return itsImpl->itsBytes;
}

const dynamic_block<unsigned char>& BmapData::bytesVec() const {
  updateIfNeeded(); 
  return itsImpl->itsBytes;
}

int BmapData::width() const {
DOTRACE("BmapData::width");
  updateIfNeeded(); 
  return itsImpl->itsWidth;
}

int BmapData::height() const {
DOTRACE("BmapData::height");
  updateIfNeeded(); 
  return itsImpl->itsHeight; 
}

int BmapData::bitsPerPixel() const {
DOTRACE("BmapData::bitsPerPixel");
  return itsImpl->itsBitsPerPixel;
}

int BmapData::byteAlignment() const {
DOTRACE("BmapData::byteAlignment");
  return itsImpl->itsByteAlignment;
}

int BmapData::byteCount() const {
DOTRACE("BmapData::byteCount");
  updateIfNeeded(); 
  return bytesPerRow() * itsImpl->itsHeight;
}

int BmapData::bytesPerRow() const {
DOTRACE("BmapData::bytesPerRow");
  updateIfNeeded(); 
  return ( (itsImpl->itsWidth*itsImpl->itsBitsPerPixel - 1)/8 + 1 );
}

void BmapData::flipContrast() {
DOTRACE("BmapData::flipContrast");
  updateIfNeeded(); 

  int num_bytes = itsImpl->itsBytes.size();

  // In this case we want to flip each bit
  if (itsImpl->itsBitsPerPixel == 1) {
	 for (int i = 0; i < num_bytes; ++i) {
		itsImpl->itsBytes[i] =
		  static_cast<unsigned char>(0xff ^ itsImpl->itsBytes[i]);
	 }
  }
  // In this case we want to reflect the value of each byte around the
  // middle value, 127.5
  else {
	 for (int i = 0; i < num_bytes; ++i) {
		itsImpl->itsBytes[i] =
		  static_cast<unsigned char>(0xff - itsImpl->itsBytes[i]);
	 }
  }
}

void BmapData::flipVertical() {
DOTRACE("BmapData::flipVertical");
  updateIfNeeded(); 

  int bytes_per_row = bytesPerRow();
  int num_bytes = byteCount();
  
  dynamic_block<unsigned char> new_bytes(num_bytes);
  
  for (int row = 0; row < itsImpl->itsHeight; ++row) {
	 int new_row = (itsImpl->itsHeight-1)-row;
	 memcpy(static_cast<void*> (&(new_bytes[new_row * bytes_per_row])),
			  static_cast<void*> (&(itsImpl->itsBytes [row     * bytes_per_row])),
			  bytes_per_row);
  }
  
  itsImpl->itsBytes.swap(new_bytes);
}

void BmapData::clear() {
DOTRACE("BmapData::clear");
  itsImpl->itsBytes.resize(1);
  itsImpl->itsHeight = 1;
  itsImpl->itsWidth = 1;
  itsImpl->itsBitsPerPixel = 1;
  itsImpl->itsByteAlignment = 1;

  itsImpl->itsUpdater.reset(0);
}

void BmapData::swap(BmapData& other) {
DOTRACE("BmapData::swap");
  updateIfNeeded();
 
  itsImpl->itsBytes.swap(other.itsImpl->itsBytes);

  ::swap(itsImpl->itsWidth, other.itsImpl->itsWidth);
  ::swap(itsImpl->itsHeight, other.itsImpl->itsHeight);
  ::swap(itsImpl->itsBitsPerPixel, other.itsImpl->itsBitsPerPixel);
  ::swap(itsImpl->itsByteAlignment, other.itsImpl->itsByteAlignment);
}

void BmapData::swap(dynamic_block<unsigned char>& bytes, int& width, int& height,
						  int& bits_per_pixel, int& byte_alignment) {
DOTRACE("BmapData::swap");
  updateIfNeeded();

  itsImpl->itsBytes.swap(bytes);

  ::swap(itsImpl->itsWidth, width);
  ::swap(itsImpl->itsHeight, height);
  ::swap(itsImpl->itsBitsPerPixel, bits_per_pixel);
  ::swap(itsImpl->itsByteAlignment, byte_alignment);
}

void BmapData::queueUpdate(shared_ptr<UpdateFunc> updater) const {
DOTRACE("BmapData::queueUpdate");
  itsImpl->itsUpdater = updater;
}

void BmapData::updateIfNeeded() const {
DOTRACE("BmapData::updateIfNeeded");
  if (itsImpl->itsUpdater.get() != 0)
	 {
		// This steals the updater from itsImpl->itsUpdater, so that we can
		// avoid endless recursion if updateIfNeeded is called again
		// as part of the updating.
		shared_ptr<UpdateFunc> tempUpdater(itsImpl->itsUpdater);
		itsImpl->itsUpdater.reset(0);

		tempUpdater->update(const_cast<BmapData&>(*this));
	 }
}

void BmapData::clearQueuedUpdate() const {
DOTRACE("BmapData::clearQueuedUpdate");
  itsImpl->itsUpdater.reset(0);
}

static const char vcid_bmapdata_cc[] = "$Header$";
#endif // !BMAPDATA_CC_DEFINED
