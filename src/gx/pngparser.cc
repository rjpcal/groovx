///////////////////////////////////////////////////////////////////////
//
// pngparser.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Apr 24 20:05:06 2002
// written: Wed Dec 25 09:01:24 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PNGPARSER_CC_DEFINED
#define PNGPARSER_CC_DEFINED

#include "pngparser.h"

#include "gx/bmapdata.h"
#include "gx/vec2.h"

#include "util/arrays.h"
#include "util/error.h"

#include <png.h>

#include <cstdio>
#include <png.h>

#include "util/debug.h"
#include "util/trace.h"

/// This class provides exception-safe cleanup for libpng.
/** It allows us to throw exceptions during our parsing routine and be
    assured that all libpng structures are properly destroyed via
    PngParser's destructor. */
class PngParser
{
public:
  PngParser() :
    itsFile(0),
    itsPngPtr(0),
    itsInfoPtr(0),
    itsEndPtr(0)
  {}

  ~PngParser() { close(); }

  void close();

  void parse(const char* filename, Gfx::BmapData& data);

private:
  PngParser(const PngParser&);
  PngParser& operator=(const PngParser&);

  FILE* itsFile;
  png_structp itsPngPtr;
  png_infop itsInfoPtr;
  png_infop itsEndPtr;
};

void PngParser::close()
{
DOTRACE("PngParser::close");
  if (itsPngPtr != 0)
    {
      png_destroy_read_struct(itsPngPtr ? &itsPngPtr : 0,
                              itsInfoPtr ? &itsInfoPtr : 0,
                              itsEndPtr ? &itsEndPtr : 0);
    }

  if (itsFile != 0)
    {
      fclose(itsFile);
      itsFile = 0;
    }
}

void PngParser::parse(const char* filename, Gfx::BmapData& data)
{
DOTRACE("PngParser::parse");
  itsFile = fopen(filename, "rb");
  if (itsFile == 0)
    {
      throw Util::Error("couldn't open file for png reading");
    }

  const int nheader = 8;
  png_byte header[nheader];

  fread(header, 1, nheader, itsFile);

  int is_png = !png_sig_cmp(header, 0, nheader);
  if (!is_png)
    {
      throw Util::Error(fstring(filename, " is not a PNG image file"));
    }

  itsPngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
  if (itsPngPtr == 0)
    throw Util::Error("PNG library couldn't create read_struct");

  itsInfoPtr = png_create_info_struct(itsPngPtr);
  if (itsInfoPtr == 0)
    {
      throw Util::Error("PNG library couldn't create info_struct");
    }

  itsEndPtr = png_create_info_struct(itsPngPtr);
  if (itsEndPtr == 0)
    {
      throw Util::Error("PNG library couldn't create end info_struct");
    }

  png_init_io(itsPngPtr, itsFile);

  png_set_sig_bytes(itsPngPtr, nheader);

  png_read_info(itsPngPtr, itsInfoPtr);

  const int bit_depth = png_get_bit_depth(itsPngPtr, itsInfoPtr);

  switch (bit_depth)
    {
    case 16:
      // Strip 16-bit pixels down to 8-bit
      png_set_strip_16(itsPngPtr);
      break;

    case 8:
      // nothing
      break;

    case 4:
    case 2:
    case 1:
      // Forces 1-, 2-, or 4-bit pixels to be expanded into full bytes.
      png_set_packing(itsPngPtr);
      break;

    default:
      throw Util::Error(fstring("bit-depth '", bit_depth,
                                "' is not supported for PNG images "
                                "(must be 8- or 16-bit)"));
    }

  const png_byte color_type = png_get_color_type(itsPngPtr, itsInfoPtr);

  if (color_type & PNG_COLOR_MASK_ALPHA)
    png_set_strip_alpha(itsPngPtr);

  if (color_type & PNG_COLOR_MASK_PALETTE)
    png_set_palette_to_rgb(itsPngPtr);

  // This must come after any+all transformations are specified
  png_read_update_info(itsPngPtr, itsInfoPtr);

  // These calls must come after read_update_info, so that we get values
  // that reflect any transformations
  const int width = png_get_image_width(itsPngPtr, itsInfoPtr);
  const int height = png_get_image_height(itsPngPtr, itsInfoPtr);

  dbgEval(3, width); dbgEvalNL(3, height);

  const int row_bytes = png_get_rowbytes(itsPngPtr, itsInfoPtr);

  dbgEvalNL(3, row_bytes);

  const int nchannels = png_get_channels(itsPngPtr, itsInfoPtr);

  dbgEvalNL(3, nchannels);

  Assert(row_bytes == width*nchannels);

  Gfx::BmapData new_data(Gfx::Vec2<int>(width, height),
                         nchannels*8, // bits_per_pixel
                         1); // byte_alignment

  fixed_block<png_bytep> row_pointers(height);

  for (int i = 0; i < height; ++i)
    {
      row_pointers[i] = (png_bytep) (new_data.rowPtr(i));
    }

  png_read_image(itsPngPtr, &row_pointers[0]);

  png_read_end(itsPngPtr, itsEndPtr);

  data.swap(new_data);

  this->close();
}

void Png::load(const char* filename, Gfx::BmapData& data)
{
DOTRACE("Png::load");
  PngParser parser;

  parser.parse(filename, data);
}

static const char vcid_pngparser_cc[] = "$Header$";
#endif // !PNGPARSER_CC_DEFINED
