///////////////////////////////////////////////////////////////////////
//
// glutil.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 17:25:48 2002
// written: Sun Aug  4 20:21:24 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLUTIL_CC_DEFINED
#define GLUTIL_CC_DEFINED

#include "togl/glutil.h"

#include <GL/gl.h>
#include <GL/glx.h>

#include <cstdio>
#include <cstdlib>

#include "util/debug.h"
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

  glReadBuffer(GL_FRONT); // by default it read GL_BACK in double buffer mode

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

//---------------------------------------------------------------------
//
// Load the named bitmap font as a sequence of bitmaps in a display list.
// fontname may be one of the predefined fonts like TOGL_BITMAP_8_BY_13, or an
// X font name.
//
//---------------------------------------------------------------------

namespace
{
  const unsigned int MAX_FONTS = 1000;
  static GLuint ListBase[MAX_FONTS];
  static GLuint ListCount[MAX_FONTS];

  const char* const DEFAULT_FONTNAME = "fixed";
}

unsigned int GLUtil::loadBitmapFont(Display* dpy, const char* fontname)
{
DOTRACE("GLUtil::loadBitmapFont");

  static int firstTime = 1;

  /* Initialize the ListBase and ListCount arrays */
  if (firstTime)
    {
      for (unsigned int i = 0; i < MAX_FONTS; ++i)
        {
          ListBase[i] = ListCount[i] = 0;
        }
      firstTime = 0;
    }

  if (fontname == 0)
    fontname = DEFAULT_FONTNAME;

  Assert( fontname );

  XFontStruct *fontinfo = XLoadQueryFont( dpy, fontname );
  if (!fontinfo)
    {
      return 0;
    }

  int first = fontinfo->min_char_or_byte2;
  int last = fontinfo->max_char_or_byte2;
  int count = last-first+1;

  GLuint fontbase = glGenLists( (GLuint) (last+1) );
  if (fontbase==0)
    {
      return 0;
    }
  glXUseXFont( fontinfo->fid, first, count, (int) fontbase+first );

  // Record the list base and number of display lists for unloadBitmapFont().
  {
    for (unsigned int i = 0; i < MAX_FONTS; ++i)
      {
        if (ListBase[i]==0)
          {
            ListBase[i] = fontbase;
            ListCount[i] = last+1;
            break;
          }
      }
  }

  return fontbase;
}

unsigned int GLUtil::loadBitmapFont(Display* dpy, NamedFont font)
{
DOTRACE("GLUtil::loadBitmapFont");

  const char* name = DEFAULT_FONTNAME;

  switch (font)
    {
    case BITMAP_FIXED:
      name = "fixed";
      break;
    case BITMAP_8_BY_13:
      name = "8x13";
      break;
    case BITMAP_9_BY_15:
      name = "9x15";
      break;
    case BITMAP_TIMES_ROMAN_10:
      name = "-adobe-times-medium-r-normal--10-100-75-75-p-54-iso8859-1";
      break;
    case BITMAP_TIMES_ROMAN_24:
      name = "-adobe-times-medium-r-normal--24-240-75-75-p-124-iso8859-1";
      break;
    case BITMAP_HELVETICA_10:
      name = "-adobe-helvetica-medium-r-normal--10-100-75-75-p-57-iso8859-1";
      break;
    case BITMAP_HELVETICA_12:
      name = "-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1";
      break;
    case BITMAP_HELVETICA_18:
      name = "-adobe-helvetica-medium-r-normal--18-180-75-75-p-98-iso8859-1";
      break;
    }

  return GLUtil::loadBitmapFont(dpy, name);
}

// Release the display lists which were generated by loadBitmapFont().
void GLUtil::unloadBitmapFont(unsigned int fontbase)
{
DOTRACE("GLUtil::unloadBitmapFont");

  for (unsigned int i = 0; i < MAX_FONTS; ++i)
    {
      DebugEvalNL(i);
      if (ListBase[i]==fontbase)
        {
          DebugEvalNL(fontbase);
          DebugEval(ListBase[i]); DebugEvalNL(ListCount[i]);
          glDeleteLists( ListBase[i], ListCount[i] );
          ListBase[i] = ListCount[i] = 0;
          return;
        }
    }
}

static const char vcid_glutil_cc[] = "$Header$";
#endif // !GLUTIL_CC_DEFINED
