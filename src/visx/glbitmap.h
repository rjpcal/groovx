///////////////////////////////////////////////////////////////////////
//
// glbitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Sep  8 11:02:30 1999
// written: Wed Mar 22 16:47:05 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_H_DEFINED
#define GLBITMAP_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(BITMAP_H)
#include "bitmap.h"
#endif

class GLBmapRenderer;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c GLBitmap is a subclass of \c Bitmap that renders the bitmap data
 * using OpenGL's glDrawPixels() or glBitmap().
 *
 **/
///////////////////////////////////////////////////////////////////////

class GLBitmap : public Bitmap {
public:
  // Creators

  /// Construct an empty bitmap.
  GLBitmap();

  /** Create a bitmap from a graphics file. See documentation for \c
      Bitmap for a list of supported file types. */
  GLBitmap(const char* filename);

  /// Construct from an \c istream by calling \c deserialize().
  GLBitmap(istream& is, IOFlag flag);

  private: void init();

public:
  /// Virtual destructor.
  virtual ~GLBitmap();

  virtual void serialize(ostream& os, IOFlag flag) const;
  virtual void deserialize(istream& is, IOFlag flag);
  virtual int charCount() const;

  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  public:
  /** Query whether the bitmap will be rendered using \c glBitmap()
      (instead of \c glDrawPixels()). */
  bool getUsingGlBitmap() const;

  /// Change whether the bitmap will be rendered using \c glBitmap().
  void setUsingGlBitmap(bool val);

private:
  GLBitmap(const GLBitmap&);
  GLBitmap& operator=(const GLBitmap&);

  GLBmapRenderer* itsRenderer;
};

static const char vcid_glbitmap_h[] = "$Header$";
#endif // !GLBITMAP_H_DEFINED
