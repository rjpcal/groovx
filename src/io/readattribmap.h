/** @file io/readattribmap.h helper class used in io::reader subclass
    implementations */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jun 25 13:19:26 2003
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

#ifndef GROOVX_IO_READATTRIBMAP_H_UTC20050626084021_DEFINED
#define GROOVX_IO_READATTRIBMAP_H_UTC20050626084021_DEFINED

#include "io/io.h"

#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include <utility>
#include <sstream>
#include <vector>

namespace io
{
  class attrib_map
  {
  public:

    struct attrib
    {
    private:
      attrib(); // not implemented

    public:
      attrib(const rutz::fstring& t, const rutz::fstring& v) :
        type(t), value(v) {}

      rutz::fstring type;
      rutz::fstring value;
    };

  private:
    typedef std::pair<rutz::fstring, attrib>  value_type;
    typedef std::vector<value_type>           list_type;

    rutz::fstring   m_obj_tag;
    list_type       m_attribs;
    io::version_id  m_version_id;

  public:
    inline
    attrib_map(const rutz::fstring& obj_tag);

    inline
    io::version_id get_version_id() const;

    inline
    void set_version_id(io::version_id id);

    inline
    attrib get(const rutz::fstring& attr_name);

    inline
    void add_attrib(const rutz::fstring& attr_name,
                    const rutz::fstring& type,
                    const rutz::fstring& value);
  };
}

#include "rutz/error.h"

inline
io::attrib_map::attrib_map(const rutz::fstring& obj_tag) :
  m_obj_tag(obj_tag),
  m_attribs(),
  m_version_id(0)
{}

inline
io::version_id io::attrib_map::get_version_id() const
{
  return m_version_id;
}

inline
void io::attrib_map::set_version_id(io::version_id id)
{
  m_version_id = id;
}

inline
io::attrib_map::attrib io::attrib_map::get(const rutz::fstring& attr_name)
{
  list_type::iterator itr = m_attribs.begin(), end = m_attribs.end();
  while (itr != end)
    {
      if ((*itr).first == attr_name)
        {
          attrib result = (*itr).second;
          m_attribs.erase(itr);
          return result;
        }
      ++itr;
    }

  const rutz::fstring msg =
    rutz::sfmt("no attribute named '%s' for %s\n"
               "known attributes are:\n",
               attr_name.c_str(),
               m_obj_tag.c_str());

  std::ostringstream buf;

  buf << msg;

  itr = m_attribs.begin();
  while (itr != end)
    {
      buf << '\t' << (*itr).first << '\n';
      ++itr;
    }

  throw rutz::error(rutz::fstring(buf.str().c_str()), SRC_POS);
}

inline
void io::attrib_map::add_attrib(const rutz::fstring& attr_name,
                                const rutz::fstring& type,
                                const rutz::fstring& value)
{
  m_attribs.push_back(value_type(attr_name, attrib(type,value)));
}

#endif // !GROOVX_IO_READATTRIBMAP_H_UTC20050626084021_DEFINED
