///////////////////////////////////////////////////////////////////////
//
// tlistutils.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 02:58:20 1999
// written: Wed May 23 10:33:23 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTUTILS_H_DEFINED
#define TLISTUTILS_H_DEFINED

template <class T> class dynamic_block;

#ifdef PRESTANDARD_IOSTREAMS
class istream;
class ostream;
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#endif

namespace GWT {
  class Canvas;
}

class TlistUtils {
public:
  static int createPreview(const GWT::Canvas& canvas,
									int* objids, unsigned int objids_size,
									int pixel_width,
									int pixel_height);

  /** Write a file containing a summary of the responses to each Trial
		in the Tlist. */
  static void writeResponses(const char* filename);

  static void writeIncidenceMatrix(const char* filename);

  /** Write a matlab-readable file with one line per Trial, where each
		line contains a list of the objid's in that Trial followed by
		the average response given to that Trial. */
  static void writeMatlab(const char* filename);
};

static const char vcid_tlistutils_h[] = "$Header$";
#endif // !TLISTUTILS_H_DEFINED
