///////////////////////////////////////////////////////////////////////
//
// bmapdata.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jan 20 00:37:03 2000
// written: Wed Nov 13 11:03:10 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPDATA_CC_DEFINED
#define BMAPDATA_CC_DEFINED

#include "gx/bmapdata.h"

#include "gx/vec2.h"

#include "util/algo.h"

#include <cstring>              // for memcpy

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// Gfx::BmapData::Impl definition
//
///////////////////////////////////////////////////////////////////////

class Gfx::BmapData::Impl
{
public:
  Impl(const Gfx::Vec2<int>& extent, int bits_per_pixel, int byte_alignment) :
    itsExtent(extent),
    itsBitsPerPixel(bits_per_pixel),
    itsByteAlignment(byte_alignment),
    itsBytes(),
    itsRowOrder(TOP_FIRST),
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

  Gfx::Vec2<int> itsExtent;
  int itsBitsPerPixel;
  int itsByteAlignment;
  dynamic_block<unsigned char> itsBytes;
  RowOrder itsRowOrder;

  mutable shared_ptr<UpdateFunc> itsUpdater;
};

///////////////////////////////////////////////////////////////////////
//
// BmapData member definitions
//
///////////////////////////////////////////////////////////////////////

Gfx::BmapData::UpdateFunc::~UpdateFunc() {}

Gfx::BmapData::BmapData() :
  itsImpl(new Impl(Gfx::Vec2<int>(0, 0), 1, 1))
{
DOTRACE("Gfx::BmapData::BmapData");
}

Gfx::BmapData::BmapData(const Gfx::Vec2<int>& extent,
                        int bits_per_pixel, int byte_alignment) :
  itsImpl(new Impl(extent, bits_per_pixel, byte_alignment))
{
DOTRACE("Gfx::BmapData::BmapData");
}

Gfx::BmapData::BmapData(const Gfx::BmapData& other) :
  itsImpl(new Impl(*(other.itsImpl)))
{
DOTRACE("Gfx::BmapData::BmapData(copy)");
}

Gfx::BmapData::~BmapData()
{
  delete itsImpl;
}

unsigned char* Gfx::BmapData::bytesPtr() const
{
DOTRACE("Gfx::BmapData::bytesPtr");
  updateIfNeeded();
  return &(itsImpl->itsBytes[0]);
}

unsigned char* Gfx::BmapData::rowPtr(unsigned int row) const
{
DOTRACE("Gfx::BmapData::rowPtr");

  const unsigned int offset =
    (rowOrder() == TOP_FIRST)
    ?
    row
    :
    height() - row - 1;

  return bytesPtr() + offset * bytesPerRow();
}

long int Gfx::BmapData::checkSum() const
{
DOTRACE("Gfx::BmapData::checkSum");

  long int sum = 0;
  unsigned int byte_count = byteCount();
  unsigned char* bytes = bytesPtr();
  while (byte_count > 0)
    {
      sum += bytes[--byte_count];
    }
  return sum;
}

int Gfx::BmapData::width() const
{
DOTRACE("Gfx::BmapData::width");
  updateIfNeeded();
  return itsImpl->itsExtent.x();
}

int Gfx::BmapData::height() const
{
DOTRACE("Gfx::BmapData::height");
  updateIfNeeded();
  return itsImpl->itsExtent.y();
}

Gfx::Vec2<int> Gfx::BmapData::size() const
{
DOTRACE("Gfx::BmapData::size");
  updateIfNeeded();
  return itsImpl->itsExtent;
}

int Gfx::BmapData::bitsPerPixel() const
{
DOTRACE("Gfx::BmapData::bitsPerPixel");
  updateIfNeeded();
  return itsImpl->itsBitsPerPixel;
}

int Gfx::BmapData::byteAlignment() const
{
DOTRACE("Gfx::BmapData::byteAlignment");
  updateIfNeeded();
  return itsImpl->itsByteAlignment;
}

unsigned int Gfx::BmapData::byteCount() const
{
DOTRACE("Gfx::BmapData::byteCount");
  updateIfNeeded();

  Assert(itsImpl->itsBytes.size() == bytesPerRow() * itsImpl->itsExtent.y());

  return itsImpl->itsBytes.size();
}

unsigned int Gfx::BmapData::bytesPerRow() const
{
DOTRACE("Gfx::BmapData::bytesPerRow");
  updateIfNeeded();
  return ( (itsImpl->itsExtent.x()*itsImpl->itsBitsPerPixel - 1)/8 + 1 );
}

Gfx::BmapData::RowOrder
Gfx::BmapData::rowOrder() const
{
  return itsImpl->itsRowOrder;
}

void Gfx::BmapData::flipContrast()
{
DOTRACE("Gfx::BmapData::flipContrast");
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

void Gfx::BmapData::flipVertical()
{
DOTRACE("Gfx::BmapData::flipVertical");
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

void Gfx::BmapData::clear()
{
DOTRACE("Gfx::BmapData::clear");

  Gfx::BmapData empty;
  swap(empty);
}

void Gfx::BmapData::swap(Gfx::BmapData& other)
{
DOTRACE("Gfx::BmapData::swap");

  Util::swap2(itsImpl, other.itsImpl);
}

void Gfx::BmapData::queueUpdate(shared_ptr<UpdateFunc> updater) const
{
DOTRACE("Gfx::BmapData::queueUpdate");
  itsImpl->itsUpdater = updater;
}

void Gfx::BmapData::updateIfNeeded() const
{
DOTRACE("Gfx::BmapData::updateIfNeeded");
  if (itsImpl->itsUpdater.get() != 0)
    {
      shared_ptr<UpdateFunc> tempUpdater(itsImpl->itsUpdater);

      // We release itsImpl->itsUpdater before doing the update, so
      // that we avoid endless recursion if updateIfNeeded is called
      // again during the updating.
      itsImpl->itsUpdater.reset(0);

      tempUpdater->update(const_cast<Gfx::BmapData&>(*this));
    }
}

void Gfx::BmapData::clearQueuedUpdate() const
{
DOTRACE("Gfx::BmapData::clearQueuedUpdate");
  itsImpl->itsUpdater.reset(0);
}

void Gfx::BmapData::setRowOrder(Gfx::BmapData::RowOrder order) const
{
  if (order != itsImpl->itsRowOrder)
    {
      const_cast<BmapData*>(this)->flipVertical();
      itsImpl->itsRowOrder = order;
    }
}

void Gfx::BmapData::specifyRowOrder(Gfx::BmapData::RowOrder order) const
{
  itsImpl->itsRowOrder = order;
}

static const char vcid_bmapdata_cc[] = "$Header$";
#endif // !BMAPDATA_CC_DEFINED
