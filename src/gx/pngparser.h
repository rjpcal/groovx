///////////////////////////////////////////////////////////////////////
//
// pngparser.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Apr 24 20:04:52 2002
// written: Wed Mar 19 12:45:54 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PNGPARSER_H_DEFINED
#define PNGPARSER_H_DEFINED

namespace Gfx
{
  class BmapData;
}

namespace Png
{
  /// Load \a data in PNG format from the file \a filename.
  void load(const char* filename, Gfx::BmapData& data);
}

static const char vcid_pngparser_h[] = "$Header$";
#endif // !PNGPARSER_H_DEFINED
