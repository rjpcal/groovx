///////////////////////////////////////////////////////////////////////
//
// glbmaprenderer.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec  1 17:52:41 1999
// written: Wed Sep 25 19:02:11 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBMAPRENDERER_H_DEFINED
#define GLBMAPRENDERER_H_DEFINED

#include "visx/bmaprenderer.h"

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c GLBmapRenderer is an implementation of \c BmapRenderer that
 * renders bitmap data using OpenGL. The exact OpenGL to be used can
 * be changed between \c glBitmap() and \c glDrawPixels(), by calling
 * \c setUsingGlBitmap() with \a true or \a false, respectively. \c
 * glBitmap() can only be used to render black-and-white bitmap data
 * (that is, bitmaps for which \a bits_per_pixel is 1). If the bitmap
 * data are not black-and-white, then \c glDrawPixels() will be used
 * regardless of the value of \c getUsingGlBitmap().
 *
 **/
///////////////////////////////////////////////////////////////////////

class GLBmapRenderer : public BmapRenderer
{
public:
  /// Default constructor.
  GLBmapRenderer();

  /// Virtual destructor.
  virtual ~GLBmapRenderer();

  virtual void doRender(Gfx::Canvas& canvas,
                        const Gfx::BmapData& data,
                        const Gfx::Vec2<double>& world_pos,
                        const Gfx::Vec2<double>& zoom) const;

  /// Query whether \c glBitmap() will be used to render the bitmap data.
  bool getUsingGlBitmap() const { return itsUsingGlBitmap; }

  /// Change whether \c glBitmap() will be used to render the bitmap data.
  void setUsingGlBitmap(bool val) { itsUsingGlBitmap = val; }

private:
  bool itsUsingGlBitmap;
};

static const char vcid_glbmaprenderer_h[] = "$Header$";
#endif // !GLBMAPRENDERER_H_DEFINED
