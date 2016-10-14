/** @file visx/tlistutils.h auxiliary functions for Trial objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Dec  4 02:58:20 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_VISX_TLISTUTILS_H_UTC20050626084017_DEFINED
#define GROOVX_VISX_TLISTUTILS_H_UTC20050626084017_DEFINED

#include "nub/uid.h"

namespace geom
{
  template <class T> class rect;
}

namespace tcl
{
  class list;
}

namespace nub
{
  template <class T> class ref;
}

/// Utility functions for working with batches of Trial objects.
namespace TlistUtils
{
  /** Write a file containing a summary of the responses to each Trial
      in the nub::objectdb. */
  void writeResponses(const char* filename);

  void writeIncidenceMatrix(const char* filename);

  /** Write a matlab-readable file with one line per Trial, where each
      line contains a list of the objid's in that Trial followed by
      the average response given to that Trial. */
  void writeMatlab(const char* filename);

  nub::uid createPreview(tcl::list objid_list,
                         const geom::rect<double>& world_viewport,
                         int num_cols_hint,
                         bool use_text_labels);

  tcl::list dealSingles(tcl::list objids, nub::uid posid);

  tcl::list dealPairs(tcl::list objids1, tcl::list objids2,
                      nub::uid posid1, nub::uid posid2);

  tcl::list dealTriads(tcl::list objids, nub::uid posid1,
                       nub::uid posid2, nub::uid posid3);

  tcl::list loadObjidFile(const char* objid_file, tcl::list objids,
                          tcl::list posids, int num_lines);
}

#endif // !GROOVX_VISX_TLISTUTILS_H_UTC20050626084017_DEFINED
