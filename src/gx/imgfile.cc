///////////////////////////////////////////////////////////////////////
//
// imgfile.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Apr 25 09:06:51 2002
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

#ifndef IMGFILE_CC_DEFINED
#define IMGFILE_CC_DEFINED

#include "imgfile.h"

#include "gx/jpegparser.h"
#include "gx/pbm.h"
#include "gx/pngparser.h"

#include "util/error.h"
#include "util/log.h"
#include "util/pipe.h"

#include <cctype>

#include "util/trace.h"

namespace
{
  fstring tolower(const char* inp)
  {
    fstring result;

    while (*inp != '\0')
      {
        result.append(char(std::tolower(*inp++)));
      }

    return result;
  }

  enum ImageType
    {
      UNKNOWN,
      JPEG,
      PNM,
      PNG,
      GIF
    };


  ImageType getImageType(const char* filename)
  {
    // Conver to lowercase so we can do case-insensitive tests of the
    // filename extension.
    fstring lowername(tolower(filename));

    if (lowername.ends_with(".pbm")
        || lowername.ends_with(".pgm")
        || lowername.ends_with(".ppm")
        || lowername.ends_with(".pnm"))
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

  // A fallback function to try to read an image by filtering it
  // through a pipe that will convert it to PNM format.
  void pipeLoad(const char* progname,
                const char* filename, Gfx::BmapData& data)
  {
  DOTRACE("<imgfile.cc>::pipeLoad");

    if (access(progname, R_OK|X_OK) != 0)
      throw rutz::error(fstring("couldn't access program '", progname, "'"), SRC_POS);
    if (access(filename, R_OK)      != 0)
      throw rutz::error(fstring("couldn't read file '", filename, "'"), SRC_POS);

    fstring nm_copy(filename);
    char* const argv[] = { (char*) progname, nm_copy.data(), (char*) 0 };

    Util::ExecPipe p("r", argv);

    Pbm::load(p.stream(), data);

    if (p.exitStatus() != 0)
      throw rutz::error("child process exited abnormally", SRC_POS);
  }
}

void ImgFile::load(const char* filename, Gfx::BmapData& data)
{
DOTRACE("ImgFile::load");

  switch (getImageType(filename))
    {
    case PNM:  Pbm::load(filename, data); break;
    case PNG:  Png::load(filename, data); break;
    case JPEG: Jpeg::load(filename, data); break;
#ifdef GIFTOPNM_PROG
    case GIF:  pipeLoad(GIFTOPNM_PROG, filename, data); break;
#endif
#ifdef ANYTOPNM_PROG
      // A fallback to try to read just about any image type, given
      // that the program "anytopnm" is installed on the host
      // machine. In that case, we can process the image file with
      // anytopnm, and pipe the output via a stream into a pnm parser.
    default:   pipeLoad(ANYTOPNM_PROG, filename, data); break;
#else
    default:   throw rutz::error(fstring("unknown image file format: ",
                                         filename), SRC_POS);
      break;
#endif
    }

  Util::log(fstring("loaded image file ", filename));
}

void ImgFile::save(const char* filename, const Gfx::BmapData& data)
{
  switch (getImageType(filename))
    {
    case PNM:  Pbm::save(filename, data); break;
    case PNG:  Png::save(filename, data); break;
    default:
      throw rutz::error(fstring("unknown file format: ", filename), SRC_POS);
    }

  Util::log(fstring("saved image file ", filename));
}

static const char vcid_imgfile_cc[] = "$Header$";
#endif // !IMGFILE_CC_DEFINED
