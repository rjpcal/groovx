/** @file io/tclpkg-io.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Oct 30 10:00:39 2000
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_IO_TCLPKG_IO_CC_UTC20050628165656_DEFINED
#define GROOVX_IO_TCLPKG_IO_CC_UTC20050628165656_DEFINED

#include "io/tclpkg-io.h"

#include "io/io.h"
#include "io/iolegacy.h"
#include "io/ioutil.h"
#include "io/outputfile.h"
#include "io/xmlreader.h"

#include "tcl/objpkg.h"
#include "tcl/list.h"
#include "tcl/pkg.h"

#include "rutz/error.h"
#include "rutz/fstring.h"

#include <fstream>

#include "rutz/trace.h"

using nub::ref;

namespace
{
  const int ALL = -1; // indicates to read all objects until eof

  tcl::list loadObjects(const char* file, int num_to_read)
  {
    std::ifstream ifs(file);
    if (ifs.fail())
      {
        throw rutz::error("unable to open file", SRC_POS);
      }

    int num_read = 0;

    ifs >> std::ws;

    tcl::list result;

    while ( (num_to_read == ALL || num_read < num_to_read)
            && (ifs.peek() != EOF) )
      {
        // allow for whole-line comments between objects beginning
        // with '#'
        if (ifs.peek() == '#')
          {
            ifs.ignore(10000000, '\n');
            continue;
          }

        IO::LegacyReader reader(ifs);

        ref<IO::IoObject> obj(reader.readRoot(0));

        result.append(obj.id());

        ++num_read;

        ifs >> std::ws;
      }

    return result;
  }

  void saveObjects(tcl::list objids, const char* filename,
                   bool use_bases)
  {
    std::ofstream ofs(filename);
    if (ofs.fail())
      {
        throw rutz::error(rutz::fstring("error opening file: ",
                                        filename), SRC_POS);
      }

    IO::LegacyWriter writer(ofs, use_bases);
    writer.usePrettyPrint(false);

    for (tcl::list::iterator<ref<IO::IoObject> >
           itr = objids.begin<ref<IO::IoObject> >(),
           end = objids.end<ref<IO::IoObject> >();
         itr != end;
         ++itr)
      {
        writer.writeRoot((*itr).get());
      }
  }
}

extern "C"
int Io_Init(Tcl_Interp* interp)
{
GVX_TRACE("Io_Init");

  GVX_PKG_CREATE(pkg, interp, "IO", "4.$Revision$");
  pkg->inherit_pkg("Obj");
  tcl::def_basic_type_cmds<IO::IoObject>(pkg, SRC_POS);

  pkg->def( "loadObjects", "filename num_to_read=-1", &loadObjects, SRC_POS );
  pkg->def( "loadObjects", "filename", rutz::bind_last(&loadObjects, ALL), SRC_POS );
  pkg->def( "saveObjects", "objids filename use_bases=yes", &saveObjects, SRC_POS );
  pkg->def( "saveObjects", "objids filename",
            rutz::bind_last(&saveObjects, true), SRC_POS );

  const unsigned int keyarg = 1;

  pkg->def_vec( "writeLGX", "objref(s)", IO::writeLGX, keyarg, SRC_POS );
  pkg->def_vec( "readLGX", "objref(s) string(s)", IO::readLGX, keyarg, SRC_POS );

  pkg->def_vec( "writeASW", "objref(s)", IO::writeASW, keyarg, SRC_POS );
  pkg->def_vec( "readASW", "objref(s) string(s)", IO::readASW, keyarg, SRC_POS );
  pkg->def( "saveASW", "objref filename", IO::saveASW, SRC_POS );
  pkg->def( "loadASW", "objref filename", IO::loadASW, SRC_POS );
  pkg->def( "retrieveASW", "filename", IO::retrieveASW, SRC_POS );

  pkg->def_vec( "writeGVX", "objref(s)", IO::writeGVX, keyarg, SRC_POS );
  pkg->def( "saveGVX", "objref filename", IO::saveGVX, SRC_POS );
  pkg->def( "loadGVX", "filename", IO::loadGVX, SRC_POS );

  pkg->def( "xmlDebug", "filename", IO::xmlDebug, SRC_POS );

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Outputfile_Init(Tcl_Interp* interp)
{
GVX_TRACE("Outputfile_Init");

  GVX_PKG_CREATE(pkg, interp, "OutputFile", "4.$Revision$");
  pkg->inherit_pkg("IO");
  tcl::def_creator<OutputFile>(pkg);
  tcl::def_basic_type_cmds<IO::IoObject>(pkg, SRC_POS);

  pkg->def_get_set("filename",
                   &OutputFile::getFilename,
                   &OutputFile::setFilename,
                   SRC_POS);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_io_tclpkg_io_cc_utc20050628165656[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_TCLPKG_IO_CC_UTC20050628165656_DEFINED
