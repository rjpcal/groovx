/** @file visx/tclpkg-tlist.cc tcl interface package for special
    Trial-related functions */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Mar 13 12:38:37 1999
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

#include "visx/tclpkg-tlist.h"

#include "tcl/list.h"
#include "tcl/pkg.h"

#include "tcl-gfx/recttcl.h"

#include "visx/tlistutils.h"

#include "rutz/trace.h"

extern "C"
int Tlist_Init(Tcl_Interp* interp)
{
GVX_TRACE("Tlist_Init");

  return tcl::pkg::init
    (interp, "Tlist", "4.$Revision$",
     [](tcl::pkg* pkg) {
      pkg->def( "createPreview", "objids viewport num_cols_hint use_text_labels",
                &TlistUtils::createPreview,
                SRC_POS );
      pkg->def( "createPreview", "objids viewport",
                rutz::bind_last(rutz::bind_last(&TlistUtils::createPreview,
                                                true), -1),
                SRC_POS );

      pkg->def( "dealSingles", "objid(s) posid",
                &TlistUtils::dealSingles,
                SRC_POS );
      pkg->def( "dealPairs", "objids1 objids2 posid1 posid2",
                &TlistUtils::dealPairs,
                SRC_POS );
      pkg->def( "dealTriads", "objids posid1 posid2 posid3",
                &TlistUtils::dealTriads,
                SRC_POS );

      pkg->def( "loadObjidFile", "objid_file objids posids num_lines=-1",
                &TlistUtils::loadObjidFile,
                SRC_POS );
      pkg->def( "loadObjidFile", "objid_file objids posids",
                rutz::bind_last(&TlistUtils::loadObjidFile, -1),
                SRC_POS );

      pkg->def( "write_responses", "filename", &TlistUtils::writeResponses, SRC_POS );
      pkg->def( "writeIncidenceMatrix", "filename", &TlistUtils::writeIncidenceMatrix, SRC_POS );
      pkg->def( "writeMatlab", "filename", &TlistUtils::writeMatlab, SRC_POS );
    });
}
