///////////////////////////////////////////////////////////////////////
//
// imgfile.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Apr 25 09:06:46 2002
// written: Thu Apr 25 09:29:51 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IMGFILE_H_DEFINED
#define IMGFILE_H_DEFINED

namespace Gfx
{
  class BmapData;
}

namespace ImgFile
{
  /// Load \a data from \a filename, which must be in a supported format.
  void load(const char* filename, Gfx::BmapData& data);

  /// Save \a data to \a filename (file format is inferred from the filename).
  void save(const char* filename, const Gfx::BmapData& data);
}

static const char vcid_imgfile_h[] = "$Header$";
#endif // !IMGFILE_H_DEFINED
