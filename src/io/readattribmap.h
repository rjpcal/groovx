///////////////////////////////////////////////////////////////////////
//
// readattribmap.h
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jun 25 13:19:26 2003
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

#ifndef GROOVX_IO_READATTRIBMAP_H_UTC20050626084021_DEFINED
#define GROOVX_IO_READATTRIBMAP_H_UTC20050626084021_DEFINED

#include "io/io.h"

#include "rutz/fstring.h"

#include <utility>
#include <vector>

namespace IO
{
  class AttribMap
  {
  public:

    struct Attrib
    {
    private:
      Attrib(); // not implemented

    public:
      Attrib(const rutz::fstring& t, const rutz::fstring& v) :
        type(t), value(v) {}

      rutz::fstring type;
      rutz::fstring value;
    };

  private:
    rutz::fstring itsObjTag;

    typedef std::pair<rutz::fstring, Attrib> ValueType;
    typedef std::vector<ValueType> ListType;

    ListType itsMap;

    IO::VersionId itsSerialVersionId;

  public:
    inline
    AttribMap(const rutz::fstring& obj_tag);

    inline
    IO::VersionId getSerialVersionId() const;

    inline
    void setSerialVersionId(IO::VersionId id);

    inline
    Attrib get(const rutz::fstring& attrib_name);

    inline
    void addNewAttrib(const rutz::fstring& attrib_name,
                      const rutz::fstring& type,
                      const rutz::fstring& value);
  };
}

#include "rutz/error.h"

inline
IO::AttribMap::AttribMap(const rutz::fstring& obj_tag) :
  itsObjTag(obj_tag),
  itsMap(),
  itsSerialVersionId(0)
{}

inline
IO::VersionId IO::AttribMap::getSerialVersionId() const
{
  return itsSerialVersionId;
}

inline
void IO::AttribMap::setSerialVersionId(IO::VersionId id)
{
  itsSerialVersionId = id;
}

inline
IO::AttribMap::Attrib IO::AttribMap::get(const rutz::fstring& attrib_name)
{
  ListType::iterator itr = itsMap.begin(), end = itsMap.end();
  while (itr != end)
    {
      if ((*itr).first == attrib_name)
        {
          Attrib result = (*itr).second;
          itsMap.erase(itr);
          return result;
        }
      ++itr;
    }

  rutz::fstring msg("no attribute named '",
                    attrib_name.c_str(), "' for ",
                    itsObjTag.c_str(), "\nknown attributes are:\n");

  itr = itsMap.begin();
  while (itr != end)
    {
      msg.append("\t", (*itr).first, "\n");
      ++itr;
    }

  throw rutz::error(msg, SRC_POS);
}

inline
void IO::AttribMap::addNewAttrib(const rutz::fstring& attrib_name,
                                 const rutz::fstring& type,
                                 const rutz::fstring& value)
{
  itsMap.push_back(ValueType(attrib_name, Attrib(type,value)));
}

static const char vcid_groovx_io_readattribmap_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_READATTRIBMAP_H_UTC20050626084021_DEFINED
