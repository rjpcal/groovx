///////////////////////////////////////////////////////////////////////
//
// xitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:39:09 1999
// written: Fri Feb 18 09:13:42 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_H_DEFINED
#define XBITMAP_H_DEFINED

#ifndef BITMAP_H_DEFINED
#include "bitmap.h"
#endif

class XBmapRenderer;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c XBitmap is a subclass of \c Bitmap that renders the bitmap data
 * using X11's drawing API's.
 *
 **/
///////////////////////////////////////////////////////////////////////

class XBitmap : public Bitmap {
public:
  // Creators

  /// Construct an empty bitmap.
  XBitmap();

  /** Create a bitmap from a graphics file. See documentation for \c
      Bitmap for a list of supported file types. */
  XBitmap(const char* filename);

  /// Construct from an \c istream by calling \c deserialize().
  XBitmap(istream& is, IOFlag flag);

  private: void init();
  
public:
  /// Virtual destructor.
  virtual ~XBitmap();

  virtual void serialize(ostream& os, IOFlag flag) const;
  virtual void deserialize(istream& is, IOFlag flag);
  virtual int charCount() const;

private:
  XBmapRenderer* itsRenderer;
};

static const char vcid_xbitmap_h[] = "$Header$";
#endif // !XBITMAP_H_DEFINED
