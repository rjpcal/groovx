///////////////////////////////////////////////////////////////////////
//
// xitmap.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep  7 14:39:09 1999
// written: Wed Sep 25 19:03:59 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_H_DEFINED
#define XBITMAP_H_DEFINED

#include "visx/bitmap.h"
#include "visx/xbmaprenderer.h"
#include "util/pointers.h"

// Used to allow a shared_ptr to be initialized before we pass it to
// the Bitmap constructor
struct XRHolder
{
  XRHolder(shared_ptr<XBmapRenderer> p) : itsRenderer(p) {}
  virtual ~XRHolder() {}
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

class XBitmap : private XRHolder, public Bitmap
{
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
