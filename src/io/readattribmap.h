///////////////////////////////////////////////////////////////////////
//
// readattribmap.h
//
// Copyright (c) 2003-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 25 13:19:26 2003
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

#ifndef READATTRIBMAP_H_DEFINED
#define READATTRIBMAP_H_DEFINED

#include "io/io.h"

#include "util/strings.h"

#include <list>
#include <utility>

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
      Attrib(const fstring& t, const fstring& v) :
        type(t), value(v) {}

      fstring type;
      fstring value;
    };

  private:
    fstring itsObjTag;

    typedef std::pair<fstring, Attrib> ValueType;
    typedef std::list<ValueType> ListType;

    ListType itsMap;

    IO::VersionId itsSerialVersionId;

  public:
    inline
    AttribMap(const fstring& obj_tag);

    inline
    IO::VersionId getSerialVersionId() const;

    inline
    void setSerialVersionId(IO::VersionId id);

    inline
    Attrib get(const fstring& attrib_name);

    inline
    void addNewAttrib(const fstring& attrib_name,
                      const fstring& type, const fstring& value);
  };
}

#include "util/error.h"

inline
IO::AttribMap::AttribMap(const fstring& obj_tag) :
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
IO::AttribMap::Attrib IO::AttribMap::get(const fstring& attrib_name)
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

  throw Util::Error(fstring("no attribute named '",
                            attrib_name.c_str(), "' for ",
                            itsObjTag.c_str()));
}

inline
void IO::AttribMap::addNewAttrib(const fstring& attrib_name,
                                 const fstring& type, const fstring& value)
{
  itsMap.push_back(ValueType(attrib_name, Attrib(type,value)));
}

static const char vcid_readattribmap_h[] = "$Header$";
#endif // !READATTRIBMAP_H_DEFINED
