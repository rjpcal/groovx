///////////////////////////////////////////////////////////////////////
//
// tlistutils.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Dec  4 02:58:20 1999
// written: Sat Dec  4 03:54:39 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTUTILS_H_DEFINED
#define TLISTUTILS_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

class Tlist;

class TlistUtils {
public:
  static int createPreview(Tlist& tlist,
									const vector<int>& objids,
									int pixel_width,
									int pixel_height);


  /** Clear all trials currently in the Tlist, then make new Trial's
		so that there is one Trial for each valid object in the
		ObjList. In addition, the trialid for each generated Trial will
		be the same as the objid of the object it refers to. Returns the
		number of trials that were created. */
  static int makeSingles(Tlist& tlist, int posid);

  /** Clear all trials currently in the Tlist, then make new Trial's
		so that there is one Trial for each ordered pair of valid
		objects in the ObjList. Returns the number of trials created. */
  static int makePairs(Tlist& tlist, int posid1, int posid2);

  /** Clear all trials currently in the Tlist, then make new Trial's
		such that there is one Trial for each ordered triad of valid
		objects in the ObjList. Returns the number of trials created. */
  static int makeTriads(Tlist& tlist, int posid[]);

  /** Make a trial that contains all the objects in the ObjList on a
		grid. New Position's will be created as necessary. Return the id
		of the trial that was created. */
  static int makeSummaryTrial(Tlist& tlist,
										int trialid, int num_cols, double scale,
										double xstep, double ystep);

  /** Write a file containing a summary of the responses to each Trial
		in the Tlist. */
  static void writeResponses(Tlist& tlist, const char* filename);

  static void writeIncidenceMatrix(Tlist& tlist, const char* filename);

  /** Write a matlab-readable file with one line per Trial, where each
		line contains a list of the objid's in that Trial followed by
		the average response given to that Trial. */
  static void writeMatlab(Tlist& tlist, const char* filename);

  /** This procedure initializes the Tlist from objid_file, which must
		be the name of a file containing one list of objid's per
		line. num_lines specifies how many lines to read, or, if set to
		-1, the entire file will be read. offset is an optional
		parameter which, if provided, will be added to each objid before
		the objid is inserted into the Tlist. Any Trial's that were
		previously in the Tlist are erased before the the file is
		read. Returns the number of trials that were loaded from the
		file. */
  static int loadObjidFile(Tlist& tlist, const char* filename,
									int num_lines, int offset = 0);

  /** Reads a list of 'simple' trial descriptions which contain
		objid's only; posid's are inferred from position int the
		list. If num_trials is passed as < 0, the function will read to
		the end of the istream. If offset is non-zero, it will be added
		to each incoming objid before it is inserted into the
		Trial. Returns the number of trials that were loaded. */
  static int readFromObjidsOnly(Tlist& tlist, istream& is,
										  int num_lines, int offset = 0);

  /** This function adds an object/position pair (specified by their
		id#'s in the ObjList and PosList) to a specified trial in a
		Tlist. */
  static void addObject(Tlist& tlist, int trial, int objid, int posid);
};

static const char vcid_tlistutils_h[] = "$Header$";
#endif // !TLISTUTILS_H_DEFINED
