/** @file io/tclpkg-io.cc tcl interface packages for io::serializable
    and output_file */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Oct 30 10:00:39 2000
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

#include "tcl-io/tclpkg-io.h"

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
#include "rutz/sfmt.h"

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

        io::legacy_reader reader(ifs);

        ref<io::serializable> obj(reader.read_root(0));

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
        throw rutz::error(rutz::sfmt("error opening file: %s",
                                     filename), SRC_POS);
      }

    io::legacy_writer writer(ofs, use_bases);
    writer.use_pretty_print(false);

    for (tcl::list::iterator<ref<io::serializable> >
           itr = objids.begin<ref<io::serializable> >(),
           end = objids.end<ref<io::serializable> >();
         itr != end;
         ++itr)
      {
        writer.write_root((*itr).get());
      }
  }
}

extern "C"
int Io_Init(Tcl_Interp* interp)
{
GVX_TRACE("Io_Init");

  GVX_PKG_CREATE(pkg, interp, "io", "4.$Revision$");
  pkg->inherit_pkg("Obj");
  tcl::def_basic_type_cmds<io::serializable>(pkg, SRC_POS);

  pkg->def( "loadObjects", "filename num_to_read=-1", &loadObjects, SRC_POS );
  pkg->def( "loadObjects", "filename", rutz::bind_last(&loadObjects, ALL), SRC_POS );
  pkg->def( "saveObjects", "objids filename use_bases=yes", &saveObjects, SRC_POS );
  pkg->def( "saveObjects", "objids filename",
            rutz::bind_last(&saveObjects, true), SRC_POS );

  const unsigned int keyarg = 1;

  pkg->def_vec( "write_lgx", "objref(s)", io::write_lgx, keyarg, SRC_POS );
  pkg->def_vec( "read_lgx", "objref(s) string(s)", io::read_lgx, keyarg, SRC_POS );

  pkg->def_vec( "write_asw", "objref(s)", io::write_asw, keyarg, SRC_POS );
  pkg->def_vec( "read_asw", "objref(s) string(s)", io::read_asw, keyarg, SRC_POS );
  pkg->def( "save_asw", "objref filename", io::save_asw, SRC_POS );
  pkg->def( "load_asw", "objref filename", io::load_asw, SRC_POS );
  pkg->def( "retrieve_asw", "filename", io::retrieve_asw, SRC_POS );

  pkg->def_vec( "write_gvx", "objref(s)", io::write_gvx, keyarg, SRC_POS );
  pkg->def( "save_gvx", "objref filename", io::save_gvx, SRC_POS );
  pkg->def( "load_gvx", "filename", io::load_gvx, SRC_POS );

  pkg->def( "xml_debug", "filename", io::xml_debug, SRC_POS );

  GVX_PKG_RETURN(pkg);
}

extern "C"
int Outputfile_Init(Tcl_Interp* interp)
{
GVX_TRACE("Outputfile_Init");

  GVX_PKG_CREATE(pkg, interp, "output_file", "4.$Revision$");
  pkg->inherit_pkg("io");
  tcl::def_creator<output_file>(pkg);
  tcl::def_basic_type_cmds<io::serializable>(pkg, SRC_POS);

  pkg->def_get_set("filename",
                   &output_file::get_filename,
                   &output_file::set_filename,
                   SRC_POS);

  GVX_PKG_RETURN(pkg);
}
