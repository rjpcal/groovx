///////////////////////////////////////////////////////////////////////
//
// factory.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Jun 26 23:40:55 1999
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

#ifndef FACTORY_H_DEFINED
#define FACTORY_H_DEFINED

#include "util/demangle.h"
#include "util/fileposition.h"
#include "util/traits.h"

#include <typeinfo>

class fstring;


/**
 *
 * \c CreatorBase is a template class that defines a single abstract
 * function, \c create(), that returns an object of type \c Base.
 *
 **/
template <class BasePtr>
class CreatorBase
{
public:
  /// Virtual destructor.
  virtual ~CreatorBase() {}

  /// Return a clone of this Creator
  virtual CreatorBase* clone() const = 0;

  /// Return a new pointer (or smart pointer) to type \c Base.
  virtual BasePtr create() = 0;
};


/**
 *
 * CreatorFromFunc implements the CreatorBase interface by storing a
 * pointer to function that returns an object of the appropriate type.
 *
 **/
template <class BasePtr, class DerivedPtr>
class CreatorFromFunc : public CreatorBase<BasePtr>
{
public:
  /// Creator function type.
  typedef DerivedPtr (*FuncType) ();

  /// Construct with a creator function.
  CreatorFromFunc(FuncType func) :
    CreatorBase<BasePtr>(), itsFunc(func) {}

  /// Clone operator.
  virtual CreatorBase<BasePtr>* clone() const
    { return new CreatorFromFunc<BasePtr, DerivedPtr>(*this); }

  /// Create an object using the creator function.
  virtual BasePtr create() { return BasePtr(itsFunc()); }

private:
  FuncType itsFunc;
};

/**
 *
 * \c AssocArray provides a non-typesafe wrapper around \c
 * std::map. The use must provide a pointer to a function that knows how to
 * properly destroy the actual contained objects according to their true
 * type.
 *
 **/

class AssocArray
{
public:
  /// Function type for destroying elements.
  typedef void (KillFunc) (void*);

  /// Default constructor.
  AssocArray(KillFunc* f);

  /// Virtual destructor.
  ~AssocArray();

  /// Raise an exception reporting an unknown type.
  void throwForType(const char* type, const FilePosition& pos);

  /// Raise an exception reporting an unknown type.
  void throwForType(const fstring& type, const FilePosition& pos);

  /// Retrieve the object associated with the tag \a name.
  void* getPtrForName(const fstring& name) const;

  /// Retrieve the object associated with the tag \a name.
  void* getPtrForName(const char* name) const;

  /// Associate the object at \a ptr with the tag \a name.
  void setPtrForName(const char* name, void* ptr);

  /// Clear all entries, calling the kill function for each.
  void clear();

private:
  AssocArray(const AssocArray&);
  AssocArray& operator=(const AssocArray&);

  struct Impl;
  Impl* const rep;
};


/**
 *
 * \c CreatorMap provides a typesafe wrapper around \c AssocArray.
 *
 **/

template<class BasePtr>
class CreatorMap
{
public:
  /// Default constructor
  CreatorMap() : base(&killPtr) {}

  /// The type of object stored in the map.
  typedef CreatorBase<BasePtr> CreatorType;

  /// Raise an exception reporting an unknown type.
  void throwForType(const char* type, const FilePosition& pos)
    { base.throwForType(type, pos); }

  /// Raise an exception reporting an unknown type.
  void throwForType(const fstring& type, const FilePosition& pos)
    { base.throwForType(type, pos); }

  /// Get the object associated with the tag \a name.
  CreatorType* getPtrForName(const fstring& name) const
    { return static_cast<CreatorType*>(base.getPtrForName(name)); }

  /// Get the object associated with the tag \a name.
  CreatorType* getPtrForName(const char* name) const
    { return static_cast<CreatorType*>(base.getPtrForName(name)); }

  /// Associate the object at \a ptr with the tag \a name.
  void setPtrForName(const char* name, CreatorType* ptr)
    { base.setPtrForName(name, static_cast<void*>(ptr)); }

private:
  AssocArray base;

  static void killPtr(void* ptr)
    { delete static_cast<CreatorType*>(ptr); }
};

/**
 *
 * \c FactoryBase exists purely to introduce a virtual destructor into
 * the \c Factory hierarchy.
 *
 **/

class FactoryBase
{
public:
  /// Virtual destructor for proper inheritance.
  virtual ~FactoryBase();
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Factory can create objects from an inheritance hierarchy given only a
 * typename. \c Factory maintains a mapping from typenames to \c
 * CreatorBase's, and so given a typename can call the \c create() function
 * of the associated \c Creator. All of the product types of a factory must
 * be derived from \c Base. The constructor is protected because factories
 * for specific types should be implemented as singleton classes derived
 * from Factory.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class BasePtr>
class Factory : public FactoryBase
{
private:
  CreatorMap<BasePtr> itsMap;

protected:
  /// Default constructor.
  Factory() : itsMap() {}

public:
  /// Registers a creation function with the factory.
  /** The default name associated with the creation function will be given
      by the type's actual C++ name. The function returns the actual name
      that was paired with the creation function.*/
  template <class DerivedPtr>
  const char* registerCreatorFunc(DerivedPtr (*func) (), const char* name = 0)
  {
    if (name == 0)
      name = rutz::demangled_name
        (typeid(typename rutz::type_traits<DerivedPtr>::pointee_t));

    itsMap.setPtrForName(name,
                         new CreatorFromFunc<BasePtr, DerivedPtr>(func));

    return name;
  }

  /** Introduces an alternate type name which can be used to create
      products of type \a Derived. There must already have been a
      creation function registered for the default name. */
  void registerAlias(const char* origName, const char* aliasName)
  {
    CreatorBase<BasePtr>* creator = itsMap.getPtrForName(origName);
    if (creator != 0)
      {
        itsMap.setPtrForName(aliasName, creator->clone());
      }
  }

  /** Returns a new object of a given type. If the given type has not
      been registered with the factory, a null pointer is returned. */
  BasePtr newObject(const fstring& type)
  {
    CreatorBase<BasePtr>* creator = itsMap.getPtrForName(type);
    if (creator == 0) return BasePtr();
    return creator->create();
  }

  /** Returns a new object of a given type. If the given type has not
      been registered with the factory, a FactorError is thrown. */
  BasePtr newCheckedObject(const fstring& type)
  {
    CreatorBase<BasePtr>* creator = itsMap.getPtrForName(type);
    if (creator == 0) itsMap.throwForType(type, SRC_POS);
    return creator->create();
  }
};

static const char vcid_factory_h[] = "$Header$";
#endif // !FACTORY_H_DEFINED
