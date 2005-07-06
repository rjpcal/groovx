/** @file media/bmapdata.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Jan 20 00:37:03 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_MEDIA_BMAPDATA_CC_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_BMAPDATA_CC_UTC20050626084018_DEFINED

#include "bmapdata.h"

#include "geom/vec2.h"

#include "rutz/algo.h"
#include "rutz/arrays.h"
#include "rutz/error.h"
#include "rutz/rand.h"
#include "rutz/sharedptr.h"

#include <algorithm>
#include <cstring>              // for memcpy

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::shared_ptr;

///////////////////////////////////////////////////////////////////////
//
// media::bmap_data::impl definition
//
///////////////////////////////////////////////////////////////////////

class media::bmap_data::impl
{
public:
  impl(const geom::vec2<int>& ex, int bits_per_pixel, int byte_alignment) :
    m_size(ex),
    m_bits_per_pixel(bits_per_pixel),
    m_byte_alignment(byte_alignment),
    m_bytes(),
    m_row_order(TOP_FIRST),
    m_updater()
  {
    GVX_PRECONDITION(m_size.x() >= 0);
    GVX_PRECONDITION(m_size.y() >= 0);

    // If m_size.x() is 0, this is still OK, since -1/8 --> -1, so the
    // whole thing goes to 0
    int bytes_per_row = (m_size.x()*bits_per_pixel - 1)/8 + 1;

    GVX_ASSERT(bytes_per_row >= 0);

    unsigned int num_bytes = bytes_per_row * m_size.y();

    m_bytes.resize( num_bytes );
  }

  // default copy-ctor and assn-oper OK

  // These are "fast" versions of the public interface that don't call
  // update_if_needed() before returning data. Therefore it is the
  // caller's responsibility to make sure update_if_needed() has been
  // called at least once per public API call.
  unsigned char* bytes_ptr() { return &(m_bytes[0]); }

  unsigned int bytes_per_row() const
  { return ( (m_size.x()*m_bits_per_pixel - 1)/8 + 1 ); }

  unsigned char* row_ptr(unsigned int row)
  {
    const unsigned int offset =
      (m_row_order == TOP_FIRST)
      ?
      row
      :
      m_size.y() - row - 1;

    return bytes_ptr() + offset * bytes_per_row();
  }

  unsigned int byte_count() const { return m_bytes.size(); }

  geom::vec2<int>                    m_size;
  int                                m_bits_per_pixel;
  int                                m_byte_alignment;
  rutz::dynamic_block<unsigned char> m_bytes;
  row_order                          m_row_order;
  mutable shared_ptr<update_func>    m_updater;
};

///////////////////////////////////////////////////////////////////////
//
// media::bmap_data member definitions
//
///////////////////////////////////////////////////////////////////////

media::bmap_data::update_func::~update_func() {}

media::bmap_data::bmap_data() :
  rep(new impl(geom::vec2<int>(0, 0), 1, 1))
{
GVX_TRACE("media::bmap_data::bmap_data");
}

media::bmap_data::bmap_data(const geom::vec2<int>& m_size,
                            int bits_per_pixel, int byte_alignment) :
  rep(new impl(m_size, bits_per_pixel, byte_alignment))
{
GVX_TRACE("media::bmap_data::bmap_data");
}

media::bmap_data::bmap_data(const media::bmap_data& other) :
  rep(new impl(*(other.rep)))
{
GVX_TRACE("media::bmap_data::bmap_data(copy)");
}

media::bmap_data::~bmap_data()
{
  delete rep;
}

unsigned char* media::bmap_data::bytes_ptr() const
{
GVX_TRACE("media::bmap_data::bytes_ptr");
  update_if_needed();
  return rep->bytes_ptr();
}

unsigned char* media::bmap_data::row_ptr(unsigned int row) const
{
GVX_TRACE("media::bmap_data::row_ptr");

  update_if_needed();

  return rep->row_ptr(row);
}

long int media::bmap_data::bytes_sum() const
{
GVX_TRACE("media::bmap_data::bytes_sum");

  update_if_needed();
  long int sum = 0;
  unsigned int c = byte_count();
  unsigned char* m_bytes = rep->bytes_ptr();
  while (c > 0)
    {
      sum += m_bytes[--c];
    }
  return sum;
}

int media::bmap_data::width() const
{
GVX_TRACE("media::bmap_data::width");
  update_if_needed();
  return rep->m_size.x();
}

int media::bmap_data::height() const
{
GVX_TRACE("media::bmap_data::height");
  update_if_needed();
  return rep->m_size.y();
}

geom::vec2<int> media::bmap_data::size() const
{
GVX_TRACE("media::bmap_data::size");
  update_if_needed();
  return rep->m_size;
}

int media::bmap_data::bits_per_pixel() const
{
GVX_TRACE("media::bmap_data::bits_per_pixel");
  update_if_needed();
  return rep->m_bits_per_pixel;
}

int media::bmap_data::bits_per_component() const
{
GVX_TRACE("media::bmap_data::bits_per_component");
  update_if_needed();
  return rep->m_bits_per_pixel > 1
    ? 8
    : 1;
}

int media::bmap_data::byte_alignment() const
{
GVX_TRACE("media::bmap_data::byte_alignment");
  update_if_needed();
  return rep->m_byte_alignment;
}

unsigned int media::bmap_data::byte_count() const
{
GVX_TRACE("media::bmap_data::byte_count");
  update_if_needed();

  GVX_ASSERT(rep->m_bytes.size() == rep->bytes_per_row() * rep->m_size.y());

  return rep->byte_count();
}

unsigned int media::bmap_data::bytes_per_row() const
{
GVX_TRACE("media::bmap_data::bytes_per_row");
  update_if_needed();
  return rep->bytes_per_row();
}

media::bmap_data::row_order
media::bmap_data::get_row_order() const
{
  return rep->m_row_order;
}

void media::bmap_data::flip_contrast()
{
GVX_TRACE("media::bmap_data::flip_contrast");
  update_if_needed();

  unsigned int num_bytes = rep->m_bytes.size();

  // In this case we want to flip each bit
  if (rep->m_bits_per_pixel == 1)
    {
      for (unsigned int i = 0; i < num_bytes; ++i)
        {
          rep->m_bytes[i] =
            static_cast<unsigned char>(0xff ^ rep->m_bytes[i]);
        }
    }
  // In this case we want to reflect the value of each byte around the
  // middle value, 127.5
  else
    {
      for (unsigned int i = 0; i < num_bytes; ++i)
        {
          rep->m_bytes[i] =
            static_cast<unsigned char>(0xff - rep->m_bytes[i]);
        }
    }
}

void media::bmap_data::flip_vertical()
{
GVX_TRACE("media::bmap_data::flip_vertical");
  update_if_needed();

  int bytes_per_row = rep->bytes_per_row();
  int num_bytes = rep->byte_count();

  rutz::dynamic_block<unsigned char> new_bytes(num_bytes);

  for (int row = 0; row < rep->m_size.y(); ++row)
    {
      int new_row = (rep->m_size.y()-1)-row;
      memcpy(static_cast<void*> (&(new_bytes   [new_row * bytes_per_row])),
             static_cast<void*> (&(rep->m_bytes[row     * bytes_per_row])),
             bytes_per_row);
    }

  rep->m_bytes.swap(new_bytes);
}

void media::bmap_data::clear()
{
GVX_TRACE("media::bmap_data::clear");

  media::bmap_data empty;
  swap(empty);
}

void media::bmap_data::swap(media::bmap_data& other)
{
GVX_TRACE("media::bmap_data::swap");

  rutz::swap2(rep, other.rep);
}

void media::bmap_data::queue_update(shared_ptr<update_func> updater) const
{
GVX_TRACE("media::bmap_data::queue_update");
  rep->m_updater = updater;
}

void media::bmap_data::update_if_needed() const
{
GVX_TRACE("media::bmap_data::update_if_needed");
  if (rep->m_updater.get() != 0)
    {
      shared_ptr<update_func> tmp_updater(rep->m_updater);

      // We release rep->m_updater before doing the update, so that we
      // avoid endless recursion if update_if_needed is called again
      // during the updating.
      rep->m_updater.reset();

      tmp_updater->update(const_cast<media::bmap_data&>(*this));
    }
}

void media::bmap_data::clear_queued_update() const
{
GVX_TRACE("media::bmap_data::clear_queued_update");
  rep->m_updater.reset();
}

void media::bmap_data::set_row_order(row_order order) const
{
  if (order != rep->m_row_order)
    {
      const_cast<bmap_data*>(this)->flip_vertical();
      rep->m_row_order = order;
    }
}

void media::bmap_data::specify_row_order(row_order order) const
{
  rep->m_row_order = order;
}

shared_ptr<media::bmap_data>
media::bmap_data::make_scrambled(int nsubimg_x, int nsubimg_y, int seed,
                                 bool allow_move_subparts,
                                 bool allow_flip_left_right,
                                 bool allow_flip_top_bottom) const
{
GVX_TRACE("media::bmap_data::make_scrambled");

  update_if_needed();

  if ( width() % nsubimg_x != 0 )
    {
      throw rutz::error("not an evenly divisible width", SRC_POS);
    }

  if ( height() % nsubimg_y != 0 )
    {
      throw rutz::error("not an evenly divisible width", SRC_POS);
    }

  shared_ptr<media::bmap_data> result
    (new media::bmap_data(this->size(),
                          this->bits_per_pixel(),
                          this->byte_alignment()));

  const int npos = nsubimg_x * nsubimg_y;

  rutz::fixed_block<int> newpos(npos);
  for (int i = 0; i < npos; ++i)
    {
      newpos[i] = i;
    }

  rutz::urand generator(seed);

  if (allow_move_subparts)
    {
      std::random_shuffle(newpos.begin(), newpos.end(), generator);
    }

  if (rep->m_bits_per_pixel != 8 && rep->m_bits_per_pixel != 24)
    {
      throw rutz::error(rutz::fstring("unknown bits-per-pixel value: ",
                                      rep->m_bits_per_pixel), SRC_POS);
    }

  const int bytes_per_pixel = rep->m_bits_per_pixel/8;

  const int size_subimg_x = result->width() / nsubimg_x * bytes_per_pixel;
  const int size_subimg_y = result->height() / nsubimg_y;

  for (int i = 0; i < npos; ++i)
    {
      const bool fliplr = allow_flip_left_right && generator.booldraw();
      const bool fliptb = allow_flip_top_bottom && generator.booldraw();

      const int src_subimg_y = i / nsubimg_x;
      const int src_subimg_x = i % nsubimg_x;

      const int src_fullimg_y = src_subimg_y * size_subimg_y;
      const int src_fullimg_x = src_subimg_x * size_subimg_x;

      const int dst_subimg_y = newpos[i] / nsubimg_x;
      const int dst_subimg_x = newpos[i] % nsubimg_x;

      const int dst_fullimg_y = dst_subimg_y * size_subimg_y;
      const int dst_fullimg_x = dst_subimg_x * size_subimg_x;

      for (int y = 0; y < size_subimg_y; ++y)
        {
          // NOTE: I tried hand-optimizing all this stuff to avoid
          // calls to row_ptr()... basically optimizing all the math
          // so that we just increment pointers in each loop iteration
          // rather than re-compute positions afresh. But, it made no
          // helpful difference in runtimes. So, better to keep the
          // clean easier-to-read code here. My guess is that the best
          // way to speed this algorithm up, if needed, would be by
          // improving cache performance. Right now we jump around in
          // both the src and dst images. We could speed things up by
          // designing the algorithm so that one of src/dst gets read
          // straight through from beginning to end.

          const unsigned char* src =
            rep->row_ptr(src_fullimg_y+y) + src_fullimg_x;

          const unsigned int dst_row =
            fliptb
            ? dst_fullimg_y+size_subimg_y-1-y
            : dst_fullimg_y+y;

          unsigned char* dst =
            result->rep->row_ptr(dst_row) + dst_fullimg_x;

          unsigned char* dst_end = dst + size_subimg_x;

          if (fliplr)
            {
              if (bytes_per_pixel == 1)
                while (dst_end != dst) { *--dst_end = *src++; }
              else if (bytes_per_pixel == 3)
                while (dst_end != dst)
                  {
                    *(dst_end-3) = *src++;
                    *(dst_end-2) = *src++;
                    *(dst_end-1) = *src++;
                    dst_end -= 3;
                  }
              else
                {
                  GVX_ASSERT(0);
                }
            }
          else
            {
              while (dst != dst_end) { *dst++ = *src++; }
            }
        }
    }

  return result;
}

static const char vcid_groovx_media_bmapdata_cc_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_MEDIA_BMAPDATA_CC_UTC20050626084018_DEFINED
