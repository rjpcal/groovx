///////////////////////////////////////////////////////////////////////
//
// tlistutils.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Dec  4 02:58:20 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef TLISTUTILS_H_DEFINED
#define TLISTUTILS_H_DEFINED

#include "util/uid.h"

namespace Tcl
{
  class List;
}

namespace Util
{
  template <class T> class Ref;
}

/// Utility functions for working with batches of Trial objects.
namespace TlistUtils
{
  /** Write a file containing a summary of the responses to each Trial
      in the ObjDb. */
  void writeResponses(const char* filename);

  void writeIncidenceMatrix(const char* filename);

  /** Write a matlab-readable file with one line per Trial, where each
      line contains a list of the objid's in that Trial followed by
      the average response given to that Trial. */
  void writeMatlab(const char* filename);

  Util::UID createPreview(Tcl::List objid_list,
                          int num_cols_hit, bool use_text_labels);

  Tcl::List dealSingles(Tcl::List objids, Util::UID posid);

  Tcl::List dealPairs(Tcl::List objids1, Tcl::List objids2,
                      Util::UID posid1, Util::UID posid2);

  Tcl::List dealTriads(Tcl::List objids, Util::UID posid1,
                       Util::UID posid2, Util::UID posid3);

  Tcl::List loadObjidFile(const char* objid_file, Tcl::List objids,
                          Tcl::List posids, int num_lines);
}

static const char vcid_tlistutils_h[] = "$Header$";
#endif // !TLISTUTILS_H_DEFINED
