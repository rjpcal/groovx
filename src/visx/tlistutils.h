///////////////////////////////////////////////////////////////////////
//
// tlistutils.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Dec  4 02:58:20 1999
// written: Wed Mar 19 12:46:27 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTUTILS_H_DEFINED
#define TLISTUTILS_H_DEFINED

/// Utility functions for working with batches of Trial objects.
class TlistUtils
{
public:
  /** Write a file containing a summary of the responses to each Trial
      in the ObjDb. */
  static void writeResponses(const char* filename);

  static void writeIncidenceMatrix(const char* filename);

  /** Write a matlab-readable file with one line per Trial, where each
      line contains a list of the objid's in that Trial followed by
      the average response given to that Trial. */
  static void writeMatlab(const char* filename);
};

static const char vcid_tlistutils_h[] = "$Header$";
#endif // !TLISTUTILS_H_DEFINED
