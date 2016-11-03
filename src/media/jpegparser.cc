/** @file media/jpegparser.cc load jpeg images (no save capability) */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Nov 11 15:16:06 2002
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

#include "jpegparser.h"

#ifdef HAVE_LIBJPEG

#include "geom/vec2.h"

#include "media/bmapdata.h"

#include "rutz/error.h"
#include "rutz/sfmt.h"

#include <csetjmp>
#include <cstdio>
extern "C"
{
#include <jpeglib.h>
}
#include "rutz/trace.h"

namespace
{
  struct jpeg_aux
  {
    FILE* infile;
    jmp_buf* jmp_state;
  };

  void cleanup(jpeg_decompress_struct* cinfo)
  {
    GVX_TRACE("<jpegparser.cc>::cleanup");

    jpeg_aux* aux = static_cast<jpeg_aux*>(cinfo->client_data);
    if (aux->infile != nullptr)
      fclose(aux->infile);
    jpeg_destroy_decompress(cinfo);
  }

  void jpeg_error_exit(j_common_ptr cinfo)
  {
    // Since we longjmp out of here, DON'T use any C++ objects that need to
    // have destructors run!

    cinfo->err->output_message(cinfo);

    jpeg_aux* aux = static_cast<jpeg_aux*>(cinfo->client_data);
    longjmp(*(aux->jmp_state), 1);
  }
}

#define SETJMP_TRY(statement)                            \
do {                                                     \
  if (setjmp(state) == 0)                                \
    {                                                    \
      statement;                                         \
    }                                                    \
  else                                                   \
    {                                                    \
      throw rutz::error(#statement " failed", SRC_POS);  \
    }                                                    \
} while (0)

void media::load_jpeg(const char* filename, media::bmap_data& data)
{
GVX_TRACE("media::load_jpeg");

  if (BITS_IN_JSAMPLE != 8)
    {
      throw rutz::error("jpeg library must be built for 8 bits-per-sample",
                        SRC_POS);
    }

  jmp_buf state;

  // 1. Allocate and initialize the JPEG decompression object
  jpeg_decompress_struct cinfo;
  jpeg_error_mgr jerr;

  jpeg_aux aux;
  aux.infile = nullptr;
  aux.jmp_state = &state;
  cinfo.client_data = static_cast<void*>(&aux);

  cinfo.err = jpeg_std_error(&jerr);
  cinfo.err->error_exit = &jpeg_error_exit;

  SETJMP_TRY(jpeg_create_decompress(&cinfo));

  // 2. Specify the source of the compressed data (i.e., the input file)
  aux.infile = fopen(filename, "rb");

  if (aux.infile == nullptr)
    {
      throw rutz::error(rutz::sfmt("couldn't open '%s' for reading",
                                   filename), SRC_POS);
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
  media::bmap_data new_data(geom::vec2<int>(cinfo.output_width,
                                            cinfo.output_height),
                            cinfo.output_components*BITS_IN_JSAMPLE,
                            1);

  while (cinfo.output_scanline < cinfo.output_height)
    {
      JSAMPLE* dest = new_data.row_ptr(cinfo.output_scanline);

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
