///////////////////////////////////////////////////////////////////////
//
// glbitmap.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 11:02:30 1999
// written: Wed Nov 20 17:00:03 2002
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

public:
  /// Virtual destructor.
  virtual ~GLBitmap();

  virtual IO::VersionId serialVersionId() const;
  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;
};

static const char vcid_glbitmap_h[] = "$Header$";
#endif // !GLBITMAP_H_DEFINED
