///////////////////////////////////////////////////////////////////////
//
// pbm.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jun 15 16:41:07 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef PBM_CC_DEFINED
#define PBM_CC_DEFINED

#include "pnmparser.h"

#include "geom/vec2.h"

#include "media/bmapdata.h"

#include "util/error.h"
#include "util/fstring.h"
#include "util/gzstreambuf.h"
#include "util/sharedptr.h"

#ifdef HAVE_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif
#include <cctype>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
  int modeForBitDepth(int depth)
  {
    switch (depth)
      {
      case 1: return 4; // black+white
      case 8: return 5; // grayscale
      case 24: return 6; // RGB
      }

    throw rutz::error(rutz::fstring("invalid Pbm bit depth value: ",
                                    depth), SRC_POS);

    ASSERT(0); return 0; // can't get here
  }

  int bitDepthForMode(int mode)
  {
    switch (mode)
      {
      case 1: case 4: return 1; // b+w
      case 2: case 5: return 8; // grayscale
      case 3: case 6: return 24; // RGB
      }

    throw rutz::error(rutz::fstring("invalid Pbm mode value: ", mode),
                      SRC_POS);

    ASSERT(0); return 0; // can't happen
  }

  void parsePbmMode1(STD_IO::istream& is, Gfx::BmapData& data)
  {
    DOTRACE("parsePbmMode1");

    int position = 0;
    int val = 0;

    const int byte_count = data.byteCount();

    while (is.peek() != EOF && position < byte_count)
      {
        dbg_eval(3, position);

        is >> val;
        data.bytesPtr()[position] = (val == 0) ? 0 : 255;

        ++position;
      }
  }

  void parsePbmMode23(STD_IO::istream& is, Gfx::BmapData& data, int max_grey)
  {
  DOTRACE("parsePbmMode23");

    const double scale = 255.0/double(max_grey);

    int position = 0;
    int val = 0;

    const int byte_count = data.byteCount();

    while (is.peek() != EOF && position < byte_count)
      {
        dbg_eval(3, position);

        is >> val;
        data.bytesPtr()[position] = static_cast<unsigned char>(val * scale);

        ++position;
      }
  }

  void parsePbmMode456(STD_IO::istream& is, Gfx::BmapData& data)
  {
  DOTRACE("parsePbmMode456");
    dbg_eval_nl(3, data.byteCount());
    is.read(reinterpret_cast<char*>(data.bytesPtr()), data.byteCount());
    unsigned int numread = is.gcount();
    if (numread < data.byteCount())
      throw rutz::error("stream underflow in parsePbmMode456", SRC_POS);
    if (is.fail() && !is.eof())
      throw rutz::error("input stream failed in parsePbmMode456", SRC_POS);
  }
}

///////////////////////////////////////////////////////////////////////
//
// Pbm member definitions
//
///////////////////////////////////////////////////////////////////////

void Pbm::save(const char* filename, const Gfx::BmapData& data)
{
  rutz::shared_ptr<STD_IO::ostream> os
    (rutz::ogzopen(filename, std::ios::binary));

  save(*os, data);
}

void Pbm::save(STD_IO::ostream& os, const Gfx::BmapData& data)
{
DOTRACE("Pbm::save");

  int mode = modeForBitDepth(data.bitsPerPixel());

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
    os.write(reinterpret_cast<const char*>(data.rowPtr(row)),
             data.bytesPerRow());
}

void Pbm::load(const char* filename, Gfx::BmapData& data)
{
  rutz::shared_ptr<STD_IO::istream> is
    (rutz::igzopen(filename, std::ios::binary));

  load(*is, data);
}

void Pbm::load(STD_IO::istream& is, Gfx::BmapData& data)
{
DOTRACE("Pbm::load");
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
      throw rutz::error(rutz::fstring("bad magic number while "
                                      "reading pnm file: ", c), SRC_POS);
    }

  int mode;

  is >> mode;
  dbg_eval_nl(3, mode);

  int bit_depth = bitDepthForMode(mode);

  while ( isspace(is.peek()) )
    {
      is.get();
    }
  if ( is.peek() == '#' )
    {
      is.ignore(1000, '\n');
    }

  Gfx::Vec2<int> extent;

  is >> extent.x(); dbg_eval(3, extent.x());
  is >> extent.y(); dbg_eval_nl(3, extent.y());

  int max_grey = 1;

  if (mode != 1 && mode != 4)
    {
      is >> max_grey;
    }

  dbg_eval_nl(3, max_grey);

  // read one more character of whitespace from the stream after MaxGrey
  c = is.get();
  if ( !isspace(c) )
    {
      throw rutz::error("missing whitespace while reading pnm file",
                        SRC_POS);
    }

  Gfx::BmapData new_data(extent, bit_depth, 1);

  switch (mode)
    {
    case 1:                 parsePbmMode1(is, new_data); break;
    case 2: case 3:         parsePbmMode23(is, new_data, max_grey); break;
    case 4: case 5: case 6: parsePbmMode456(is, new_data); break;
    default: ASSERT(false); break;
    }

  data.swap(new_data);
}

static const char vcid_pbm_cc[] = "$Header$";
#endif // !PBM_CC_DEFINED
