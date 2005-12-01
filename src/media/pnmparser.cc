/** @file media/pnmparser.cc load/save pnm images (i.e., pbm, pgm, ppm) */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Jun 15 16:41:07 1999
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

#ifndef GROOVX_MEDIA_PNMPARSER_CC_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_PNMPARSER_CC_UTC20050626084018_DEFINED

#include "pnmparser.h"

#include "geom/vec2.h"

#include "media/bmapdata.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/gzstreambuf.h"
#include "rutz/sharedptr.h"

#include <cctype>
#include <iostream>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  int mode_for_bit_depth(int depth)
  {
    switch (depth)
      {
      case 1: return 4; // black+white
      case 8: return 5; // grayscale
      case 24: return 6; // RGB
      }

    throw rutz::error(rutz::cat("invalid PNM bit depth value: ",
                                depth), SRC_POS);

    GVX_ASSERT(0); return 0; // can't get here
  }

  int bit_depth_for_mode(int mode)
  {
    switch (mode)
      {
      case 1: case 4: return 1; // b+w
      case 2: case 5: return 8; // grayscale
      case 3: case 6: return 24; // RGB
      }

    throw rutz::error(rutz::cat("invalid PNM mode value: ", mode),
                      SRC_POS);

    GVX_ASSERT(0); return 0; // can't happen
  }

  void parse_pbm_mode_1(std::istream& is, media::bmap_data& data)
  {
    GVX_TRACE("parse_pbm_mode_1");

    int position = 0;
    int val = 0;

    const int byte_count = data.byte_count();

    while (is.peek() != EOF && position < byte_count)
      {
        dbg_eval(3, position);

        is >> val;
        data.bytes_ptr()[position] = (val == 0) ? 0 : 255;

        ++position;
      }
  }

  void parse_pbm_mode_23(std::istream& is, media::bmap_data& data,
                         int max_grey)
  {
  GVX_TRACE("parse_pbm_mode_23");

    const double scale = 255.0/double(max_grey);

    int position = 0;
    int val = 0;

    const int byte_count = data.byte_count();

    while (is.peek() != EOF && position < byte_count)
      {
        dbg_eval(3, position);

        is >> val;
        data.bytes_ptr()[position] = static_cast<unsigned char>(val * scale);

        ++position;
      }
  }

  void parse_pbm_mode_456(std::istream& is,
                          media::bmap_data& data,
                          int max_grey,
                          int mode)
  {
  GVX_TRACE("parse_pbm_mode_456");

    dbg_eval_nl(3, data.byte_count());

    if (max_grey == 255 // the most common case
        || mode == 4)
      {
        is.read(reinterpret_cast<char*>(data.bytes_ptr()), data.byte_count());
        unsigned int numread = is.gcount();
        if (numread < data.byte_count())
          throw rutz::error("stream underflow in parse_pbm_mode_456", SRC_POS);
      }
    else
      {
        int nbits = 0;
        for (int g = max_grey; g != 0; g >>= 1)
          {
            ++nbits;
          }

        dbg_eval_nl(3, nbits);

        GVX_ASSERT(nbits >= 1);

        const int nbytes = ((nbits - 1) / 8) + 1;

        dbg_eval_nl(3, nbytes);

        unsigned char* p = data.bytes_ptr();
        unsigned char* stop = data.bytes_ptr() + data.byte_count();

        while (p != stop)
          {
            int val = 0;
            for (int i = 0; i < nbytes; ++i)
              {
                int c = is.get();
                if (c == EOF)
                  throw rutz::error("premature EOF while reading pnm file", SRC_POS);
                val += (c << (8*i));
              }

            *p = (unsigned char)(255*double(val)/double(max_grey));
            ++p;
          }
      }

    if (is.fail() && !is.eof())
      throw rutz::error("input stream failed in parse_pbm_mode_456", SRC_POS);
  }
}

///////////////////////////////////////////////////////////////////////
//
// PNM load/save definitions
//
///////////////////////////////////////////////////////////////////////

void media::save_pnm(const char* filename, const media::bmap_data& data)
{
  rutz::shared_ptr<std::ostream> os
    (rutz::ogzopen(filename, std::ios::binary));

  save_pnm(*os, data);
}

void media::save_pnm(std::ostream& os, const media::bmap_data& data)
{
GVX_TRACE("media::save_pnm");

  int mode = mode_for_bit_depth(data.bits_per_pixel());

  os << 'P' << mode
     << ' ' << data.size().x()
     << ' ' << data.size().y();

  if (mode != 1 && mode != 4)
    {
      const int max_grey = 255;
      os << ' ' << max_grey;
    }

  os << '\n';

  for (int row = 0; row < data.height(); ++row)
    os.write(reinterpret_cast<const char*>(data.row_ptr(row)),
             data.bytes_per_row());
}

void media::load_pnm(const char* filename, media::bmap_data& data)
{
  rutz::shared_ptr<std::istream> is
    (rutz::igzopen(filename, std::ios::binary));

  load_pnm(*is, data);
}

void media::load_pnm(std::istream& is, media::bmap_data& data)
{
GVX_TRACE("media::load_pnm");
  if (is.fail())
    {
      throw rutz::error("input stream failed before reading pnm file",
                        SRC_POS);
    }

  if (is.peek() == EOF)
    {
      throw rutz::error("input stream empty while reading pnm file",
                        SRC_POS);
    }

  int c = is.get();
  if (c != 'P')
    {
      throw rutz::error(rutz::cat("bad magic number while "
                                  "reading pnm file: ", c), SRC_POS);
    }

  int mode;

  is >> mode;
  dbg_eval_nl(3, mode);

  const int bit_depth = bit_depth_for_mode(mode);

  while ( isspace(is.peek()) )
    {
      is.get();
    }
  if ( is.peek() == '#' )
    {
      is.ignore(1000, '\n');
    }

  geom::vec2<int> extent;

  is >> extent.x();
  is >> extent.y();

  dbg_dump(3, extent);

  int max_grey = 1;

  if (mode != 1 && mode != 4)
    {
      is >> max_grey;
    }

  dbg_eval_nl(3, max_grey);

  if (max_grey < 1)
    {
      throw rutz::error(rutz::cat("while reading pnm file: "
                                  "invalid max grey value: ",
                                  max_grey), SRC_POS);
    }

  // one more character of whitespace after max_grey
  c = is.get();
  if ( !isspace(c) )
    {
      throw rutz::error("missing whitespace while reading pnm file",
                        SRC_POS);
    }

  media::bmap_data new_data(extent, bit_depth, 1);

  switch (mode)
    {
    case 1:                 parse_pbm_mode_1(is, new_data); break;
    case 2: case 3:         parse_pbm_mode_23(is, new_data, max_grey); break;
    case 4: case 5: case 6: parse_pbm_mode_456(is, new_data, max_grey, mode); break;
    default: GVX_ASSERT(false); break;
    }

  data.swap(new_data);
}

static const char vcid_groovx_media_pnmparser_cc_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_MEDIA_PNMPARSER_CC_UTC20050626084018_DEFINED
