///////////////////////////////////////////////////////////////////////
//
// objdb.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Nov 21 00:26:29 1999
// written: Mon Jan 13 11:08:25 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJDB_CC_DEFINED
#define OBJDB_CC_DEFINED

#include "util/objdb.h"

#include "util/object.h"
#include "util/ref.h"

#include <typeinfo>
#include <map>

#include "util/trace.h"
#include "util/debug.h"

InvalidIdError::~InvalidIdError() {}

///////////////////////////////////////////////////////////////////////
//
// ObjDb::Impl definition
//
///////////////////////////////////////////////////////////////////////

class ObjDb::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:

  typedef Util::SoftRef<Util::Object> ObjRef;

  typedef std::map<Util::UID, ObjRef> MapType;
  mutable MapType itsPtrMap;

  Impl() : itsPtrMap() {}

  // Check whether the iterator points to a valid spot in the map, AND
  // that it points to a still-living object. If the object has died,
  // then we erase the iterator.
  bool isValidItr(MapType::iterator itr) const
  {
    if (itr == itsPtrMap.end()) return false;

    if ((*itr).second.isInvalid())
      {
        itsPtrMap.erase(itr);
        return false;
      }

    return true;
  }

  int count() const
    { return itsPtrMap.size(); }

  bool isValidId(Util::UID id) const
    {
      MapType::iterator itr = itsPtrMap.find(id);
      return isValidItr(itr);
    }

  void release(Util::UID id)
    {
      MapType::iterator itr = itsPtrMap.find(id);

      itsPtrMap.erase(itr);
    }

  void remove(Util::UID id)
    {
      MapType::iterator itr = itsPtrMap.find(id);
      if (!isValidItr(itr)) return;

      if ( (*itr).second.get()->isShared() )
        throw Util::Error("attempted to remove a shared object");

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

          if ((*it).second.isValid())
            {
              dbgEvalNL(3, typeid(*(*it).second).name());
              dbgEvalNL(3, (*it).second->id());
            }

          itsPtrMap.erase(it);
        }
#endif
    }

  Util::Object* getCheckedObj(Util::UID id) throw (InvalidIdError)
    {
      MapType::iterator itr = itsPtrMap.find(id);
      if (!isValidItr(itr))
        {
          throw InvalidIdError(fstring("attempted to access "
                                       "invalid object '", id, "'"));
        }

      return (*itr).second.get();
    }

  void insertObj(Util::Object* ptr, bool strong)
    {
      Precondition(ptr != 0);

      // Check if the object is already in the map
      MapType::iterator existing_site = itsPtrMap.find(ptr->id());
      if (existing_site != itsPtrMap.end())
        {
          // Make sure the existing object is the same as the object
          // that we're trying to insert
          Assert( (*existing_site).second.get() == ptr );
        }

      const int new_id = ptr->id();

      // Must create the ObjRef with "PRIVATE" to avoid endless recursion
      itsPtrMap.insert(MapType::value_type
                       (new_id, ObjRef(ptr,
                                       strong ? Util::STRONG : Util::WEAK,
                                       Util::PRIVATE)));
    }
};

///////////////////////////////////////////////////////////////////////
//
// ObjDb::Iterator definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  class ObjDbIter :
    public Util::FwdIterIfx<const Util::SoftRef<Util::Object> >
  {
  public:
    typedef ObjDb::Impl::MapType MapType;

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

    virtual Interface* clone() const
    {
      return new ObjDbIter(itsMap, itsIter);
    }

    virtual void next()
    {
      if (!atEnd())
        {
          ++itsIter;
          advanceToValid();
        }
    }

    virtual ValueType& get() const { return (*itsIter).second; }

    virtual bool     atEnd() const { return (itsIter == itsEnd); }
  };
}

///////////////////////////////////////////////////////////////////////
//
// ObjDb member definitions
//
///////////////////////////////////////////////////////////////////////

ObjDb& ObjDb::theDb()
{
  static ObjDb* instance = 0;
  if (instance == 0)
    {
      instance = new ObjDb;
    }
  return *instance;
}

ObjDb::Iterator ObjDb::objects() const
{
DOTRACE("ObjDb::children");

 return shared_ptr<ObjDb::Iterator::Interface>
   (new ObjDbIter(rep->itsPtrMap, rep->itsPtrMap.begin()));
}

ObjDb::ObjDb() :
  rep(new Impl)
{
DOTRACE("ObjDb::ObjDb");
}

ObjDb::~ObjDb()
{
DOTRACE("ObjDb::~ObjDb");
  delete rep;
}

int ObjDb::count() const
{
DOTRACE("ObjDb::count");

  return rep->count();
}

bool ObjDb::isValidId(Util::UID id) const
{
DOTRACE("ObjDb::isValidId");
  return rep->isValidId(id);
}

void ObjDb::remove(Util::UID id)
{
DOTRACE("ObjDb::remove");
  rep->remove(id);
}

void ObjDb::release(Util::UID id)
{
DOTRACE("ObjDb::release");
  rep->release(id);
}

void ObjDb::purge()
{
DOTRACE("ObjDb::clear");
  dbgEvalNL(3, typeid(*this).name());
  rep->purge();
}

void ObjDb::clear()
{
DOTRACE("ObjDb::clear");
  // Call purge until no more items can be removed
  while ( rep->purge() != 0 )
    { ; }
}

void ObjDb::clearOnExit()
{
DOTRACE("ObjDb::clearOnExit");
  rep->clearAll();
}

Util::Object* ObjDb::getCheckedObj(Util::UID id) throw (InvalidIdError)
{
DOTRACE("ObjDb::getCheckedObj");
  return rep->getCheckedObj(id);
}

void ObjDb::insertObj(Util::Object* obj)
{
DOTRACE("ObjDb::insertObj");
  rep->insertObj(obj, true);
}

void ObjDb::insertObjWeak(Util::Object* obj)
{
DOTRACE("ObjDb::insertObjWeak");
  rep->insertObj(obj, false);
}

static const char vcid_objdb_cc[] = "$Header$";
#endif // !OBJDB_CC_DEFINED
