///////////////////////////////////////////////////////////////////////
//
// imgfile.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Apr 25 09:06:51 2002
// written: Mon Jan 13 11:01:38 2003
// $Id$
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
