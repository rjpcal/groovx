///////////////////////////////////////////////////////////////////////
//
// objdb.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Nov 21 00:26:29 1999
// written: Mon Aug 20 08:26:08 2001
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

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

InvalidIdError::~InvalidIdError() {}

///////////////////////////////////////////////////////////////////////
//
// ObjDb::Impl definition
//
///////////////////////////////////////////////////////////////////////

class ObjDb::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:

  typedef Util::WeakRef<Util::Object> ObjRef;

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
    { itsPtrMap.clear(); }

  Util::Object* getCheckedPtrBase(Util::UID id) throw (InvalidIdError)
    {
      MapType::iterator itr = itsPtrMap.find(id);
      if (!isValidItr(itr))
        {
          throw InvalidIdError(fstring("attempted to access "
                                       "invalid object '", id, "'"));
        }

      return (*itr).second.get();
    }

  void insertPtrBase(Util::Object* ptr)
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

      // Must create the ObjRef with "noInsert" to avoid endless recursion
      itsPtrMap.insert(MapType::value_type(new_id, ObjRef(ptr, true)));
    }
};

///////////////////////////////////////////////////////////////////////
//
// ObjDb::ItrImpl definition
//
///////////////////////////////////////////////////////////////////////

class ObjDb::ItrImpl {
public:
  typedef ObjDb::Impl::MapType MapType;

  ItrImpl(ObjDb::Impl::MapType& m, MapType::iterator itr) :
    itsMap(m), itsIter(itr) {}

  MapType& itsMap;
  MapType::iterator itsIter;
};

///////////////////////////////////////////////////////////////////////
//
// ObjDb::Iterator member definitions
//
///////////////////////////////////////////////////////////////////////

ObjDb::Iterator::Iterator(ObjDb::ItrImpl* impl) :
  itsImpl(impl)
{
DOTRACE("ObjDb::Iterator::Iterator()");
}

ObjDb::Iterator::~Iterator()
{
DOTRACE("ObjDb::Iterator::~Iterator");
  delete itsImpl;
}

ObjDb::Iterator::Iterator(const ObjDb::Iterator& other) :
  itsImpl(new ItrImpl(*(other.itsImpl)))
{
DOTRACE("ObjDb::Iterator::Iterator(copy)");
}

ObjDb::Iterator&
ObjDb::Iterator::operator=(const ObjDb::Iterator& other)
{
DOTRACE("ObjDb::Iterator::operator=");
  ItrImpl* old_impl = itsImpl;
  itsImpl = new ItrImpl(*(other.itsImpl));
  delete old_impl;
  return *this;
}

bool ObjDb::Iterator::operator==(const ObjDb::Iterator& other) const
{
  return itsImpl->itsIter == other.itsImpl->itsIter;
}

ObjDb::Iterator&
ObjDb::Iterator::operator++()
{
  ++(itsImpl->itsIter);

  while (true)
    {
      if (itsImpl->itsIter == itsImpl->itsMap.end()) break;

      if ((*(itsImpl->itsIter)).second.isValid()) break;

      ItrImpl::MapType::iterator bad = itsImpl->itsIter;
      ++(itsImpl->itsIter);

      itsImpl->itsMap.erase(bad);
    }

  return *this;
}

const Util::WeakRef<Util::Object>& ObjDb::Iterator::operator*() const
{
  return (*(itsImpl->itsIter)).second;
}

///////////////////////////////////////////////////////////////////////
//
// ObjDb member definitions
//
///////////////////////////////////////////////////////////////////////

ObjDb ObjDb::theInstance;

ObjDb& ObjDb::theDb()
{
  return theInstance;
}

ObjDb::Iterator ObjDb::begin() const
{
DOTRACE("ObjDb::begin");
  return Iterator(new ItrImpl(itsImpl->itsPtrMap, itsImpl->itsPtrMap.begin()));
}

ObjDb::Iterator ObjDb::end() const
{
DOTRACE("ObjDb::end");
  return Iterator(new ItrImpl(itsImpl->itsPtrMap, itsImpl->itsPtrMap.end()));
}

ObjDb::ObjDb() :
  itsImpl(new Impl)
{
DOTRACE("ObjDb::ObjDb");
}

ObjDb::~ObjDb()
{
DOTRACE("ObjDb::~ObjDb");
  delete itsImpl;
}

int ObjDb::count() const
{
DOTRACE("ObjDb::count");

  return itsImpl->count();
}

bool ObjDb::isValidId(Util::UID id) const
{
DOTRACE("ObjDb::isValidId");
  return itsImpl->isValidId(id);
}

void ObjDb::remove(Util::UID id)
{
DOTRACE("ObjDb::remove");
  itsImpl->remove(id);
}

void ObjDb::release(Util::UID id)
{
DOTRACE("ObjDb::release");
  itsImpl->release(id);
}

void ObjDb::purge()
{
DOTRACE("ObjDb::clear");
  DebugEvalNL(typeid(*this).name());
  itsImpl->purge();
}

void ObjDb::clear()
{
DOTRACE("ObjDb::clear");
  // Call purge until no more items can be removed
  while ( itsImpl->purge() != 0 )
    { ; }
}

void ObjDb::clearOnExit()
{
DOTRACE("ObjDb::clearOnExit");
  itsImpl->clearAll();
}

Util::Object* ObjDb::getCheckedPtrBase(Util::UID id) throw (InvalidIdError)
{
DOTRACE("ObjDb::getCheckedPtrBase");
  return itsImpl->getCheckedPtrBase(id);
}

void ObjDb::insertPtrBase(Util::Object* ptr)
{
DOTRACE("ObjDb::insertPtrBase");
  itsImpl->insertPtrBase(ptr);
}

static const char vcid_objdb_cc[] = "$Header$";
#endif // !OBJDB_CC_DEFINED
