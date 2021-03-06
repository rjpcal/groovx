/** @file media/bmapdata.cc storage class for bitmap images, handling
    sizing and storage alignment */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Jan 20 00:37:03 2000
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "bmapdata.h"

#include "geom/vec2.h"

#include "rutz/error.h"
#include "rutz/rand.h"
#include "rutz/sfmt.h"

#include <algorithm>
#include <cstring>              // for memcpy
#include <memory>
#include <utility>
#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using std::shared_ptr;

///////////////////////////////////////////////////////////////////////
//
// media::bmap_data::impl definition
//
///////////////////////////////////////////////////////////////////////

class media::bmap_data::impl
{
public:
  impl(const geom::vec2<size_t>& ex, unsigned int bits_per_pixel, unsigned int byte_alignment) :
    m_size(ex),
    m_bits_per_pixel(bits_per_pixel),
    m_byte_alignment(byte_alignment),
    m_bytes(),
    m_row_order(row_order::TOP_FIRST)
  {
    // If m_size.x() is 0, this is still OK, since 7/8 --> 0, so the
    // whole thing goes to 0
    const size_t bytes_per_row = (m_size.x()*bits_per_pixel + 7)/8;

    const size_t num_bytes = bytes_per_row * m_size.y();

    m_bytes.resize( num_bytes );
  }

  // default copy-ctor and assn-oper OK

  unsigned char* bytes_ptr() { return &(m_bytes[0]); }

  size_t bytes_per_row() const
  { return ( (m_size.x()*m_bits_per_pixel + 7)/8 ); }

  unsigned char* row_ptr(size_t row)
  {
    GVX_ASSERT(row < m_size.y());

    const size_t offset =
      (m_row_order == row_order::TOP_FIRST)
      ?
      row
      :
      m_size.y() - row - 1;

    return bytes_ptr() + offset * bytes_per_row();
  }

  size_t byte_count() const { return m_bytes.size(); }

  geom::vec2<size_t>                 m_size;
  unsigned int                       m_bits_per_pixel;
  unsigned int                       m_byte_alignment;
  std::vector<unsigned char>         m_bytes;
  row_order                          m_row_order;
};

///////////////////////////////////////////////////////////////////////
//
// media::bmap_data member definitions
//
///////////////////////////////////////////////////////////////////////

media::bmap_data::bmap_data() :
  rep(new impl(geom::vec2<size_t>(0, 0), 1, 1))
{
GVX_TRACE("media::bmap_data::bmap_data");
}

media::bmap_data::bmap_data(const geom::vec2<size_t>& m_size,
                            unsigned int bits_per_pixel, unsigned int byte_alignment) :
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
  return rep->bytes_ptr();
}

unsigned char* media::bmap_data::row_ptr(size_t row) const
{
GVX_TRACE("media::bmap_data::row_ptr");

  return rep->row_ptr(row);
}

long int media::bmap_data::bytes_sum() const
{
GVX_TRACE("media::bmap_data::bytes_sum");

  long int sum = 0;
  size_t c = byte_count();
  const unsigned char* const bytes = rep->bytes_ptr();
  while (c > 0)
    {
      sum += bytes[--c];
    }
  return sum;
}

uint32_t media::bmap_data::bkdr_hash() const {
  uint32_t seed = 131;
  uint32_t result = 0;
  const unsigned char* const bytes = rep->bytes_ptr();

  for (size_t i = 0, n = byte_count(); i < n; ++i)
    result = (result * seed) + bytes[i];

  return result;
}
size_t media::bmap_data::width() const
{
GVX_TRACE("media::bmap_data::width");
  return rep->m_size.x();
}

size_t media::bmap_data::height() const
{
GVX_TRACE("media::bmap_data::height");
  return rep->m_size.y();
}

geom::vec2<size_t> media::bmap_data::size() const
{
GVX_TRACE("media::bmap_data::size");
  return rep->m_size;
}

unsigned int media::bmap_data::bits_per_pixel() const
{
GVX_TRACE("media::bmap_data::bits_per_pixel");
  return rep->m_bits_per_pixel;
}

unsigned int media::bmap_data::bits_per_component() const
{
GVX_TRACE("media::bmap_data::bits_per_component");
  return rep->m_bits_per_pixel > 1
    ? 8
    : 1;
}

unsigned int media::bmap_data::byte_alignment() const
{
GVX_TRACE("media::bmap_data::byte_alignment");
  return rep->m_byte_alignment;
}

size_t media::bmap_data::byte_count() const
{
GVX_TRACE("media::bmap_data::byte_count");

  GVX_ASSERT(rep->m_bytes.size() == rep->bytes_per_row() * rep->m_size.y());

  return rep->byte_count();
}

size_t media::bmap_data::bytes_per_row() const
{
GVX_TRACE("media::bmap_data::bytes_per_row");
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

  size_t num_bytes = rep->m_bytes.size();

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

  const size_t bytes_per_row = rep->bytes_per_row();
  const size_t num_bytes = rep->byte_count();

  std::vector<unsigned char> new_bytes(num_bytes);

  for (size_t row = 0; row < rep->m_size.y(); ++row)
    {
      size_t new_row = (rep->m_size.y()-1)-row;
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
  this->swap(empty);
}

void media::bmap_data::swap(media::bmap_data& other) noexcept
{
  std::swap(rep, other.rep);
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

media::bmap_data
media::bmap_data::make_scrambled(unsigned int nsubimg_x,
                                 unsigned int nsubimg_y,
                                 unsigned long seed,
                                 bool allow_move_subparts,
                                 bool allow_flip_left_right,
                                 bool allow_flip_top_bottom) const
{
GVX_TRACE("media::bmap_data::make_scrambled");

  if ( width() % nsubimg_x != 0 )
    {
      throw rutz::error("not an evenly divisible width", SRC_POS);
    }

  if ( height() % nsubimg_y != 0 )
    {
      throw rutz::error("not an evenly divisible width", SRC_POS);
    }

  media::bmap_data result(this->size(),
                          this->bits_per_pixel(),
                          this->byte_alignment());

  const unsigned int npos = nsubimg_x * nsubimg_y;

  std::vector<unsigned int> newpos(npos);
  for (unsigned int i = 0; i < npos; ++i)
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
      throw rutz::error(rutz::sfmt("unknown bits-per-pixel value: %d",
                                   rep->m_bits_per_pixel), SRC_POS);
    }

  const size_t bytes_per_pixel = rep->m_bits_per_pixel/8;

  const size_t size_subimg_x = result.width() / nsubimg_x * bytes_per_pixel;
  const size_t size_subimg_y = result.height() / nsubimg_y;

  for (unsigned int i = 0; i < npos; ++i)
    {
      const bool fliplr = allow_flip_left_right && generator.booldraw();
      const bool fliptb = allow_flip_top_bottom && generator.booldraw();

      const size_t src_subimg_y = i / nsubimg_x;
      const size_t src_subimg_x = i % nsubimg_x;

      const size_t src_fullimg_y = src_subimg_y * size_subimg_y;
      const size_t src_fullimg_x = src_subimg_x * size_subimg_x;

      const size_t dst_subimg_y = newpos[i] / nsubimg_x;
      const size_t dst_subimg_x = newpos[i] % nsubimg_x;

      const size_t dst_fullimg_y = dst_subimg_y * size_subimg_y;
      const size_t dst_fullimg_x = dst_subimg_x * size_subimg_x;

      for (unsigned int y = 0; y < size_subimg_y; ++y)
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

          const size_t dst_row =
            fliptb
            ? dst_fullimg_y+size_subimg_y-1-y
            : dst_fullimg_y+y;

          unsigned char* dst =
            result.rep->row_ptr(dst_row) + dst_fullimg_x;

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
