///////////////////////////////////////////////////////////////////////
//
// writeidmap.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed May 26 10:58:49 2004
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_IO_WRITEIDMAP_H_UTC20050626084021_DEFINED
#define GROOVX_IO_WRITEIDMAP_H_UTC20050626084021_DEFINED

#include <map>

#include "rutz/debug.h"

namespace IO
{
  /// Translate UIDs into a repeatable id sequence for the XML file.
  /** This way, we can guarantee that, a given object hierarchy will always
      produce the same XML file, regardless of what the UIDs happen to
      be. */
  class WriteIdMap
  {
  public:
    WriteIdMap() :
      itsMap(),
      itsNextId(1)
    {}

    int get(Nub::UID uid) const
    {
      if (uid == 0) return 0;

      MapType::iterator itr = itsMap.find(uid);
      if (itr == itsMap.end())
        {
          int val = itsNextId++;
          itsMap.insert(MapType::value_type(uid, val));
          return val;
        }

      // else...
      GVX_ASSERT((*itr).second != 0);
      return (*itr).second;
    }

  private:
    typedef std::map<Nub::UID, int> MapType;
    mutable MapType itsMap;
    mutable int itsNextId;
  };
}

static const char vcid_groovx_io_writeidmap_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_WRITEIDMAP_H_UTC20050626084021_DEFINED
