///////////////////////////////////////////////////////////////////////
//
// jpegparser.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Nov 11 15:16:06 2002
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

#ifndef JPEGPARSER_CC_DEFINED
#define JPEGPARSER_CC_DEFINED

#include "jpegparser.h"

#ifndef HAVE_LIBJPEG

#include "util/error.h"

void Jpeg::load(const char* /*filename*/, Gfx::BmapData& /*data*/)
{
  throw Util::Error("jpeg image files are not supported in this build");
}

#else

#include "gx/bmapdata.h"
#include "gx/vec2.h"

#include "util/error.h"

#include <csetjmp>
#include <cstdio>
extern "C"
{
#include <jpeglib.h>
}
#include "util/trace.h"

namespace
{
  struct JpegAux
  {
    FILE* infile;
    jmp_buf* jmp_state;
  };

  void cleanup(jpeg_decompress_struct* cinfo)
  {
    DOTRACE("<jpegparser.cc>::cleanup");

    JpegAux* aux = static_cast<JpegAux*>(cinfo->client_data);
    if (aux->infile != 0)
      fclose(aux->infile);
    jpeg_destroy_decompress(cinfo);
  }

  void jpegErrorExit(j_common_ptr cinfo)
  {
    // Since we longjmp out of here, DON'T use any C++ objects that need to
    // have destructors run!

    cinfo->err->output_message(cinfo);

    JpegAux* aux = static_cast<JpegAux*>(cinfo->client_data);
    longjmp(*(aux->jmp_state), 1);
  }
}

#define SETJMP_TRY(statement)                   \
do {                                            \
  if (setjmp(state) == 0)                       \
    {                                           \
      statement;                                \
    }                                           \
  else                                          \
    {                                           \
      throw Util::Error(#statement " failed");  \
    }                                           \
} while (0)

void Jpeg::load(const char* filename, Gfx::BmapData& data)
{
DOTRACE("Jpeg::load");

  if (BITS_IN_JSAMPLE != 8)
    {
      throw Util::Error("jpeg library must be built for 8 bits-per-sample");
    }

  jmp_buf state;

  // 1. Allocate and initialize the JPEG decompression object
  jpeg_decompress_struct cinfo;
  jpeg_error_mgr jerr;

  JpegAux aux;
  aux.infile = 0;
  aux.jmp_state = &state;
  cinfo.client_data = static_cast<void*>(&aux);

  cinfo.err = jpeg_std_error(&jerr);
  cinfo.err->error_exit = &jpegErrorExit;

  SETJMP_TRY(jpeg_create_decompress(&cinfo));

  // 2. Specify the source of the compressed data (i.e., the input file)
  aux.infile = fopen(filename, "rb");

  if (aux.infile == 0)
    {
      throw Util::Error(fstring("couldn't open '", filename, "' for reading"));
    }


  SETJMP_TRY(jpeg_stdio_src(&cinfo, aux.infile));

  // 3. Call jpeg_read_header() to obtain image info
  SETJMP_TRY(jpeg_read_header(&cinfo, TRUE));

  // 4. Set parameters for decompression

  // 5. Start decompression
  SETJMP_TRY(jpeg_start_decompress(&cinfo));

  // cinfo.output_width
  // cinfo.output_height
  // cinfo.out_color_components
  // cinfo.output_components

  // 6. Read scanlines
  Gfx::BmapData new_data(Gfx::Vec2<int>(cinfo.output_width,
                                        cinfo.output_height),
                         cinfo.output_components*BITS_IN_JSAMPLE,
                         1);

  while (cinfo.output_scanline < cinfo.output_height)
    {
      JSAMPLE* dest = new_data.rowPtr(cinfo.output_scanline);

      SETJMP_TRY(jpeg_read_scanlines(&cinfo,
                                     &dest,
                                     /* max lines */ 1));
    }

  // 7. finish decompression
  SETJMP_TRY(jpeg_finish_decompress(&cinfo));

  // 8. cleanup
  cleanup(&cinfo);

  data.swap(new_data);
}

#endif // HAVE_LIBJPEG

static const char vcid_jpegparser_cc[] = "$Header$";
#endif // !JPEGPARSER_CC_DEFINED
