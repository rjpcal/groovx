///////////////////////////////////////////////////////////////////////
//
// pbm.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 16:41:06 1999
// written: Thu Aug  9 16:58:50 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PBM_H_DEFINED
#define PBM_H_DEFINED

#ifdef PRESTANDARD_IOSTREAMS
class istream;
class ostream;
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#endif

class BmapData;


namespace Pbm
{
  /// Load \a data in PBM format from the file \a filename.
  void load(const char* filename, BmapData& data);

  /// Load \a data in PBM format from the \c std::ostream \a os.
  void load(STD_IO::istream& is, BmapData& data);

  /// Save \a data in PBM format to the file \a filename.
  void save(const char* filename, const BmapData& data);

  /// Save \a data in PBM format to the \c std::ostream \a os.
  void save(STD_IO::ostream& os, const BmapData& data);
};

static const char vcid_pbm_h[] = "$Header$";
#endif // !PBM_H_DEFINED
