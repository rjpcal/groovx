///////////////////////////////////////////////////////////////////////
//
// glbitmap.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 11:02:30 1999
// written: Wed Jun 26 12:05:48 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_H_DEFINED
#define GLBITMAP_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(BITMAP_H_DEFINED)
#include "visx/bitmap.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GLBMAPRENDERER_H_DEFINED)
#include "visx/glbmaprenderer.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

/// Allows a shared_ptr to be initialized before passing it to the Bitmap constructor.
struct GLRHolder
{
  GLRHolder(shared_ptr<GLBmapRenderer> p) : itsRenderer(p) {}
  virtual ~GLRHolder() {}
  shared_ptr<GLBmapRenderer> itsRenderer;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c GLBitmap is a subclass of \c Bitmap that renders the bitmap data
 * using OpenGL's glDrawPixels() or glBitmap().
 *
 **/
///////////////////////////////////////////////////////////////////////

class GLBitmap : private GLRHolder, public Bitmap
{
protected:
  /// Construct an empty bitmap.
  GLBitmap();

public:
  /// Default creator.
  static GLBitmap* make();

public:
  /// Virtual destructor.
  virtual ~GLBitmap();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  /** Query whether the bitmap will be rendered using \c glBitmap()
      (instead of \c glDrawPixels()). */
  bool getUsingGlBitmap() const;

  /// Change whether the bitmap will be rendered using \c glBitmap().
  void setUsingGlBitmap(bool val);
};

static const char vcid_glbitmap_h[] = "$Header$";
#endif // !GLBITMAP_H_DEFINED
