///////////////////////////////////////////////////////////////////////
//
// xitmap.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep  7 14:39:09 1999
// written: Wed Nov 20 17:00:23 2002
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

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;
};

static const char vcid_xbitmap_h[] = "$Header$";
#endif // !XBITMAP_H_DEFINED
