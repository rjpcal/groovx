///////////////////////////////////////////////////////////////////////
//
// objdb.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sun Nov 21 00:26:29 1999
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

#ifndef GROOVX_NUB_OBJDB_CC_UTC20050626084019_DEFINED
#define GROOVX_NUB_OBJDB_CC_UTC20050626084019_DEFINED

#include "objdb.h"

#include "nub/object.h"
#include "nub/ref.h"

#include <typeinfo>
#include <map>

#include "rutz/trace.h"
#include "rutz/debug.h"
DBG_REGISTER

using rutz::shared_ptr;

Nub::InvalidIdError::InvalidIdError(Nub::UID id,
                                    const rutz::file_pos& pos)
  :
  rutz::error(rutz::fstring("attempted to access "
                            "invalid object '", id, "'"), pos)
{}

Nub::InvalidIdError::~InvalidIdError() throw() {}

///////////////////////////////////////////////////////////////////////
//
// Nub::ObjDb::Impl definition
//
///////////////////////////////////////////////////////////////////////

class Nub::ObjDb::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:

  typedef Nub::SoftRef<Nub::Object> ObjRef;

  typedef std::map<Nub::UID, ObjRef> MapType;
  mutable MapType itsPtrMap;

  Impl() : itsPtrMap() {}

  // Check whether the iterator points to a valid spot in the map, AND
  // that it points to a still-living object. If the object has died,
  // then we erase the iterator.
  bool isValidItr(MapType::iterator itr) const throw()
  {
    if (itr == itsPtrMap.end()) return false;

    if ((*itr).second.isInvalid())
      {
        itsPtrMap.erase(itr);
        return false;
      }

    return true;
  }

  bool isValidId(Nub::UID id) const throw()
    {
      MapType::iterator itr = itsPtrMap.find(id);
      return isValidItr(itr);
    }

  int count() const throw()
    { return itsPtrMap.size(); }

  void release(Nub::UID id)
    {
      MapType::iterator itr = itsPtrMap.find(id);

      itsPtrMap.erase(itr);
    }

  void remove(Nub::UID id)
    {
      MapType::iterator itr = itsPtrMap.find(id);
      if (!isValidItr(itr)) return;

      if ( (*itr).second.get()->isShared() )
        throw rutz::error("attempted to remove a shared object", SRC_POS);

      itsPtrMap.erase(itr);
    }

  // Return the number of items removed
  int purge()
    {
      MapType new_map;

      int num_removed = 0;

      for (MapType::iterator
             itr = itsPtrMap.begin(),
             end = itsPtrMap.end();
           itr != end;
           ++itr)
        {
          // If the object is shared, we'll be saving the object, so
          // copy it into the new_map,
          if ( isValidItr(itr) && (*itr).second.get()->isShared() )
            {
              new_map.insert(*itr);
            }
          else
            {
              ++num_removed;
            }
        }

      // Now swap maps so that the old map gets cleared and everything erased
      itsPtrMap.swap(new_map);
      return num_removed;
    }

  void clearAll()
    {
      itsPtrMap.clear();

#if 0 // an alternate implementation for verbose debugging:
      while (!itsPtrMap.empty())
        {
          MapType::iterator it = itsPtrMap.begin();

          if ((*it).second.is_valid())
            {
              dbg_eval_nl(3, typeid(*(*it).second).name());
              dbg_eval_nl(3, (*it).second->id());
            }

          itsPtrMap.erase(it);
        }
#endif
    }

  Nub::Object* getCheckedObj(Nub::UID id) throw (Nub::InvalidIdError)
    {
      MapType::iterator itr = itsPtrMap.find(id);
      if (!isValidItr(itr))
        {
          throw Nub::InvalidIdError(id, SRC_POS);
        }

      return (*itr).second.get();
    }

  void insertObj(Nub::Object* ptr, bool strong)
    {
      PRECONDITION(ptr != 0);

      // Check if the object is already in the map
      MapType::iterator existing_site = itsPtrMap.find(ptr->id());
      if (existing_site != itsPtrMap.end())
        {
          // Make sure the existing object is the same as the object
          // that we're trying to insert
          ASSERT( (*existing_site).second.get() == ptr );
        }

      const int new_id = ptr->id();

      // Must create the ObjRef with "PRIVATE" to avoid endless recursion
      itsPtrMap.insert(MapType::value_type
                       (new_id, ObjRef(ptr,
                                       strong ? Nub::STRONG : Nub::WEAK,
                                       Nub::PRIVATE)));
    }
};

///////////////////////////////////////////////////////////////////////
//
// Nub::ObjDb::Iterator definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  class ObjDbIter :
    public rutz::fwd_iter_ifx<const Nub::SoftRef<Nub::Object> >
  {
  public:
    typedef Nub::ObjDb::Impl::MapType MapType;

    void advanceToValid()
    {
      while (true)
        {
          if (itsIter == itsEnd) break;

          if ((*itsIter).second.isValid()) break;

          MapType::iterator bad = itsIter;
          ++itsIter;

          itsMap.erase(bad);
        }
    }

    ObjDbIter(MapType& m, MapType::iterator itr) :
      itsMap(m), itsIter(itr), itsEnd(m.end())
    {
      advanceToValid();
    }

    MapType& itsMap;
    MapType::iterator itsIter;
    const MapType::iterator itsEnd;

    virtual ifx_t* clone() const
    {
      return new ObjDbIter(itsMap, itsIter);
    }

    virtual void next()
    {
      if (!at_end())
        {
          ++itsIter;
          advanceToValid();
        }
    }

    virtual value_t& get()  const { return (*itsIter).second; }

    virtual bool  at_end() const { return (itsIter == itsEnd); }
  };
}

///////////////////////////////////////////////////////////////////////
//
// Nub::ObjDb member definitions
//
///////////////////////////////////////////////////////////////////////

Nub::ObjDb& Nub::ObjDb::theDb()
{
  static Nub::ObjDb* instance = 0;
  if (instance == 0)
    {
      instance = new Nub::ObjDb;
    }
  return *instance;
}

Nub::ObjDb::Iterator Nub::ObjDb::objects() const
{
DOTRACE("Nub::ObjDb::children");

 return shared_ptr<Nub::ObjDb::Iterator::ifx_t>
   (new ObjDbIter(rep->itsPtrMap, rep->itsPtrMap.begin()));
}

Nub::ObjDb::ObjDb() :
  rep(new Impl)
{
DOTRACE("Nub::ObjDb::ObjDb");
}

Nub::ObjDb::~ObjDb()
{
DOTRACE("Nub::ObjDb::~ObjDb");
  delete rep;
}

int Nub::ObjDb::count() const throw()
{
DOTRACE("Nub::ObjDb::count");

  return rep->count();
}

bool Nub::ObjDb::isValidId(Nub::UID id) const throw()
{
DOTRACE("Nub::ObjDb::isValidId");
  return rep->isValidId(id);
}

void Nub::ObjDb::remove(Nub::UID id)
{
DOTRACE("Nub::ObjDb::remove");
  rep->remove(id);
}

void Nub::ObjDb::release(Nub::UID id)
{
DOTRACE("Nub::ObjDb::release");
  rep->release(id);
}

void Nub::ObjDb::purge()
{
DOTRACE("Nub::ObjDb::clear");
  dbg_eval_nl(3, typeid(*this).name());
  rep->purge();
}

void Nub::ObjDb::clear()
{
DOTRACE("Nub::ObjDb::clear");
  // Call purge until no more items can be removed
  while ( rep->purge() != 0 )
    { ; }
}

void Nub::ObjDb::clearOnExit()
{
DOTRACE("Nub::ObjDb::clearOnExit");
  rep->clearAll();
}

Nub::Object* Nub::ObjDb::getCheckedObj(Nub::UID id) throw (Nub::InvalidIdError)
{
DOTRACE("Nub::ObjDb::getCheckedObj");
  return rep->getCheckedObj(id);
}

void Nub::ObjDb::insertObj(Nub::Object* obj)
{
DOTRACE("Nub::ObjDb::insertObj");
  rep->insertObj(obj, true);
}

void Nub::ObjDb::insertObjWeak(Nub::Object* obj)
{
DOTRACE("Nub::ObjDb::insertObjWeak");
  rep->insertObj(obj, false);
}

static const char vcid_groovx_nub_objdb_cc_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_OBJDB_CC_UTC20050626084019_DEFINED
