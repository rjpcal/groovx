///////////////////////////////////////////////////////////////////////
//
// iotcl.cc
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Oct 30 10:00:39 2000
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

#ifndef IOTCL_CC_DEFINED
#define IOTCL_CC_DEFINED

#include "io/io.h"
#include "io/ioutil.h"
#include "io/outputfile.h"
#include "io/xmlreader.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/strings.h"

#include "util/trace.h"

extern "C"
int Io_Init(Tcl_Interp* interp)
{
DOTRACE("Io_Init");

  PKG_CREATE(interp, "IO", "$Revision$");
  pkg->inheritPkg("Obj");
  Tcl::defGenericObjCmds<IO::IoObject>(pkg, SRC_POS);

  const unsigned int keyarg = 1;

  pkg->defVec( "writeLGX", "objref(s)", IO::writeLGX, keyarg, SRC_POS );
  pkg->defVec( "readLGX", "objref(s) string(s)", IO::readLGX, keyarg, SRC_POS );

  pkg->defVec( "writeASW", "objref(s)", IO::writeASW, keyarg, SRC_POS );
  pkg->defVec( "readASW", "objref(s) string(s)", IO::readASW, keyarg, SRC_POS );
  pkg->def( "saveASW", "objref filename", IO::saveASW, SRC_POS );
  pkg->def( "loadASW", "objref filename", IO::loadASW, SRC_POS );
  pkg->def( "retrieveASW", "filename", IO::retrieveASW, SRC_POS );

  pkg->defVec( "writeGVX", "objref(s)", IO::writeGVX, keyarg, SRC_POS );
  pkg->def( "saveGVX", "objref filename", IO::saveGVX, SRC_POS );
  pkg->def( "loadGVX", "filename", IO::loadGVX, SRC_POS );

  pkg->def( "xmlDebug", "filename", IO::xmlDebug, SRC_POS );

  PKG_RETURN;
}

extern "C"
int Outputfile_Init(Tcl_Interp* interp)
{
DOTRACE("Outputfile_Init");

  PKG_CREATE(interp, "OutputFile", "$Revision$");
  pkg->inheritPkg("IO");
  Tcl::defCreator<OutputFile>(pkg);
  Tcl::defGenericObjCmds<IO::IoObject>(pkg, SRC_POS);

  pkg->defAttrib("filename",
                 &OutputFile::getFilename,
                 &OutputFile::setFilename,
                 SRC_POS);

  PKG_RETURN;
}

static const char vcid_iotcl_cc[] = "$Header$";
#endif // !IOTCL_CC_DEFINED
