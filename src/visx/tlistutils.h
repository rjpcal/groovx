///////////////////////////////////////////////////////////////////////
//
// tlistutils.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  4 02:58:20 1999
// written: Sun Aug 26 08:35:12 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTUTILS_H_DEFINED
#define TLISTUTILS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(UID_H_DEFINED)
#include "util/uid.h"
#endif

namespace Gfx
{
  class Canvas;
}

template <class T> class dynamic_block;

class TlistUtils
{
public:
  static Util::UID createPreview(Gfx::Canvas& canvas,
                                 Util::UID* objids, unsigned int objids_size);

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
