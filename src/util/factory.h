///////////////////////////////////////////////////////////////////////
//
// factory.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 23:40:55 1999
// written: Thu Mar 30 12:28:53 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACTORY_H_DEFINED
#define FACTORY_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(DEMANGLE_H_DEFINED)
#include "system/demangle.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TYPEINFO_DEFINED)
#include <typeinfo>
#define TYPEINFO_DEFINED
#endif

/**
 *
 * Exception class for \c Factory's.
 *
 **/
class FactoryError : public ErrorWithMsg {
public:
  /// Construct with an informative message \a msg.
  FactoryError(const char* str) : ErrorWithMsg(str) {}

  static void throwForType(const char* type);
};


/**
 *
 * \c CreatorBase is a template class that defines a single abstract
 * function, \c create(), that returns an object of type \c Base.
 *
 **/
template <class Base>
class CreatorBase {
public:
  /// Return a new object of type \c Base.
  virtual Base* create() = 0;
};


/**
 *
 * Creator implements the CreatorBase interface by returning a new
 * object of type Derived from the create() method. Derived must have
 * a default constructor, and must be a subclass of Base.
 *
 **/
template <class Base, class Derived>
class Creator : public CreatorBase<Base> {
public:
  /// Return a new object of type \c Derived.
  virtual Base* create() { return new Derived; }
};


/**
 *
 * \c CreatorMapBase provides a non-typesafe wrapper around \c
 * std::map. The \c killPtr() function should be overridden to delete
 * the pointer after casting it to its true type.
 *
 **/

class CreatorMapBase {
public:
  /// Default constructor.
  CreatorMapBase();

  /// Virtual destructor.
  virtual ~CreatorMapBase();

protected:
  /// Retrieve the object associated with the tag \a name.
  void* getPtrForName(const char* name) const;

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

template<class Base>
class CreatorMap : private CreatorMapBase {
public:
  /// Virtual destructor calls \c clear() to free all memory.
  virtual ~CreatorMap() { CreatorMapBase::clear(); }

  /// The type of object stored in the map.
  typedef CreatorBase<Base> CreatorType;

  /// Get the object associated with the tag \a name.
  CreatorType* getPtrForName(const char* name) const
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

class FactoryBase {
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

template <class Base>
class Factory : public FactoryBase {
private:
  CreatorMap<Base> itsMap;

protected:
  /// Default constructor.
  Factory() : itsMap() {}

public:
  /** Registers a new type with the factory. The class Derived must be
      a subclass of Base. */
  template <class Derived>
  void registerType(Derived* /*dummy*/) {
	 itsMap.setPtrForName(demangle_cstr(typeid(Derived).name()),
								 new Creator<Base, Derived>);
  }

  /** Returns a new object of a given type. If the given type has not
      been registered with the factory, a null pointer is returned. */
  Base* newObject(const char* type) {
	 CreatorBase<Base>* creator = itsMap.getPtrForName(type);
	 if (creator == 0) return 0;
	 return creator->create();
  }
 
  /** Returns a new object of a given type. If the given type has not
      been registered with the factory, a FactorError is thrown. */
  Base* newCheckedObject(const char* type) {
	 Base* p = newObject(type);
	 if (p == 0) FactoryError::throwForType(type);
	 return p;
  }

  /** A convenience function for the case where a Derived* rather than
      Base* to a derived object is desired. A new object is created
      with newCheckedObject, and the result is down-casted to the
      specified Derived class. If the downcast fails, a FactorError is
      thrown. */
  template <class Derived>
  Derived* newTypedObject(const char* type, Derived* /*dummy*/) {
	 Base* b = newCheckedObject(type);
	 Derived* d = dynamic_cast<Derived*>(b);
	 if (d == 0) { 
		delete b;
		FactoryError::throwForType(type);
	 }
	 return d;
  }
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * \c FactoryRegistrar is a convenience class for registering types
 * with a factory. \c FactoryRegistrar objects should not be
 * instantiated; instead, clients simply use the static function \c
 * registerWith() to register the \c Derived type with a \c Factory
 * rooted in the \c Base type.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class Base, class Derived>
class FactoryRegistrar {
protected:
  /// Default constructor protected so that clients don't instantiate this class.
  FactoryRegistrar();

public:
  /** Registers a creator for type \c Derived with \a theFactory,
      which is rooted in type \c Base. \c Derived must be a class
      derived from \c Base. */
  static void registerWith(Factory<Base>& theFactory)
	 {
		theFactory.registerType( (Derived*) 0 );
	 }
};


static const char vcid_factory_h[] = "$Header$";
#endif // !FACTORY_H_DEFINED
