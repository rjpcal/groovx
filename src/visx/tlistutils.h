///////////////////////////////////////////////////////////////////////
//
// tlistutils.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 02:58:20 1999
// written: Fri Nov 10 17:03:52 2000
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

  /** This procedure initializes the Tlist from objid_file, which must
		be the name of a file containing one list of objid's per
		line. num_lines specifies how many lines to read, or, if set to
		-1, the entire file will be read. offset is an optional
		parameter which, if provided, will be added to each objid before
		the objid is inserted into the Tlist. Any Trial's that were
		previously in the Tlist are erased before the the file is
		read. Returns the number of trials that were loaded from the
		file. */
  static int loadObjidFile(const char* filename,
									int num_lines, int offset = 0);

  /** Reads a list of 'simple' trial descriptions which contain
		objid's only; posid's are inferred from position int the
		list. If num_trials is passed as < 0, the function will read to
		the end of the STD_IO::istream. If offset is non-zero, it will be added
		to each incoming objid before it is inserted into the
		Trial. Returns the number of trials that were loaded. */
  static int readFromObjidsOnly(STD_IO::istream& is,
										  int num_lines, int offset = 0);
};

static const char vcid_tlistutils_h[] = "$Header$";
#endif // !TLISTUTILS_H_DEFINED
