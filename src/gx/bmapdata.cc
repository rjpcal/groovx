///////////////////////////////////////////////////////////////////////
//
// bmapdata.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Jan 20 00:37:03 2000
// written: Wed Mar 19 12:45:55 2003
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
  Impl(const Gfx::Vec2<int>& ex, int bits_per_pixel, int byte_alignment) :
    extent(ex),
    bitsPerPixel(bits_per_pixel),
    byteAlignment(byte_alignment),
    bytes(),
    rowOrder(TOP_FIRST),
    updater(0)
  {
    Precondition(extent.x() >= 0); Precondition(extent.y() >= 0);

    // If extent.x() is 0, this is still OK, since -1/8 --> -1, so the
    // whole thing goes to 0
    int bytes_per_row = (extent.x()*bits_per_pixel - 1)/8 + 1;

    Assert(bytes_per_row >= 0);

    unsigned int num_bytes = bytes_per_row * extent.y();

    bytes.resize( num_bytes );
  }

  // default copy-ctor and assn-oper OK

  Gfx::Vec2<int> extent;
  int bitsPerPixel;
  int byteAlignment;
  dynamic_block<unsigned char> bytes;
  RowOrder rowOrder;

  mutable shared_ptr<UpdateFunc> updater;
};

///////////////////////////////////////////////////////////////////////
//
// BmapData member definitions
//
///////////////////////////////////////////////////////////////////////

Gfx::BmapData::UpdateFunc::~UpdateFunc() {}

Gfx::BmapData::BmapData() :
  rep(new Impl(Gfx::Vec2<int>(0, 0), 1, 1))
{
DOTRACE("Gfx::BmapData::BmapData");
}

Gfx::BmapData::BmapData(const Gfx::Vec2<int>& extent,
                        int bits_per_pixel, int byte_alignment) :
  rep(new Impl(extent, bits_per_pixel, byte_alignment))
{
DOTRACE("Gfx::BmapData::BmapData");
}

Gfx::BmapData::BmapData(const Gfx::BmapData& other) :
  rep(new Impl(*(other.rep)))
{
DOTRACE("Gfx::BmapData::BmapData(copy)");
}

Gfx::BmapData::~BmapData()
{
  delete rep;
}

unsigned char* Gfx::BmapData::bytesPtr() const
{
DOTRACE("Gfx::BmapData::bytesPtr");
  updateIfNeeded();
  return &(rep->bytes[0]);
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
  return rep->extent.x();
}

int Gfx::BmapData::height() const
{
DOTRACE("Gfx::BmapData::height");
  updateIfNeeded();
  return rep->extent.y();
}

Gfx::Vec2<int> Gfx::BmapData::size() const
{
DOTRACE("Gfx::BmapData::size");
  updateIfNeeded();
  return rep->extent;
}

int Gfx::BmapData::bitsPerPixel() const
{
DOTRACE("Gfx::BmapData::bitsPerPixel");
  updateIfNeeded();
  return rep->bitsPerPixel;
}

int Gfx::BmapData::byteAlignment() const
{
DOTRACE("Gfx::BmapData::byteAlignment");
  updateIfNeeded();
  return rep->byteAlignment;
}

unsigned int Gfx::BmapData::byteCount() const
{
DOTRACE("Gfx::BmapData::byteCount");
  updateIfNeeded();

  Assert(rep->bytes.size() == bytesPerRow() * rep->extent.y());

  return rep->bytes.size();
}

unsigned int Gfx::BmapData::bytesPerRow() const
{
DOTRACE("Gfx::BmapData::bytesPerRow");
  updateIfNeeded();
  return ( (rep->extent.x()*rep->bitsPerPixel - 1)/8 + 1 );
}

Gfx::BmapData::RowOrder
Gfx::BmapData::rowOrder() const
{
  return rep->rowOrder;
}

void Gfx::BmapData::flipContrast()
{
DOTRACE("Gfx::BmapData::flipContrast");
  updateIfNeeded();

  unsigned int num_bytes = rep->bytes.size();

  // In this case we want to flip each bit
  if (rep->bitsPerPixel == 1)
    {
      for (unsigned int i = 0; i < num_bytes; ++i)
        {
          rep->bytes[i] = static_cast<unsigned char>(0xff ^ rep->bytes[i]);
        }
    }
  // In this case we want to reflect the value of each byte around the
  // middle value, 127.5
  else
    {
      for (unsigned int i = 0; i < num_bytes; ++i)
        {
          rep->bytes[i] = static_cast<unsigned char>(0xff - rep->bytes[i]);
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

  for (int row = 0; row < rep->extent.y(); ++row)
    {
      int new_row = (rep->extent.y()-1)-row;
      memcpy(static_cast<void*> (&(new_bytes [new_row * bytes_per_row])),
             static_cast<void*> (&(rep->bytes[row     * bytes_per_row])),
             bytes_per_row);
    }

  rep->bytes.swap(new_bytes);
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

  Util::swap2(rep, other.rep);
}

void Gfx::BmapData::queueUpdate(shared_ptr<UpdateFunc> updater) const
{
DOTRACE("Gfx::BmapData::queueUpdate");
  rep->updater = updater;
}

void Gfx::BmapData::updateIfNeeded() const
{
DOTRACE("Gfx::BmapData::updateIfNeeded");
  if (rep->updater.get() != 0)
    {
      shared_ptr<UpdateFunc> tempUpdater(rep->updater);

      // We release rep->updater before doing the update, so
      // that we avoid endless recursion if updateIfNeeded is called
      // again during the updating.
      rep->updater.reset(0);

      tempUpdater->update(const_cast<Gfx::BmapData&>(*this));
    }
}

void Gfx::BmapData::clearQueuedUpdate() const
{
DOTRACE("Gfx::BmapData::clearQueuedUpdate");
  rep->updater.reset(0);
}

void Gfx::BmapData::setRowOrder(Gfx::BmapData::RowOrder order) const
{
  if (order != rep->rowOrder)
    {
      const_cast<BmapData*>(this)->flipVertical();
      rep->rowOrder = order;
    }
}

void Gfx::BmapData::specifyRowOrder(Gfx::BmapData::RowOrder order) const
{
  rep->rowOrder = order;
}

static const char vcid_bmapdata_cc[] = "$Header$";
#endif // !BMAPDATA_CC_DEFINED
