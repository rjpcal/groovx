///////////////////////////////////////////////////////////////////////
//
// readobjectmap.h
//
// Copyright (c) 2003-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jun 25 13:13:18 2003
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

#ifndef READOBJECTMAP_H_DEFINED
#define READOBJECTMAP_H_DEFINED

#include "util/uid.h"

#include <map>

class fstring;

namespace Util
{
  template <class T> class Ref;
}

namespace IO
{
  class IoObject;

  class ObjectMap
  {
  private:
    typedef std::map<Util::UID, Util::Ref<IO::IoObject> > MapType;
    MapType itsMap;

  public:
    inline
    ObjectMap();

    // This returns the object for the given id; the object must
    // already have been created, otherwise an exception will be thrown.
    inline
    Util::Ref<IO::IoObject> getObject(Util::UID id);

    // This will create an object for the id if one has not yet been
    // created, then return the object for that id.
    inline
    Util::Ref<IO::IoObject> fetchObject(const fstring& type, Util::UID id);

    inline
    void assignObjectForId(Util::UID id, Util::Ref<IO::IoObject> object);

    inline
    void clear();
  };
}

#include "io/io.h"

#include "util/error.h"
#include "util/objmgr.h"
#include "util/ref.h"
#include "util/strings.h"

inline
IO::ObjectMap::ObjectMap() : itsMap() {}

inline
Util::Ref<IO::IoObject>
IO::ObjectMap::getObject(Util::UID id)
{
  MapType::const_iterator itr = itsMap.find(id);
  if ( itr == itsMap.end() )
    {
      throw Util::Error(fstring("no object was found "
                                "for the given id:", id));
    }

  return (*itr).second;
}

inline
Util::Ref<IO::IoObject>
IO::ObjectMap::fetchObject(const fstring& type, Util::UID id)
{
  MapType::const_iterator itr = itsMap.find(id);

  if ( itr == itsMap.end() )
    {
      Util::Ref<IO::IoObject> obj
        (Util::ObjMgr::newTypedObj<IO::IoObject>(type));

      itsMap.insert(MapType::value_type(id, obj));

      return obj;
    }

  return (*itr).second;
}

inline
void IO::ObjectMap::assignObjectForId(Util::UID id,
                                      Util::Ref<IO::IoObject> object)
{
  MapType::const_iterator itr = itsMap.find(id);

  // See if an object has already been created for this id
  if ( itr != itsMap.end() )
    {
      fstring msg;
      msg.append("object has already been created\n");
      msg.append("\ttype: ", object->objTypename().c_str(), "\n");
      msg.append("\tid: ", id);
      throw Util::Error(msg);
    }

  itsMap.insert(MapType::value_type(id, object));
}

inline
void IO::ObjectMap::clear()
{
  itsMap.clear();
}

static const char vcid_readobjectmap_h[] = "$Header$";
#endif // !READOBJECTMAP_H_DEFINED
