///////////////////////////////////////////////////////////////////////
//
// bmapdata.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jan 20 00:37:03 2000
// written: Thu Aug  9 16:06:22 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPDATA_CC_DEFINED
#define BMAPDATA_CC_DEFINED

#include "bmapdata.h"

#include "point.h"

#include "util/algo.h"

#include <cstring>              // for memcpy

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
  Impl(const Point<int>& extent, int bits_per_pixel, int byte_alignment) :
    itsExtent(extent),
    itsBitsPerPixel(bits_per_pixel),
    itsByteAlignment(byte_alignment),
    itsBytes(),
    itsUpdater(0)
  {
    Precondition(extent.x() >= 0); Precondition(extent.y() >= 0);

    // If extent.x() is 0, this is still OK, since -1/8 --> -1, so the
    // whole thing goes to 0
    int bytes_per_row = (extent.x()*bits_per_pixel - 1)/8 + 1;

    Assert(bytes_per_row >= 0);

    unsigned int num_bytes = bytes_per_row * extent.y();

    itsBytes.resize( num_bytes );
  }

  // default copy-ctor and assn-oper OK

  Point<int> itsExtent;
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
  itsImpl(new Impl(Point<int>(0, 0), 1, 1))
{
DOTRACE("BmapData::BmapData");
}

BmapData::BmapData(const Point<int>& extent,
                   int bits_per_pixel, int byte_alignment) :
  itsImpl(new Impl(extent, bits_per_pixel, byte_alignment))
{
DOTRACE("BmapData::BmapData");
}

BmapData::BmapData(const BmapData& other) :
  itsImpl(new Impl(*(other.itsImpl)))
{
DOTRACE("BmapData::BmapData(copy)");
}

BmapData::~BmapData()
{
  delete itsImpl;
}

unsigned char* BmapData::bytesPtr() const
{
DOTRACE("BmapData::bytesPtr");
  updateIfNeeded();
  return const_cast<unsigned char*>(&(itsImpl->itsBytes[0]));
}

dynamic_block<unsigned char>& BmapData::bytesVec()
{
  updateIfNeeded();
  return itsImpl->itsBytes;
}

const dynamic_block<unsigned char>& BmapData::bytesVec() const
{
  updateIfNeeded();
  return itsImpl->itsBytes;
}

int BmapData::width() const
{
DOTRACE("BmapData::width");
  updateIfNeeded();
  return itsImpl->itsExtent.x();
}

int BmapData::height() const
{
DOTRACE("BmapData::height");
  updateIfNeeded();
  return itsImpl->itsExtent.y();
}

Point<int> BmapData::extent() const
{
DOTRACE("BmapData::height");
  return itsImpl->itsExtent;
}

int BmapData::bitsPerPixel() const
{
DOTRACE("BmapData::bitsPerPixel");
  return itsImpl->itsBitsPerPixel;
}

int BmapData::byteAlignment() const
{
DOTRACE("BmapData::byteAlignment");
  return itsImpl->itsByteAlignment;
}

int BmapData::byteCount() const
{
DOTRACE("BmapData::byteCount");
  updateIfNeeded();
  return bytesPerRow() * itsImpl->itsExtent.y();
}

int BmapData::bytesPerRow() const
{
DOTRACE("BmapData::bytesPerRow");
  updateIfNeeded();
  return ( (itsImpl->itsExtent.x()*itsImpl->itsBitsPerPixel - 1)/8 + 1 );
}

void BmapData::flipContrast()
{
DOTRACE("BmapData::flipContrast");
  updateIfNeeded();

  unsigned int num_bytes = itsImpl->itsBytes.size();

  // In this case we want to flip each bit
  if (itsImpl->itsBitsPerPixel == 1)
    {
      for (unsigned int i = 0; i < num_bytes; ++i)
        {
          itsImpl->itsBytes[i] =
            static_cast<unsigned char>(0xff ^ itsImpl->itsBytes[i]);
        }
    }
  // In this case we want to reflect the value of each byte around the
  // middle value, 127.5
  else
    {
      for (unsigned int i = 0; i < num_bytes; ++i)
        {
          itsImpl->itsBytes[i] =
            static_cast<unsigned char>(0xff - itsImpl->itsBytes[i]);
        }
    }
}

void BmapData::flipVertical()
{
DOTRACE("BmapData::flipVertical");
  updateIfNeeded();

  int bytes_per_row = bytesPerRow();
  int num_bytes = byteCount();

  dynamic_block<unsigned char> new_bytes(num_bytes);

  for (int row = 0; row < itsImpl->itsExtent.y(); ++row)
    {
      int new_row = (itsImpl->itsExtent.y()-1)-row;
      memcpy(static_cast<void*> (&(new_bytes[new_row * bytes_per_row])),
             static_cast<void*> (&(itsImpl->itsBytes [row     * bytes_per_row])),
             bytes_per_row);
    }

  itsImpl->itsBytes.swap(new_bytes);
}

void BmapData::clear()
{
DOTRACE("BmapData::clear");
  itsImpl->itsBytes.resize(1);
  itsImpl->itsExtent.set(1,1);
  itsImpl->itsBitsPerPixel = 1;
  itsImpl->itsByteAlignment = 1;

  itsImpl->itsUpdater.reset(0);
}

void BmapData::swap(BmapData& other)
{
DOTRACE("BmapData::swap");
  updateIfNeeded();

  itsImpl->itsBytes.swap(other.itsImpl->itsBytes);

  Util::swap(itsImpl->itsExtent, other.itsImpl->itsExtent);
  Util::swap(itsImpl->itsBitsPerPixel, other.itsImpl->itsBitsPerPixel);
  Util::swap(itsImpl->itsByteAlignment, other.itsImpl->itsByteAlignment);
}

void BmapData::swap(dynamic_block<unsigned char>& bytes, int& width, int& height,
                    int& bits_per_pixel, int& byte_alignment)
{
DOTRACE("BmapData::swap");
  updateIfNeeded();

  itsImpl->itsBytes.swap(bytes);

  Util::swap(itsImpl->itsExtent.x(), width);
  Util::swap(itsImpl->itsExtent.y(), height);
  Util::swap(itsImpl->itsBitsPerPixel, bits_per_pixel);
  Util::swap(itsImpl->itsByteAlignment, byte_alignment);
}

void BmapData::queueUpdate(shared_ptr<UpdateFunc> updater) const
{
DOTRACE("BmapData::queueUpdate");
  itsImpl->itsUpdater = updater;
}

void BmapData::updateIfNeeded() const
{
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

void BmapData::clearQueuedUpdate() const
{
DOTRACE("BmapData::clearQueuedUpdate");
  itsImpl->itsUpdater.reset(0);
}

static const char vcid_bmapdata_cc[] = "$Header$";
#endif // !BMAPDATA_CC_DEFINED
