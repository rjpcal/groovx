///////////////////////////////////////////////////////////////////////
//
// xitmap.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep  7 14:39:09 1999
// written: Fri Aug 10 12:38:46 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_H_DEFINED
#define XBITMAP_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(BITMAP_H_DEFINED)
#include "bitmap.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(XBMAPRENDERER_H_DEFINED)
#include "xbmaprenderer.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

// Used to allow a shared_ptr to be initialized before we pass it to
// the Bitmap constructor
struct XRHolder {
  XRHolder(shared_ptr<XBmapRenderer> p) : itsRenderer(p) {}
  shared_ptr<XBmapRenderer> itsRenderer;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c XBitmap is a subclass of \c Bitmap that renders the bitmap data
 * using X11's drawing API's.
 *
 **/
///////////////////////////////////////////////////////////////////////

class XBitmap : private XRHolder, public Bitmap {
protected:
  /// Construct an empty bitmap.
  XBitmap();

public:
  /// Default creator.
  static XBitmap* make();

  /// Virtual destructor.
  virtual ~XBitmap();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;
};

static const char vcid_xbitmap_h[] = "$Header$";
#endif // !XBITMAP_H_DEFINED
