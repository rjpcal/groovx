///////////////////////////////////////////////////////////////////////
//
// factory.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Jun 26 23:40:55 1999
// written: Mon Jan 13 11:08:26 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACTORY_H_DEFINED
#define FACTORY_H_DEFINED

#include "util/traits.h"

#include "system/demangle.h"

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
  typedef DerivedPtr (*FuncType) ();

  CreatorFromFunc(FuncType func) :
    CreatorBase<BasePtr>(), itsFunc(func) {}

  CreatorFromFunc(const CreatorFromFunc& other) :
    CreatorBase<BasePtr>(), itsFunc(other.itsFunc) {}

  CreatorFromFunc& operator=(const CreatorFromFunc& other)
    { itsFunc = other.itsFunc; return *this; }

  virtual CreatorBase<BasePtr>* clone() const
    { return new CreatorFromFunc<BasePtr, DerivedPtr>(*this); }

  virtual BasePtr create() { return BasePtr(itsFunc()); }

private:
  FuncType itsFunc;
};

/**
 *
 * \c CreatorMapBase provides a non-typesafe wrapper around \c
 * std::map. The \c killPtr() function should be overridden to delete
 * the pointer after casting it to its true type.
 *
 **/

class CreatorMapBase
{
public:
  /// Default constructor.
  CreatorMapBase();

  /// Virtual destructor.
  virtual ~CreatorMapBase();

  /// Raise an exception reporting an unknown type.
  void throwForType(const char* type);

  /// Raise an exception reporting an unknown type.
  void throwForType(const fstring& type);

protected:
  /// Retrieve the object associated with the tag \a name.
  void* getPtrForName(const fstring& name) const;

  /// Associate the object at \a ptr with the tag \a name.
  void setPtrForName(const char* name, void* ptr);

  /// Delete the object at \a ptr.
  virtual void killPtr(void* ptr) = 0;

  /** This must be called by derived class's destructors in order to
      avoid a memory leak. */
  void clear();

private:
  CreatorMapBase(const CreatorMapBase&);
  CreatorMapBase& operator=(const CreatorMapBase&);

  struct Impl;
  Impl* const itsImpl;
};


/**
 *
 * \c CreatorMap provides a typesafe wrapper around \c CreatorMapBase.
 *
 **/

template<class BasePtr>
class CreatorMap : public CreatorMapBase
{
public:
  /// Virtual destructor calls \c clear() to free all memory.
  virtual ~CreatorMap() { CreatorMapBase::clear(); }

  /// The type of object stored in the map.
  typedef CreatorBase<BasePtr> CreatorType;

  /// Get the object associated with the tag \a name.
  CreatorType* getPtrForName(const fstring& name) const
    { return static_cast<CreatorType*>(CreatorMapBase::getPtrForName(name)); }

  /// Associate the object at \a ptr with the tag \a name.
  void setPtrForName(const char* name, CreatorType* ptr)
    { CreatorMapBase::setPtrForName(name, static_cast<void*>(ptr)); }

protected:
  /// Deletes the object at \a ptr.
  virtual void killPtr(void* ptr)
    { delete static_cast<CreatorType*>(ptr); }
};

/**
 *
 * \cFactoryBase exists purely to introduce a virtual destructor into
 * the \c Factory hierarchy.
 *
 **/

class FactoryBase
{
public:
  virtual ~FactoryBase();
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Factory can create objects from an inheritance hierarchy given
 * only a typename. \Factory maintains a mapping from typenames to \c
 * CreatorBase's, and so given a typename can call the \c create()
 * function of the associated \c Creator. All of the product types of
 * a factory must be derived from \c Base. The constructor is
 * protected because factories for specific types should be
 * implemented as singleton classes derived from Factory.
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
      name = demangle_cstr
        (typeid(typename Util::TypeTraits<DerivedPtr>::Pointee).name());

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
    if (creator == 0) itsMap.throwForType(type);
    return creator->create();
  }
};

static const char vcid_factory_h[] = "$Header$";
#endif // !FACTORY_H_DEFINED
