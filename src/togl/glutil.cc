///////////////////////////////////////////////////////////////////////
//
// glutil.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 17:25:48 2002
// written: Sat Aug  3 17:28:39 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLUTIL_CC_DEFINED
#define GLUTIL_CC_DEFINED

#include "togl/glutil.h"

#include <GL/gl.h>

#include <cstdio>
#include <cstdlib>

#include "util/trace.h"

/*
 * Generate EPS file.
 * Contributed by Miguel A. De Riera Pasenau (miguel@DALILA.UPC.ES)
 */

/* Function that creates a EPS File from a created pixmap on the current
 * context.
 * Based on the code from Copyright (c) Mark J. Kilgard, 1996.
 * Parameters: name_file, b&w / Color flag, redraw function.
 * The redraw function is needed in order to draw things into the new
 * created pixmap.
 */

/* Copyright (c) Mark J. Kilgard, 1996. */

void* GLUtil::grabPixels(int inColor, unsigned int width, unsigned int height)
{
DOTRACE("GLUtil::grabPixels");
  GLenum format;
  unsigned int size;

  if (inColor)
    {
      format = GL_RGB;
      size = width * height * 3;
    }
  else
    {
      format = GL_COLOR_INDEX;
      /* format = GL_LUMINANCE; */
      size = width * height * 1;
    }

  GLvoid* buffer = (GLvoid *) malloc(size);
  if (buffer == NULL)
    return NULL;

  /* Save current modes. */
  GLint swapbytes, lsbfirst, rowlength;
  GLint skiprows, skippixels, alignment;

  glGetIntegerv(GL_PACK_SWAP_BYTES, &swapbytes);
  glGetIntegerv(GL_PACK_LSB_FIRST, &lsbfirst);
  glGetIntegerv(GL_PACK_ROW_LENGTH, &rowlength);
  glGetIntegerv(GL_PACK_SKIP_ROWS, &skiprows);
  glGetIntegerv(GL_PACK_SKIP_PIXELS, &skippixels);
  glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);

  /* Little endian machines (DEC Alpha for example) could
     benefit from setting GL_PACK_LSB_FIRST to GL_TRUE
     instead of GL_FALSE, but this would require changing the
     generated bitmaps too. */
  glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
  glPixelStorei(GL_PACK_LSB_FIRST, GL_FALSE);
  glPixelStorei(GL_PACK_ROW_LENGTH, 0);
  glPixelStorei(GL_PACK_SKIP_ROWS, 0);
  glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  /* Actually read the pixels. */
  glReadPixels(0, 0, width, height, format,
               GL_UNSIGNED_BYTE, (GLvoid *) buffer);

  /* Restore saved modes. */
  glPixelStorei(GL_PACK_SWAP_BYTES, swapbytes);
  glPixelStorei(GL_PACK_LSB_FIRST, lsbfirst);
  glPixelStorei(GL_PACK_ROW_LENGTH, rowlength);
  glPixelStorei(GL_PACK_SKIP_ROWS, skiprows);
  glPixelStorei(GL_PACK_SKIP_PIXELS, skippixels);
  glPixelStorei(GL_PACK_ALIGNMENT, alignment);
  return buffer;
}

int GLUtil::generateEPS(const char *filename, int inColor,
                        unsigned int width, unsigned int height)
{
DOTRACE("GLUtil::generateEPS");
  GLvoid* pixels = GLUtil::grabPixels(inColor, width, height);
  if (pixels == NULL)
    return 1;

  unsigned int components =
    inColor ? 3 /* Red, green, blue. */ : 1 /* Luminance. */;

  FILE* fp = fopen(filename, "w");
  if (fp == NULL)
    {
      return 2;
    }
  fprintf(fp, "%%!PS-Adobe-2.0 EPSF-1.2\n");
  fprintf(fp, "%%%%Creator: OpenGL pixmap render output\n");
  fprintf(fp, "%%%%BoundingBox: 0 0 %d %d\n", width, height);
  fprintf(fp, "%%%%EndComments\n");

  /* # of lines, 40 bytes per line */
  unsigned int num_lines = ((( width * height) + 7) / 8 ) / 40;
  fprintf(fp, "%%%%BeginPreview: %d %d %d %d\n%%",
          width, height, 1, num_lines);

  int pos = 0;
  unsigned char* curpix = (unsigned char*) pixels;

  {for ( unsigned int i = 0; i < width * height * components; )
    {
      unsigned char bitpixel = 0;
      if (inColor)
        {
          double pix = 0.0;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x80;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x40;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x20;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x10;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x08;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x04;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x02;
          pix = 0.30 * ( double)curpix[ i++] + 0.59 * ( double)curpix[ i++] + 0.11 * ( double)curpix[ i++];
          if ( pix > 127.0) bitpixel |= 0x01;
        }
      else
        {
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x80;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x40;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x20;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x10;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x08;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x04;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x02;
          if ( curpix[ i++] > 0x7f) bitpixel |= 0x01;
        }
      fprintf(fp, "%02hx", bitpixel);
      if (++pos >= 40)
        {
          fprintf(fp, "\n%%");
          pos = 0;
        }
    }
  }

  if (pos)
    fprintf(fp, "\n%%%%EndPreview\n");
  else
    fprintf(fp, "%%EndPreview\n");

  fprintf(fp, "gsave\n");
  fprintf(fp, "/bwproc {\n");
  fprintf(fp, "    rgbproc\n");
  fprintf(fp, "    dup length 3 idiv string 0 3 0\n");
  fprintf(fp, "    5 -1 roll {\n");
  fprintf(fp, "    add 2 1 roll 1 sub dup 0 eq\n");
  fprintf(fp, "    { pop 3 idiv 3 -1 roll dup 4 -1 roll dup\n");
  fprintf(fp, "        3 1 roll 5 -1 roll put 1 add 3 0 }\n");
  fprintf(fp, "    { 2 1 roll } ifelse\n");
  fprintf(fp, "    } forall\n");
  fprintf(fp, "    pop pop pop\n");
  fprintf(fp, "} def\n");
  fprintf(fp, "systemdict /colorimage known not {\n");
  fprintf(fp, "    /colorimage {\n");
  fprintf(fp, "        pop\n");
  fprintf(fp, "        pop\n");
  fprintf(fp, "        /rgbproc exch def\n");
  fprintf(fp, "        { bwproc } image\n");
  fprintf(fp, "    } def\n");
  fprintf(fp, "} if\n");
  fprintf(fp, "/picstr %d string def\n", width * components);
  fprintf(fp, "%d %d scale\n", width, height);
  fprintf(fp, "%d %d %d\n", width, height, 8);
  fprintf(fp, "[%d 0 0 %d 0 0]\n", width, height);
  fprintf(fp, "{currentfile picstr readhexstring pop}\n");
  fprintf(fp, "false %d\n", components);
  fprintf(fp, "colorimage\n");

  curpix = (unsigned char *) pixels;
  pos = 0;
  {for (unsigned int i = width * height * components; i > 0; i--)
    {
      fprintf(fp, "%02hx", *curpix++);
      if (++pos >= 40)
        {
          fprintf(fp, "\n");
          pos = 0;
        }
    }
  }

  if (pos)
    fprintf(fp, "\n");

  fprintf(fp, "grestore\n");
  free(pixels);
  fclose(fp);
  return 0;
}

static const char vcid_glutil_cc[] = "$Header$";
#endif // !GLUTIL_CC_DEFINED
