/** @file media/pngparser.cc load/save png images */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Apr 24 20:05:06 2002
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

#ifndef GROOVX_MEDIA_PNGPARSER_CC_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_PNGPARSER_CC_UTC20050626084018_DEFINED

#include "pngparser.h"

#ifndef HAVE_LIBPNG

#include "rutz/error.h"

void media::load_png(const char* /*filename*/, media::bmap_data& /*data*/)
{
  throw rutz::error("png image files are not supported in this build",
                    SRC_POS);
}

void media::save_png(const char* /*filename*/, const media::bmap_data& /*data*/)
{
  throw rutz::error("png image files are not supported in this build",
                    SRC_POS);
}

#else

#include "geom/vec2.h"

#include "media/bmapdata.h"

#include "rutz/arrays.h"
#include "rutz/error.h"
#include "rutz/sfmt.h"

#include <cstdio>
#include <png.h>

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

namespace
{
  /// This class provides exception-safe cleanup for libpng.
  /** It allows us to throw exceptions during our parsing routine and
      be assured that all libpng structures are properly destroyed via
      png_parser's destructor. */
  class png_parser
  {
  public:
    png_parser() :
      m_file(0),
      m_png_ptr(0),
      m_info_ptr(0),
      m_end_ptr(0)
    {}

    ~png_parser() { close(); }

    void close();

    void parse(const char* filename, media::bmap_data& data);

  private:
    png_parser(const png_parser&);
    png_parser& operator=(const png_parser&);

    FILE* m_file;
    png_structp m_png_ptr;
    png_infop m_info_ptr;
    png_infop m_end_ptr;
  };

  void png_parser::close()
  {
    GVX_TRACE("png_parser::close");
    if (m_png_ptr != 0)
      {
        png_destroy_read_struct(m_png_ptr ? &m_png_ptr : 0,
                                m_info_ptr ? &m_info_ptr : 0,
                                m_end_ptr ? &m_end_ptr : 0);
      }

    if (m_file != 0)
      {
        fclose(m_file);
        m_file = 0;
      }
  }

  void png_parser::parse(const char* filename, media::bmap_data& data)
  {
    GVX_TRACE("png_parser::parse");
    m_file = fopen(filename, "rb");
    if (m_file == 0)
      {
        throw rutz::error("couldn't open file for png reading", SRC_POS);
      }

    const int nheader = 8;
    png_byte header[nheader];

    if (nheader != fread(header, 1, nheader, m_file))
      {
        throw rutz::error(rutz::sfmt("couldn't read header %s", filename), SRC_POS);
      }

    int is_png = !png_sig_cmp(header, 0, nheader);
    if (!is_png)
      {
        throw rutz::error(rutz::sfmt("%s is not a PNG image file",
                                     filename),
                          SRC_POS);
      }

    m_png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (m_png_ptr == 0)
      throw rutz::error("PNG library couldn't create read_struct",
                        SRC_POS);

    m_info_ptr = png_create_info_struct(m_png_ptr);
    if (m_info_ptr == 0)
      {
        throw rutz::error("PNG library couldn't create info_struct",
                          SRC_POS);
      }

    m_end_ptr = png_create_info_struct(m_png_ptr);
    if (m_end_ptr == 0)
      {
        throw rutz::error("PNG library couldn't create end info_struct",
                          SRC_POS);
      }

    png_init_io(m_png_ptr, m_file);

    png_set_sig_bytes(m_png_ptr, nheader);

    png_read_info(m_png_ptr, m_info_ptr);

    const int bit_depth = png_get_bit_depth(m_png_ptr, m_info_ptr);

    switch (bit_depth)
      {
      case 16:
        // Strip 16-bit pixels down to 8-bit
        png_set_strip_16(m_png_ptr);
        break;

      case 8:
        // nothing
        break;

      case 4:
      case 2:
      case 1:
        // Forces 1-, 2-, or 4-bit pixels to be expanded into full bytes.
        png_set_packing(m_png_ptr);
        break;

      default:
        throw rutz::error(rutz::sfmt("bit-depth '%d' is not supported "
                                     "for PNG images "
                                     "(must be 8- or 16-bit)",
                                     bit_depth),
                          SRC_POS);
      }

    const png_byte color_type = png_get_color_type(m_png_ptr, m_info_ptr);

    if (color_type & PNG_COLOR_MASK_ALPHA)
      png_set_strip_alpha(m_png_ptr);

    if (color_type & PNG_COLOR_MASK_PALETTE)
      png_set_palette_to_rgb(m_png_ptr);

    // This must come after any+all transformations are specified
    png_read_update_info(m_png_ptr, m_info_ptr);

    // These calls must come after read_update_info, so that we get values
    // that reflect any transformations
    const int width = png_get_image_width(m_png_ptr, m_info_ptr);
    const int height = png_get_image_height(m_png_ptr, m_info_ptr);

    dbg_eval(3, width); dbg_eval_nl(3, height);

    const size_t row_bytes = png_get_rowbytes(m_png_ptr, m_info_ptr);

    dbg_eval_nl(3, row_bytes);

    const int nchannels = png_get_channels(m_png_ptr, m_info_ptr);

    dbg_eval_nl(3, nchannels);

    GVX_ASSERT(row_bytes == size_t(width*nchannels));

    media::bmap_data new_data(geom::vec2<int>(width, height),
                              nchannels*8, // bits_per_pixel
                              1); // byte_alignment

    rutz::fixed_block<png_bytep> row_pointers(height);

    for (int i = 0; i < height; ++i)
      {
        row_pointers[i] = (png_bytep) (new_data.row_ptr(i));
      }

    png_read_image(m_png_ptr, &row_pointers[0]);

    png_read_end(m_png_ptr, m_end_ptr);

    data.swap(new_data);

    this->close();
  }

  class png_writer
  {
  public:
    png_writer() :
      m_file(0),
      m_png_ptr(0),
      m_info_ptr(0)
    {}

    ~png_writer() { close(); }

    void close();

    void write(const char* filename, const media::bmap_data& data);

  private:
    png_writer(const png_writer&);
    png_writer& operator=(const png_writer&);

    FILE* m_file;
    png_structp m_png_ptr;
    png_infop m_info_ptr;
  };

  void png_writer::close()
  {
    GVX_TRACE("png_writer::close");
    if (m_png_ptr != 0)
      {
        png_destroy_write_struct(m_png_ptr ? &m_png_ptr : 0,
                                 m_info_ptr ? &m_info_ptr : 0);
      }

    if (m_file != 0)
      {
        fclose(m_file);
        m_file = 0;
      }
  }

  int get_color_type(const media::bmap_data& data)
  {
    switch (data.bits_per_pixel())
      {
      case 1: return PNG_COLOR_TYPE_GRAY;
      case 8: return PNG_COLOR_TYPE_GRAY;
      case 24: return PNG_COLOR_TYPE_RGB;
      case 32: return PNG_COLOR_TYPE_RGB_ALPHA;
      default:
        throw rutz::error(rutz::sfmt("invalid bits_per_pixel value: %d",
                                     data.bits_per_pixel()), SRC_POS);
      }
  }

  void png_writer::write(const char* filename,
                         const media::bmap_data& data)
  {
    GVX_TRACE("png_writer::write");

    m_file = fopen(filename, "wb");
    if (m_file == 0)
      {
        throw rutz::error(rutz::sfmt("couldn't open file '%s' for "
                                     "png writing", filename), SRC_POS);
      }

    m_png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (m_png_ptr == 0)
      throw rutz::error("PNG library couldn't create write_struct",
                        SRC_POS);

    m_info_ptr = png_create_info_struct(m_png_ptr);
    if (m_info_ptr == 0)
      {
        throw rutz::error("PNG library couldn't create info_struct",
                          SRC_POS);
      }

    png_init_io(m_png_ptr, m_file);

    png_set_compression_level(m_png_ptr, 7);

    png_set_IHDR(m_png_ptr, m_info_ptr,
                 data.width(), data.height(),
                 data.bits_per_component(),
                 get_color_type(data),
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    png_write_info(m_png_ptr, m_info_ptr);

    const int height = data.height();

    rutz::fixed_block<png_bytep> row_pointers(height);

    for (int i = 0; i < height; ++i)
      {
        row_pointers[i] = static_cast<png_bytep>(data.row_ptr(i));
      }

    png_write_image(m_png_ptr, &row_pointers[0]);
    png_write_end(m_png_ptr, m_info_ptr);

    this->close();
  }

} // end anonymous namespace

void media::load_png(const char* filename, media::bmap_data& data)
{
GVX_TRACE("media::load_png");
  png_parser parser;

  parser.parse(filename, data);
}

void media::save_png(const char* filename, const media::bmap_data& data)
{
GVX_TRACE("media::save_png");
  png_writer writer;

  writer.write(filename, data);
}

#endif // HAVE_LIBPNG

#endif // !GROOVX_MEDIA_PNGPARSER_CC_UTC20050626084018_DEFINED
