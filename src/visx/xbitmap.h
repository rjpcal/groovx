///////////////////////////////////////////////////////////////////////
//
// xitmap.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep  7 14:39:09 1999
// written: Tue Sep 26 19:02:10 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_H_DEFINED
#define XBITMAP_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(BITMAP_H_DEFINED)
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

#ifdef LEGACY
  /// Construct from an \c STD_IO::istream by calling \c legacyDesrlz().
  XBitmap(STD_IO::istream& is, IO::IOFlag flag);
#endif

  private: void init();
  
public:
  /// Virtual destructor.
  virtual ~XBitmap();

  virtual void legacySrlz(IO::Writer* writer, STD_IO::ostream& os, IO::IOFlag flag) const;
  virtual void legacyDesrlz(IO::Reader* reader, STD_IO::istream& is, IO::IOFlag flag);
  virtual int legacyCharCount() const;

private:
  XBitmap(const XBitmap&);
  XBitmap& operator=(const XBitmap&);

  XBmapRenderer* itsRenderer;
};

static const char vcid_xbitmap_h[] = "$Header$";
#endif // !XBITMAP_H_DEFINED
