///////////////////////////////////////////////////////////////////////
//
// iotcl.cc
//
// Copyright (c) 2000-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Oct 30 10:00:39 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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
  Tcl::defGenericObjCmds<IO::IoObject>(pkg);

  pkg->defVec( "writeLGX", "item_id(s)", IO::writeLGX );
  pkg->defVec( "readLGX", "item_id(s) string(s)", IO::readLGX );

  pkg->defVec( "writeASW", "item_id(s)", IO::writeASW );
  pkg->defVec( "readASW", "item_id(s) string(s)", IO::readASW );
  pkg->def( "saveASW", "item_id filename", IO::saveASW );
  pkg->def( "loadASW", "item_id filename", IO::loadASW );
  pkg->def( "retrieveASW", "filename", IO::retrieveASW );

  pkg->defVec( "writeGVX", "item_id(s)", IO::writeGVX );
  pkg->def( "saveGVX", "item_id filename", IO::saveGVX );
  pkg->def( "loadGVX", "filename", IO::loadGVX );

  pkg->def( "xmlDebug", "filename", IO::xmlDebug );

  PKG_RETURN;
}

extern "C"
int Outputfile_Init(Tcl_Interp* interp)
{
DOTRACE("Outputfile_Init");

  PKG_CREATE(interp, "OutputFile", "$Revision$");
  pkg->inheritPkg("IO");
  Tcl::defCreator<OutputFile>(pkg);
  Tcl::defGenericObjCmds<IO::IoObject>(pkg);

  pkg->defAttrib("filename",
                 &OutputFile::getFilename,
                 &OutputFile::setFilename);

  PKG_RETURN;
}

static const char vcid_iotcl_cc[] = "$Header$";
#endif // !IOTCL_CC_DEFINED
