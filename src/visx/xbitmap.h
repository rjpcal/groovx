///////////////////////////////////////////////////////////////////////
//
// xitmap.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep  7 14:39:09 1999
// written: Thu Nov 21 11:48:02 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XBITMAP_H_DEFINED
#define XBITMAP_H_DEFINED

#include "visx/bitmap.h"

/// XBitmap is being phased out. New clients should just use Bitmap instead.
class XBitmap : public Bitmap
{
protected:
  /// Construct an empty bitmap.
  XBitmap();

public:
  /// Default creator.
  static XBitmap* make();

  /// Virtual destructor.
  virtual ~XBitmap();

  virtual fstring ioTypename() const;

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  // no writeTo() override since we just want Bitmap's writeTo()
};

static const char vcid_xbitmap_h[] = "$Header$";
#endif // !XBITMAP_H_DEFINED
