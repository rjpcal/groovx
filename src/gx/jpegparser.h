///////////////////////////////////////////////////////////////////////
//
// jpegparser.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Nov 11 15:15:46 2002
// written: Wed Mar 19 12:45:54 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef JPEGPARSER_H_DEFINED
#define JPEGPARSER_H_DEFINED

namespace Gfx
{
  class BmapData;
}

namespace Jpeg
{
  /// Load \a data in JPEG format from the file \a filename.
  void load(const char* filename, Gfx::BmapData& data);
}

static const char vcid_jpegparser_h[] = "$Header$";
#endif // !JPEGPARSER_H_DEFINED
