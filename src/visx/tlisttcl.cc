///////////////////////////////////////////////////////////////////////
//
// tlisttcl.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Mar 13 12:38:37 1999
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

#ifndef TLISTTCL_CC_DEFINED
#define TLISTTCL_CC_DEFINED

#include "visx/tlisttcl.h"

#include "gfx/recttcl.h"

#include "tcl/tcllistobj.h"
#include "tcl/tclpkg.h"

#include "visx/tlistutils.h"

#include "util/trace.h"

extern "C"
int Tlist_Init(Tcl_Interp* interp)
{
DOTRACE("Tlist_Init");

  PKG_CREATE(interp, "Tlist", "4.$Revision$");
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

  PKG_RETURN;
}

static const char vcid_tlisttcl_cc[] = "$Id$ $URL$";
#endif // !TLISTTCL_CC_DEFINED
