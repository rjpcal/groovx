///////////////////////////////////////////////////////////////////////
//
// imgfile.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Apr 25 09:06:51 2002
// written: Wed Mar 19 17:55:57 2003
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
      PBM,
      PNG
    };


  ImageType getImageType(const fstring& filename)
  {
    if (filename.ends_with(".pbm")
        || filename.ends_with(".pgm")
        || filename.ends_with(".ppm"))
      {
        return PBM;
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
}

void ImgFile::load(const char* filename_cstr, Gfx::BmapData& data)
{
DOTRACE("ImgFile::load");

  fstring filename(tolower(filename_cstr));

  switch (getImageType(filename))
    {
    case PBM:  Pbm::load(filename_cstr, data); break;
    case PNG:  Png::load(filename_cstr, data); break;
    case JPEG: Jpeg::load(filename_cstr, data); break;
    default:
      throw Util::Error(fstring("unknown file format: ", filename_cstr));
    }

  Util::log(fstring("loaded image file ", filename_cstr));
}

void ImgFile::save(const char* filename_cstr, const Gfx::BmapData& data)
{
  fstring filename(tolower(filename_cstr));

  switch (getImageType(filename))
    {
    case PBM:  Pbm::save(filename_cstr, data); break;
    default:
      throw Util::Error(fstring("unknown file format: ", filename_cstr));
    }

  Util::log(fstring("saved image file ", filename_cstr));
}

static const char vcid_imgfile_cc[] = "$Header$";
#endif // !IMGFILE_CC_DEFINED
