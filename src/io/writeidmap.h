/** @file io/writeidmap.h helper class used in io::writer subclass
    implementations */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed May 26 10:58:49 2004
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

#ifndef GROOVX_IO_WRITEIDMAP_H_UTC20050626084021_DEFINED
#define GROOVX_IO_WRITEIDMAP_H_UTC20050626084021_DEFINED

#include <map>

#include "rutz/debug.h"

namespace io
{
  /// Translate nub::uid values into a repeatable id sequence for the XML file.
  /** This way, we can guarantee that a given object hierarchy will
      always produce the same XML file, regardless of what the
      nub::uid values happen to be. */
  class write_id_map
  {
  public:
    write_id_map() :
      m_map(),
      m_next_id(1)
    {}

    int get(nub::uid uid) const
    {
      if (uid == 0) return 0;

      map_type::iterator itr = m_map.find(uid);
      if (itr == m_map.end())
        {
          int val = m_next_id++;
          m_map.insert(map_type::value_type(uid, val));
          return val;
        }

      // else...
      GVX_ASSERT((*itr).second != 0);
      return (*itr).second;
    }

  private:
    typedef std::map<nub::uid, int> map_type;

    mutable map_type   m_map;
    mutable int        m_next_id;
  };
}

static const char vcid_groovx_io_writeidmap_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_WRITEIDMAP_H_UTC20050626084021_DEFINED
