///////////////////////////////////////////////////////////////////////
//
// bmapdata.cc
//
// Copyright (c) 2000-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Jan 20 00:37:03 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef BMAPDATA_CC_DEFINED
#define BMAPDATA_CC_DEFINED

#include "gx/bmapdata.h"

#include "gx/vec2.h"

#include "util/algo.h"
#include "util/arrays.h"
#include "util/error.h"
#include "util/pointers.h"
#include "util/rand.h"

#include <algorithm>
#include <cstring>              // for memcpy

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

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

  // These are "fast" versions of the public interface that don't call
  // updateIfNeeded() before returning data. Therefore it is the caller's
  // responsibility to make sure updateIfNeeded() has been called at least
  // once per public API call.
  unsigned char* bytesPtr() { return &(bytes[0]); }

  unsigned int bytesPerRow() const
  { return ( (extent.x()*bitsPerPixel - 1)/8 + 1 ); }

  unsigned char* rowPtr(unsigned int row)
  {
    const unsigned int offset =
      (rowOrder == TOP_FIRST)
      ?
      row
      :
      extent.y() - row - 1;

    return bytesPtr() + offset * bytesPerRow();
  }

  unsigned int byteCount() const { return bytes.size(); }

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
  return rep->bytesPtr();
}

unsigned char* Gfx::BmapData::rowPtr(unsigned int row) const
{
DOTRACE("Gfx::BmapData::rowPtr");

  updateIfNeeded();

  return rep->rowPtr(row);
}

long int Gfx::BmapData::checkSum() const
{
DOTRACE("Gfx::BmapData::checkSum");

  updateIfNeeded();
  long int sum = 0;
  unsigned int byte_count = byteCount();
  unsigned char* bytes = rep->bytesPtr();
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

int Gfx::BmapData::bitsPerComponent() const
{
DOTRACE("Gfx::BmapData::bitsPerComponent");
  updateIfNeeded();
  return rep->bitsPerPixel > 1
    ? 8
    : 1;
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

  Assert(rep->bytes.size() == rep->bytesPerRow() * rep->extent.y());

  return rep->byteCount();
}

unsigned int Gfx::BmapData::bytesPerRow() const
{
DOTRACE("Gfx::BmapData::bytesPerRow");
  updateIfNeeded();
  return rep->bytesPerRow();
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

  int bytes_per_row = rep->bytesPerRow();
  int num_bytes = rep->byteCount();

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

shared_ptr<Gfx::BmapData>
Gfx::BmapData::makeScrambled(int nsubimg_x, int nsubimg_y, int seed) const
{
DOTRACE("Gfx::BmapData::makeScrambled");

  Gfx::BmapData result(this->size(),
                       this->bitsPerPixel(),
                       this->byteAlignment());

  if ( result.width() % nsubimg_x != 0 )
    {
      throw Util::Error("not an evenly divisible width");
    }

  if ( result.height() % nsubimg_y != 0 )
    {
      throw Util::Error("not an evenly divisible width");
    }

  const int npos = nsubimg_x * nsubimg_y;

  fixed_block<int> newpos(npos);
  for (int i = 0; i < npos; ++i)
    {
      newpos[i] = i;
    }

  Util::Urand generator(seed);

  std::random_shuffle(newpos.begin(), newpos.end(), generator);

  const int bytes_per_row = rep->bytesPerRow();

  const int size_subimg_x = result.width() / nsubimg_x * bitsPerPixel()/8;
  const int size_subimg_y = result.height() / nsubimg_y;

  for (int i = 0; i < npos; ++i)
    {
      const bool fliplr = generator.booldraw();
      const bool fliptb = generator.booldraw();

      const int src_subimg_y = i / nsubimg_y;
      const int src_subimg_x = i % nsubimg_y;

      const int src_fullimg_y = src_subimg_y * size_subimg_y;
      const int src_fullimg_x = src_subimg_x * size_subimg_x;

      const int dst_subimg_y = newpos[i] / nsubimg_y;
      const int dst_subimg_x = newpos[i] % nsubimg_y;

      const int dst_fullimg_y = dst_subimg_y * size_subimg_y;
      const int dst_fullimg_x = dst_subimg_x * size_subimg_x;

      for (int y = 0; y < size_subimg_y; ++y)
        {
          const unsigned int src_offset =
            (rep->rowOrder == TOP_FIRST)
            ?
            src_fullimg_y + y
            :
            rep->extent.y() - 1 - (src_fullimg_y + y);

          const unsigned char* src =
            rep->bytesPtr() + src_offset * bytes_per_row + src_fullimg_x;

//           const unsigned char* src =
//             rep->rowPtr(src_fullimg_y+y) + src_fullimg_x;

          unsigned int dst_row =
            fliptb
            ? dst_fullimg_y+size_subimg_y-1-y
            : dst_fullimg_y+y;

          const unsigned int dst_offset =
            (rep->rowOrder == TOP_FIRST)
            ?
            dst_row
            :
            result.rep->extent.y() - 1 - dst_row;

//           unsigned char* dst =
//             result.rep->rowPtr(dst_row) + dst_fullimg_x;

          unsigned char* dst =
            result.rep->bytesPtr() + dst_offset * bytes_per_row + dst_fullimg_x;

          unsigned char* dst_end = dst + size_subimg_x;

          if (fliplr)
            {
              while (dst_end != dst)
                {
                  *--dst_end = *src++;
                }
            }
          else
            {
              while (dst != dst_end)
                {
                  *dst++ = *src++;
                }
            }
        }
    }

  shared_ptr<Gfx::BmapData> ret(new Gfx::BmapData);
  ret->swap(result);
  return ret;
}

static const char vcid_bmapdata_cc[] = "$Header$";
#endif // !BMAPDATA_CC_DEFINED
