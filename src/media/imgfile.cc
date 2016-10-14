/** @file media/imgfile.cc load/save images in any (supported) format */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Apr 25 09:06:51 2002
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

#ifndef GROOVX_MEDIA_IMGFILE_CC_UTC20050626084018_DEFINED
#define GROOVX_MEDIA_IMGFILE_CC_UTC20050626084018_DEFINED

#include "imgfile.h"

#include "media/jpegparser.h"
#include "media/pngparser.h"
#include "media/pnmparser.h"

#include "nub/log.h"

#include "rutz/error.h"
#include "rutz/pipe.h"
#include "rutz/sfmt.h"

#include <cctype>
#include <iterator>
#include <vector>

#include "rutz/trace.h"

using rutz::fstring;

namespace
{
  fstring tolower(const char* inp)
  {
    std::vector<char> result;

    while (*inp != '\0')
      {
        result.push_back(char(std::tolower(*inp++)));
      }

    result.push_back('\0');

    return fstring(&result[0]);
  }

  enum image_file_type
    {
      UNKNOWN,
      JPEG,
      PNM,
      PNG,
      GIF
    };

  image_file_type get_image_file_type(const char* filename)
  {
    // Conver to lowercase so we can do case-insensitive tests of the
    // filename extension.
    fstring lowername(tolower(filename));

    if (lowername.ends_with(".pbm")
        || lowername.ends_with(".pgm")
        || lowername.ends_with(".ppm")
        || lowername.ends_with(".pnm")
        || lowername.ends_with(".pbm.gz")
        || lowername.ends_with(".pgm.gz")
        || lowername.ends_with(".ppm.gz")
        || lowername.ends_with(".pnm.gz"))
      {
        return PNM;
      }
    else if (lowername.ends_with(".png"))
      {
        return PNG;
      }
    else if (lowername.ends_with(".jpg")
             || lowername.ends_with(".jpeg"))
      {
        return JPEG;
      }
    else if (lowername.ends_with(".gif"))
      {
        return GIF;
      }
    // else...
    return UNKNOWN;
  }

#if defined(GVX_GIFTOPNM_PROG) || defined(GVX_ANYTOPNM_PROG)

  // A fallback function to try to read an image by filtering it
  // through a pipe that will convert it to PNM format.
  void pipe_load(const char* progname,
                 const char* filename, media::bmap_data& data)
  {
  GVX_TRACE("<imgfile.cc>::pipe_load");

    if (access(progname, R_OK|X_OK) != 0)
      throw rutz::error(rutz::sfmt("couldn't access program '%s'", progname), SRC_POS);
    if (access(filename, R_OK)      != 0)
      throw rutz::error(rutz::sfmt("couldn't read file '%s'", filename), SRC_POS);

    std::vector<char> nm_copy;
    std::copy(filename, filename+strlen(filename)+1,
              std::back_inserter(nm_copy));
    char* const argv[] = { (char*) progname, &nm_copy[0], (char*) 0 };

    rutz::exec_pipe p("r", argv);

    media::load_pnm(p.stream(), data);

    if (p.exit_status() != 0)
      throw rutz::error("child process exited abnormally", SRC_POS);
  }

#endif

}

void media::load_image(const char* filename, media::bmap_data& data)
{
GVX_TRACE("media::load_image");

  switch (get_image_file_type(filename))
    {
    case PNM:  media::load_pnm(filename, data); break;
    case PNG:  media::load_png(filename, data); break;
    case JPEG: media::load_jpeg(filename, data); break;
#ifdef GVX_GIFTOPNM_PROG
    case GIF:  pipe_load(GVX_GIFTOPNM_PROG, filename, data); break;
#endif
#ifdef GVX_ANYTOPNM_PROG
      // A fallback to try to read just about any image type, given
      // that the program "anytopnm" is installed on the host
      // machine. In that case, we can process the image file with
      // anytopnm, and pipe the output via a stream into a pnm parser.
    default:   pipe_load(GVX_ANYTOPNM_PROG, filename, data); break;
#else
    default:   throw rutz::error(rutz::sfmt("unknown image file format: %s",
                                            filename), SRC_POS);
      break;
#endif
    }

  nub::log(rutz::sfmt("loaded image file %s", filename));
}

void media::save_image(const char* filename,
                       const media::bmap_data& data)
{
  switch (get_image_file_type(filename))
    {
    case PNM:  media::save_pnm(filename, data); break;
    case PNG:  media::save_png(filename, data); break;
    default:
      throw rutz::error(rutz::sfmt("unknown file format: %s", filename),
                        SRC_POS);
    }

  nub::log(rutz::sfmt("saved image file %s", filename));
}

#endif // !GROOVX_MEDIA_IMGFILE_CC_UTC20050626084018_DEFINED
