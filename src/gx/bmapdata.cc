///////////////////////////////////////////////////////////////////////
//
// bmapdata.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jan 20 00:37:03 2000
// written: Thu Jan 20 02:12:14 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPDATA_CC_DEFINED
#define BMAPDATA_CC_DEFINED

#include "bmapdata.h"

#include <algorithm>
#include <cstring>				  // for memcpy

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// BitmapRep::BmapData member definitions
//
///////////////////////////////////////////////////////////////////////

BmapData::UpdateFunc::~UpdateFunc() {}

BmapData::BmapData(int width, int height,
						 int bits_per_pixel, int byte_alignment) :
  itsWidth(width),
  itsHeight(height),
  itsBitsPerPixel(bits_per_pixel),
  itsByteAlignment(byte_alignment),
  itsBytes(),
  itsUpdater(0)
{
DOTRACE("BmapData::BmapData");
  int num_bytes = (width/8 + 1) * height  + 1;
  Assert(num_bytes > 0);
  itsBytes.resize( num_bytes );
}

unsigned char* BmapData::bytesPtr() const {
DOTRACE("BmapData::bytesPtr");
  updateIfNeeded(); 
  return const_cast<unsigned char*>(&(itsBytes[0]));
}

vector<unsigned char>& BmapData::bytesVec() {
  updateIfNeeded(); 
  return itsBytes;
}

const vector<unsigned char>& BmapData::bytesVec() const {
  updateIfNeeded(); 
  return itsBytes;
}

int BmapData::width() const {
DOTRACE("BmapData::width");
  updateIfNeeded(); 
  return itsWidth;
}

int BmapData::height() const {
DOTRACE("BmapData::height");
  updateIfNeeded(); 
  return itsHeight; 
}

int BmapData::byteCount() const {
DOTRACE("BmapData::byteCount");
  updateIfNeeded(); 
  return bytesPerRow() * itsHeight;
}

int BmapData::bytesPerRow() const {
DOTRACE("BmapData::bytesPerRow");
  updateIfNeeded(); 
  return ( (itsWidth*itsBitsPerPixel - 1)/8 + 1 );
}

void BmapData::flipContrast() {
DOTRACE("BmapData::flipContrast");
  updateIfNeeded(); 

  int num_bytes = itsBytes.size();

  // In this case we want to flip each bit
  if (itsBitsPerPixel == 1) {
	 for (int i = 0; i < num_bytes; ++i) {
		itsBytes[i] ^= 0xff;
	 }
  }
  // In this case we want to reflect the value of each byte around the
  // middle value, 127.5
  else {
	 for (int i = 0; i < num_bytes; ++i) {
		itsBytes[i] = 0xff - itsBytes[i];
	 }
  }
}

void BmapData::flipVertical() {
DOTRACE("BmapData::flipVertical");
  updateIfNeeded(); 

  int bytes_per_row = bytesPerRow();
  int num_bytes = byteCount();
  
  vector<unsigned char> new_bytes(num_bytes);
  
  for (int row = 0; row < itsHeight; ++row) {
	 int new_row = (itsHeight-1)-row;
	 memcpy(static_cast<void*> (&(new_bytes[new_row * bytes_per_row])),
			  static_cast<void*> (&(itsBytes [row     * bytes_per_row])),
			  bytes_per_row);
  }
  
  itsBytes.swap(new_bytes);
}

void BmapData::clear() {
DOTRACE("BmapData::clear");
  itsBytes.resize(1);
  itsHeight = 1;
  itsWidth = 1;
  itsBitsPerPixel = 1;
  itsByteAlignment = 1;

  itsUpdater.reset(0);
}

void BmapData::swap(BmapData& other) {
DOTRACE("BmapData::swap");
  updateIfNeeded();
 
  itsBytes.swap(other.itsBytes);

#ifdef ACC_COMPILER
#define std
#endif
  std::swap(itsWidth, other.itsWidth);
  std::swap(itsHeight, other.itsHeight);
  std::swap(itsBitsPerPixel, other.itsBitsPerPixel);
  std::swap(itsByteAlignment, other.itsByteAlignment);
#ifdef ACC_COMPILER
#undef std
#endif
}

void BmapData::swap(vector<unsigned char>& bytes, int& width, int& height,
						  int& bits_per_pixel, int& byte_alignment) {
DOTRACE("BmapData::swap");
  updateIfNeeded();

  itsBytes.swap(bytes);

#ifdef ACC_COMPILER
#define std
#endif
  std::swap(itsWidth, width);
  std::swap(itsHeight, height);
  std::swap(itsBitsPerPixel, bits_per_pixel);
  std::swap(itsByteAlignment, byte_alignment);
#ifdef ACC_COMPILER
#undef std
#endif
}

static const char vcid_bmapdata_cc[] = "$Header$";
#endif // !BMAPDATA_CC_DEFINED
