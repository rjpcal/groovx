///////////////////////////////////////////////////////////////////////
//
// pbm.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 16:41:06 1999
// written: Mon Sep  9 12:16:39 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PBM_H_DEFINED
#define PBM_H_DEFINED

#ifdef HAVE_IOSFWD
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#else
class istream;
class ostream;
#endif

namespace Gfx
{
  class BmapData;
}


namespace Pbm
{
  /// Load \a data in PBM format from the file \a filename.
  void load(const char* filename, Gfx::BmapData& data);

  /// Load \a data in PBM format from the \c std::ostream \a os.
  void load(STD_IO::istream& is, Gfx::BmapData& data);

  /// Save \a data in PBM format to the file \a filename.
  void save(const char* filename, const Gfx::BmapData& data);

  /// Save \a data in PBM format to the \c std::ostream \a os.
  void save(STD_IO::ostream& os, const Gfx::BmapData& data);
};

static const char vcid_pbm_h[] = "$Header$";
#endif // !PBM_H_DEFINED
