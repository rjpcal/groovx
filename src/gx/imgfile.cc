///////////////////////////////////////////////////////////////////////
//
// imgfile.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Apr 25 09:06:51 2002
// written: Thu May  1 18:57:07 2003
// $Id$
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
#include <unistd.h>

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
      PNG
    };


  ImageType getImageType(const fstring& filename)
  {
    if (filename.ends_with(".pbm")
        || filename.ends_with(".pgm")
        || filename.ends_with(".ppm"))
      {
        return PNM;
      }
    else if (filename.ends_with(".png"))
      {
        return PNG;
      }
    else if (filename.ends_with(".jpg")
             || filename.ends_with(".jpeg")
             || filename.ends_with(".JPG"))
      {
        return JPEG;
      }
    // else...
    return UNKNOWN;
  }

  // A fallback function to try to read just about any image type, given
  // that the program "anytopnm" is installed on the host machine. In that
  // case, we can process the image file with anytopnm, and pipe the output
  // via a stream into a pnm parser.
  bool genericLoad(const char* filename_cstr, Gfx::BmapData& data)
  {
  DOTRACE("<imgfile.cc>::genericLoad");

#ifndef ANYTOPNM_PROG
    return false;
#else

    if (access(filename_cstr, R_OK)      != 0) return false;
    if (access(ANYTOPNM_PROG, R_OK|X_OK) != 0) return false;

    fstring cmd(ANYTOPNM_PROG, " ", filename_cstr);

    Util::Pipe pipe(cmd.c_str(), "r");

    Pbm::load(pipe.stream(), data);

    pipe.close();

    if (pipe.exitStatus() != 0)
      return false;

    return true;
#endif
  }
}

void ImgFile::load(const char* filename_cstr, Gfx::BmapData& data)
{
DOTRACE("ImgFile::load");

  fstring filename(tolower(filename_cstr));

  switch (getImageType(filename))
    {
    case PNM:  Pbm::load(filename_cstr, data); break;
    case PNG:  Png::load(filename_cstr, data); break;
    case JPEG: Jpeg::load(filename_cstr, data); break;
    default:
      if (!genericLoad(filename_cstr, data))
        throw Util::Error(fstring("unknown file format: ", filename_cstr));
    }

  Util::log(fstring("loaded image file ", filename_cstr));
}

void ImgFile::save(const char* filename_cstr, const Gfx::BmapData& data)
{
  fstring filename(tolower(filename_cstr));

  switch (getImageType(filename))
    {
    case PNM:  Pbm::save(filename_cstr, data); break;
    default:
      throw Util::Error(fstring("unknown file format: ", filename_cstr));
    }

  Util::log(fstring("saved image file ", filename_cstr));
}

static const char vcid_imgfile_cc[] = "$Header$";
#endif // !IMGFILE_CC_DEFINED
