///////////////////////////////////////////////////////////////////////
//
// glutil.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 17:25:48 2002
// written: Tue Nov 12 17:33:41 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLUTIL_CC_DEFINED
#define GLUTIL_CC_DEFINED

#include "togl/glutil.h"

#include "util/error.h"

#include <GL/gl.h>
#include <GL/glx.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "util/debug.h"
#include "util/trace.h"

static void
dump_bitmap (unsigned int width, unsigned int height, GLubyte *bitmap)
{
  unsigned int x, y;

  printf ("    ");
  for (x = 0; x < 8*width; x++)
    printf ("%o", 7 - (x % 8));
  printf ("\n");
  for (y = 0; y < height; y++)
    {
      printf ("%3o:", y);
      for (x = 0; x < 8*width; x++)
        printf ((bitmap[width*(height - y - 1) + x/8] & (1 << (7 - (x %
8))))
                 ? "*" : ".");
      printf ("   ");
      for (x = 0; x < width; x++)
        printf ("0x%02x, ", bitmap[width*(height - y - 1) + x]);
      printf ("\n");
    }
}

/* Implementation.  */

/* Fill a BITMAP with a character C from thew current font
   in the graphics context GC.  WIDTH is the width in bytes
   and HEIGHT is the height in bits.

   Note that the generated bitmaps must be used with

        glPixelStorei (GL_UNPACK_SWAP_BYTES, GL_FALSE);
        glPixelStorei (GL_UNPACK_LSB_FIRST, GL_FALSE);
        glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
        glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

   Possible optimizations:

     * use only one reusable pixmap with the maximum dimensions.
     * draw the entire font into a single pixmap (careful with
       proportional fonts!).
*/


/*
 * Generate OpenGL-compatible bitmap.
 */
static void
fill_bitmap (Display *dpy, Window win, GC gc,
             unsigned int width, unsigned int height,
             int x0, int y0, unsigned int c, GLubyte *bitmap)
{
  XImage *image;
  unsigned int x, y;
  Pixmap pixmap;
  XChar2b char2b;

  pixmap = XCreatePixmap (dpy, win, 8*width, height, 1);
  XSetForeground(dpy, gc, 0);
  XFillRectangle (dpy, pixmap, gc, 0, 0, 8*width, height);
  XSetForeground(dpy, gc, 1);

  char2b.byte1 = (c >> 8) & 0xff;
  char2b.byte2 = (c & 0xff);

  XDrawString16 (dpy, pixmap, gc, x0, y0, &char2b, 1);

  image = XGetImage (dpy, pixmap, 0, 0, 8*width, height, 1, XYPixmap);
  if (image) {
    /* Fill the bitmap (X11 and OpenGL are upside down wrt each other).  */
    for (y = 0; y < height; y++)
      for (x = 0; x < 8*width; x++)
        if (XGetPixel (image, x, y))
          bitmap[width*(height - y - 1) + x/8] |= (1 << (7 - (x % 8)));
    XDestroyImage (image);
  }

  XFreePixmap (dpy, pixmap);
}

/*
 * determine if a given glyph is valid and return the
 * corresponding XCharStruct.
 */
static XCharStruct *isvalid(XFontStruct *fs, int which)
{
  unsigned int  rows,pages;
  int           byte1 = 0, byte2 = 0;
  int           i,valid = 1;

  rows = fs->max_byte1 - fs->min_byte1 + 1;
  pages = fs->max_char_or_byte2 - fs->min_char_or_byte2 + 1;

  if (rows == 1) {
    /* "linear" fonts */
    if ((fs->min_char_or_byte2 > which) ||
        (fs->max_char_or_byte2 < which)) valid = 0;
  } else {
    /* "matrix" fonts */
    byte2 = which & 0xff;
    byte1 = which >> 8;
    if ((fs->min_char_or_byte2 > byte2) ||
        (fs->max_char_or_byte2 < byte2) ||
        (fs->min_byte1 > byte1) ||
        (fs->max_byte1 < byte1)) valid = 0;
  }

  if (valid) {
    if (fs->per_char) {
      if (rows == 1) {
        /* "linear" fonts */
        return(fs->per_char + (which-fs->min_char_or_byte2) );
      } else {
        /* "matrix" fonts */
        i = ((byte1 - fs->min_byte1) * pages) +
             (byte2 - fs->min_char_or_byte2);
        return(fs->per_char + i);
      }
    } else {
        return(&fs->min_bounds);
    }
  }
  return(NULL);
}


void myUseXFont( Display* dpy, Window win, Font font, int first, int count, int listbase )
{
  GLXContext CC;
  Pixmap pixmap;
  GC gc;
  XGCValues values;
  unsigned long valuemask;
  XFontStruct *fs;

  GLint swapbytes, lsbfirst, rowlength;
  GLint skiprows, skippixels, alignment;

  unsigned int max_width, max_height, max_bm_width, max_bm_height;
  GLubyte *bm;

  int i;

  fs = XQueryFont (dpy, font);
  if (!fs)
    {
      throw Util::Error("bad font");
      return;
    }

  /* Allocate a bitmap that can fit all characters.  */
  max_width = fs->max_bounds.rbearing - fs->min_bounds.lbearing;
  max_height = fs->max_bounds.ascent + fs->max_bounds.descent;
  max_bm_width = (max_width + 7) / 8;
  max_bm_height = max_height;

  bm = (GLubyte *) malloc((max_bm_width * max_bm_height) * sizeof(GLubyte));
  if (!bm) {
      XFreeFontInfo( NULL, fs, 1 );
      throw Util::Error("out of memory");
      return;
    }

  /* Save the current packing mode for bitmaps.  */
  glGetIntegerv (GL_UNPACK_SWAP_BYTES, &swapbytes);
  glGetIntegerv (GL_UNPACK_LSB_FIRST, &lsbfirst);
  glGetIntegerv (GL_UNPACK_ROW_LENGTH, &rowlength);
  glGetIntegerv (GL_UNPACK_SKIP_ROWS, &skiprows);
  glGetIntegerv (GL_UNPACK_SKIP_PIXELS, &skippixels);
  glGetIntegerv (GL_UNPACK_ALIGNMENT, &alignment);

  /* Enforce a standard packing mode which is compatible with
     fill_bitmap() from above.  This is actually the default mode,
     except for the (non)alignment.  */
  glPixelStorei (GL_UNPACK_SWAP_BYTES, GL_FALSE);
  glPixelStorei (GL_UNPACK_LSB_FIRST, GL_FALSE);
  glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
  glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

  pixmap = XCreatePixmap (dpy, win, 10, 10, 1);
  values.foreground = BlackPixel (dpy, DefaultScreen (dpy));
  values.background = WhitePixel (dpy, DefaultScreen (dpy));
  values.font = fs->fid;
  valuemask = GCForeground | GCBackground | GCFont;
  gc = XCreateGC (dpy, pixmap, valuemask, &values);
  XFreePixmap (dpy, pixmap);

  for (i = 0; i < count; i++)
//   for (i = 48; i < 49; i++)
    {
      unsigned int width, height, bm_width, bm_height;
      GLfloat x0, y0, dx, dy;
      XCharStruct *ch;
      int x, y;
      unsigned int c = first + i;
      int list = listbase + i;
      int valid;

      /* check on index validity and get the bounds */
      ch = isvalid(fs, c);
      if (!ch) {
        ch = &fs->max_bounds;
        valid = 0;
      } else {
        valid = 1;
      }

      dbgEval(2, list); dbgEval(2, c); dbgEvalNL(2, valid);

      /* glBitmap()' parameters:
         straight from the glXUseXFont(3) manpage.  */
      width = ch->rbearing - ch->lbearing;
      height = ch->ascent + ch->descent;
      x0 = - ch->lbearing;
      y0 = ch->descent - 1;
      dx = ch->width;
      dy = 0;

      /* X11's starting point.  */
      x = - ch->lbearing;
      y = ch->ascent;

      /* Round the width to a multiple of eight.  We will use this also
         for the pixmap for capturing the X11 font.  This is slightly
         inefficient, but it makes the OpenGL part real easy.  */
      bm_width = (width + 7) / 8;
      bm_height = height;

      double p = 4*double(i-48)/42.0 - 2.0;

      glBegin(GL_LINES);
      glVertex2d(0.0, 0.0);
      glVertex2d(0.9, p);
      glEnd();
      glFlush();

      glRasterPos2d(1.25, p);

      glNewList (list, GL_COMPILE_AND_EXECUTE);
        if (valid && (bm_width > 0) && (bm_height > 0)) {

            memset (bm, '\0', bm_width * bm_height);
            fill_bitmap (dpy, win, gc, bm_width, bm_height, x, y, c, bm);

            glBitmap (width, height, x0, y0, dx, dy, bm);
            dbgEval(2, width); dbgEvalNL(2, height);
            dump_bitmap(bm_width, bm_height, bm);
          } else {
            glBitmap (0, 0, 0.0, 0.0, dx, dy, NULL);
          }
      glEndList ();

      glRasterPos2d(1.0, p);

      if (valid && (bm_width > 0) && (bm_height > 0))
        glBitmap (width, height, x0, y0, dx, dy, bm);
      else
        glBitmap (0, 0, 0.0, 0.0, dx, dy, NULL);

      glRasterPos2d(1.5, p);
      glCallList(list);

      glFlush();
    }

  free(bm);
  XFreeFontInfo( NULL, fs, 1 );
  XFreeGC (dpy, gc);

  /* Restore saved packing modes.  */
  glPixelStorei(GL_UNPACK_SWAP_BYTES, swapbytes);
  glPixelStorei(GL_UNPACK_LSB_FIRST, lsbfirst);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, rowlength);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, skiprows);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, skippixels);
  glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
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

unsigned int GLUtil::loadBitmapFont(Display* dpy, GLXContext ctx, const char* fontname)
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
  dbgEval(2, fontinfo); dbgEvalNL(2, fontinfo->fid);
  if (!fontinfo)
    {
      return 0;
    }

  const int first = fontinfo->min_char_or_byte2;
  dbgEval(2, first);
  const int last = fontinfo->max_char_or_byte2;
  dbgEval(2, last);
  const int count = last-first+1;
  dbgEvalNL(2, count);

  GLuint fontbase = glGenLists( (GLuint) (last+1) );
  dbgEvalNL(2, fontbase);
  if (fontbase==0)
    {
      return 0;
    }

//   myUseXFont( dpy, glXGetCurrentDrawable(), fontinfo->fid, first, count, (int) fontbase+first );
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

  for (int l = first; l < first+count; ++l)
    {
      double p = 4*double((l-first-1)-48)/42.0 - 2.0;
      glRasterPos2d(-1.0, p);
      glCallList(l);
    }
  glFlush();

  return fontbase;
}

unsigned int GLUtil::loadBitmapFont(Display* dpy, GLXContext ctx, NamedFont font)
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

  return GLUtil::loadBitmapFont(dpy, ctx, name);
}

// Release the display lists which were generated by loadBitmapFont().
void GLUtil::unloadBitmapFont(unsigned int fontbase)
{
DOTRACE("GLUtil::unloadBitmapFont");

  for (unsigned int i = 0; i < MAX_FONTS; ++i)
    {
      dbgEvalNL(3, i);
      if (ListBase[i]==fontbase)
        {
          dbgEvalNL(3, fontbase);
          dbgEval(3, ListBase[i]); dbgEvalNL(3, ListCount[i]);
          glDeleteLists( ListBase[i], ListCount[i] );
          ListBase[i] = ListCount[i] = 0;
          return;
        }
    }
}

static const char vcid_glutil_cc[] = "$Header$";
#endif // !GLUTIL_CC_DEFINED
