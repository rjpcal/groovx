///////////////////////////////////////////////////////////////////////
//
// glbitmap.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 11:02:30 1999
// written: Thu Nov 21 11:48:48 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLBITMAP_H_DEFINED
#define GLBITMAP_H_DEFINED

#include "visx/bitmap.h"

/// GLBitmap is being phased out. New clients should just use Bitmap instead.
class GLBitmap : public Bitmap
{
protected:
  /// Construct an empty bitmap.
  GLBitmap();

public:
  /// Default creator.
  static GLBitmap* make();

  /// Virtual destructor.
  virtual ~GLBitmap();

  virtual fstring ioTypename() const;

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  // no writeTo() override since we just want Bitmap's writeTo()
};

static const char vcid_glbitmap_h[] = "$Header$";
#endif // !GLBITMAP_H_DEFINED
