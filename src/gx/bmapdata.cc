///////////////////////////////////////////////////////////////////////
//
// bmapdata.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jan 20 00:37:03 2000
// written: Sun Mar  5 18:53:23 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPDATA_CC_DEFINED
#define BMAPDATA_CC_DEFINED

#include "bmapdata.h"

#include <algorithm>
#include <cstring>				  // for memcpy
#include <memory>
#include <vector>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

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
  vector<unsigned char> itsBytes;

  mutable auto_ptr<UpdateFunc> itsUpdater;
}

///////////////////////////////////////////////////////////////////////
//
// BmapData member definitions
//
///////////////////////////////////////////////////////////////////////

BmapData::UpdateFunc::~UpdateFunc() {}

BmapData::BmapData() :
  itsImpl(new Impl(0, 0, 1, 1,))
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
  itsBytes.resize( num_bytes );
}

BmapData::~BmapData() {
  delete itsImpl;
}

unsigned char* BmapData::bytesPtr() const {
DOTRACE("BmapData::bytesPtr");
  updateIfNeeded(); 
  return const_cast<unsigned char*>(&(itsImpl->itsBytes[0]));
}

vector<unsigned char>& BmapData::bytesVec() {
  updateIfNeeded(); 
  return itsImpl->itsBytes;
}

const vector<unsigned char>& BmapData::bytesVec() const {
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
		itsImpl->itsBytes[i] ^= 0xff;
	 }
  }
  // In this case we want to reflect the value of each byte around the
  // middle value, 127.5
  else {
	 for (int i = 0; i < num_bytes; ++i) {
		itsImpl->itsBytes[i] = 0xff - itsImpl->itsBytes[i];
	 }
  }
}

void BmapData::flipVertical() {
DOTRACE("BmapData::flipVertical");
  updateIfNeeded(); 

  int bytes_per_row = bytesPerRow();
  int num_bytes = byteCount();
  
  vector<unsigned char> new_bytes(num_bytes);
  
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

#ifdef ACC_COMPILER
#define std
#endif
  std::swap(itsImpl->itsWidth, other.itsImpl->itsWidth);
  std::swap(itsImpl->itsHeight, other.itsImpl->itsHeight);
  std::swap(itsImpl->itsBitsPerPixel, other.itsImpl->itsBitsPerPixel);
  std::swap(itsImpl->itsByteAlignment, other.itsImpl->itsByteAlignment);
#ifdef ACC_COMPILER
#undef std
#endif
}

void BmapData::swap(vector<unsigned char>& bytes, int& width, int& height,
						  int& bits_per_pixel, int& byte_alignment) {
DOTRACE("BmapData::swap");
  updateIfNeeded();

  itsImpl->itsBytes.swap(bytes);

#ifdef ACC_COMPILER
#define std
#endif
  std::swap(itsImpl->itsWidth, width);
  std::swap(itsImpl->itsHeight, height);
  std::swap(itsImpl->itsBitsPerPixel, bits_per_pixel);
  std::swap(itsImpl->itsByteAlignment, byte_alignment);
#ifdef ACC_COMPILER
#undef std
#endif
}

static const char vcid_bmapdata_cc[] = "$Header$";
#endif // !BMAPDATA_CC_DEFINED
