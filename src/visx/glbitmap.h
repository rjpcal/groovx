///////////////////////////////////////////////////////////////////////
//
// glbitmap.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 11:02:30 1999
// written: Fri May 18 17:04:03 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_H_DEFINED
#define GLBITMAP_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(BITMAP_H_DEFINED)
#include "bitmap.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(GLBMAPRENDERER_H_DEFINED)
#include "glbmaprenderer.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

// Used to allow a shared_ptr to be initialized before we pass it to
// the Bitmap constructor
struct GLRHolder {
  GLRHolder(shared_ptr<GLBmapRenderer> p) : itsRenderer(p) {}
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

class GLBitmap : private GLRHolder, public Bitmap {
protected:
  /// Construct an empty bitmap.
  GLBitmap();

  /** Create a bitmap from a graphics file. See documentation for \c
      Bitmap for a list of supported file types. */
  GLBitmap(const char* filename);

public:
  /// Default creator.
  static GLBitmap* make();

  private: void init();

public:
  /// Virtual destructor.
  virtual ~GLBitmap();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  public:
  /** Query whether the bitmap will be rendered using \c glBitmap()
      (instead of \c glDrawPixels()). */
  bool getUsingGlBitmap() const;

  /// Change whether the bitmap will be rendered using \c glBitmap().
  void setUsingGlBitmap(bool val);
};

static const char vcid_glbitmap_h[] = "$Header$";
#endif // !GLBITMAP_H_DEFINED
