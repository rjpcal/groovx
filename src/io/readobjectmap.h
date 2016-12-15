/** @file io/readobjectmap.h helper class used in io::reader subclass
    implementations */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Jun 25 13:13:18 2003
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_IO_READOBJECTMAP_H_UTC20050626084021_DEFINED
#define GROOVX_IO_READOBJECTMAP_H_UTC20050626084021_DEFINED

#include "nub/uid.h"

#include "rutz/sfmt.h"

#include <map>

namespace rutz
{
  class fstring;
}

namespace nub
{
  template <class T> class ref;
}

namespace io
{
  class serializable;

  class object_map
  {
  private:
    typedef std::map<nub::uid, nub::ref<io::serializable> > map_type;
    map_type m_objects;

  public:
    inline
    object_map();

    // This returns the object for the given id; the object must
    // already have been created, otherwise an exception will be
    // thrown.
    inline
    nub::ref<io::serializable> get_existing_object(nub::uid id);

    // This will create an object for the id if one has not yet been
    // created, then return the object for that id.
    inline
    nub::ref<io::serializable> fetch_object(const rutz::fstring& type, nub::uid id);

    inline
    void add_object_for_id(nub::uid id, nub::ref<io::serializable> object);

    inline
    void clear();
  };
}

#include "io/io.h"

#include "nub/objmgr.h"
#include "nub/ref.h"

#include "rutz/error.h"
#include "rutz/fstring.h"

inline
io::object_map::object_map() : m_objects() {}

inline
nub::ref<io::serializable>
io::object_map::get_existing_object(nub::uid id)
{
  map_type::const_iterator itr = m_objects.find(id);
  if ( itr == m_objects.end() )
    {
      throw rutz::error(rutz::sfmt("no object was found "
                                   "for the given id: %lu", id),
                        SRC_POS);
    }

  return (*itr).second;
}

inline
nub::ref<io::serializable>
io::object_map::fetch_object(const rutz::fstring& type, nub::uid id)
{
  map_type::const_iterator itr = m_objects.find(id);

  if ( itr == m_objects.end() )
    {
      nub::ref<io::serializable> obj
        (nub::obj_mgr::new_typed_obj<io::serializable>(type));

      m_objects.insert(map_type::value_type(id, obj));

      return obj;
    }

  return (*itr).second;
}

inline
void io::object_map::add_object_for_id(nub::uid id,
                                       nub::ref<io::serializable> object)
{
  map_type::const_iterator itr = m_objects.find(id);

  // See if an object has already been created for this id
  if ( itr != m_objects.end() )
    {
      const rutz::fstring msg =
        rutz::sfmt("object has already been created\n"
                   "\ttype: %s\n"
                   "\tid: %lu",
                   object->obj_typename().c_str(), id);
      throw rutz::error(msg, SRC_POS);
    }

  m_objects.insert(map_type::value_type(id, object));
}

inline
void io::object_map::clear()
{
  m_objects.clear();
}

#endif // !GROOVX_IO_READOBJECTMAP_H_UTC20050626084021_DEFINED
