///////////////////////////////////////////////////////////////////////
//
// jpegparser.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov 11 15:16:06 2002
// written: Mon Nov 11 16:52:09 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JPEGPARSER_CC_DEFINED
#define JPEGPARSER_CC_DEFINED

#include "jpegparser.h"

#include "gx/bmapdata.h"
#include "gx/vec2.h"

#include "util/error.h"

#include <cstdio>
#include <jpeglib.h>
#include <setjmp.h>

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
    DOTRACE("<jpegparser.cc>::jpegErrorExit");

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
  FILE* ff = fopen(filename, "rb");

  if (ff == 0)
    {
      throw Util::Error(fstring("couldn't open '", filename, "' for reading"));
    }


  SETJMP_TRY(jpeg_stdio_src(&cinfo, ff));

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

static const char vcid_jpegparser_cc[] = "$Header$";
#endif // !JPEGPARSER_CC_DEFINED
