///////////////////////////////////////////////////////////////////////
//
// factory.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 23:40:55 1999
// written: Sat Nov 20 20:54:51 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACTORY_H_DEFINED
#define FACTORY_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef MAP_DEFINED
#include <map>
#define MAP_DEFINED
#endif

#ifndef TYPEINFO_DEFINED
#include <typeinfo>
#define TYPEINFO_DEFINED
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

#ifndef DEMANGLE_H_DEFINED
#include "demangle.h"
#endif

/**
 *
 * Exception class for Factory's.
 *
 **/
class FactoryError : public ErrorWithMsg {
public:
  ///
  FactoryError() : ErrorWithMsg() {}
  ///
  FactoryError(const string& str) : ErrorWithMsg(str) {}
};


namespace {
  const string bad_create_msg = "unable to create object of type ";
}


/**
 *
 * CreatorBase is a template class that defines a single abstract
 * function, create(), that returns an object of type Base.
 *
 **/
template <class Base>
class CreatorBase {
public:
  ///
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
  ///
  virtual Base* create() { return new Derived; }
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * Factory maintains a mapping from type names to CreatorBase's, and
 * in this way is able to create new objects given only a type
 * name. All of the product types of a factory must be derived from
 * Base. The constructors is protected because factories for specific
 * types should be implemented as singleton classes derived from
 * Factory.
 *
 * @memo Creates new objects from a typename.
 **/
///////////////////////////////////////////////////////////////////////

template <class Base>
class Factory {
private:
  typedef map<string, CreatorBase<Base>* > MapType;

  MapType itsMap;

  void clear() {
  	 for (typename MapType::iterator ii = itsMap.begin();
 			ii != itsMap.end(); 
 			++ii) {
  		delete (ii->second);
  		ii->second = 0;
  	 }
  }

protected:
  ///
  Factory() {}
  ///
  virtual ~Factory() { clear(); }

public:
  /** Registers a new type with the factory. The class Derived must be
      a subclass of Base. */
  template <class Derived>
  void registerType(Derived* dummy) {
	 itsMap[demangle(typeid(Derived).name())] = new Creator<Base, Derived>;
  }

  /** Returns a new object of a given type. If the given type has not
      been registered with the factory, a null pointer is returned. */
  Base* newObject(const string& type) {
	 CreatorBase<Base>* creator = itsMap[type];
	 if (creator == 0) return 0;
	 return creator->create();
  }
 
  /** Returns a new object of a given type. If the given type has not
      been registered with the factory, a FactorError is thrown. */
  Base* newCheckedObject(const string& type) {
	 Base* p = newObject(type);
	 if (p == 0) throw FactoryError(bad_create_msg + type);
	 return p;
  }

  /** A convenience function for the case where a Derived* rather than
      Base* to a derived object is desired. A new object is created
      with newCheckedObject, and the result is down-casted to the
      specified Derived class. If the downcast fails, a FactorError is
      thrown. */
  template <class Derived>
  Derived* newTypedObject(const string& type, Derived* dummy) {
	 Base* b = newCheckedObject(type);
	 Derived* d = dynamic_cast<Derived*>(b);
	 if (d == 0) { 
		delete b;
		throw FactoryError(bad_create_msg + type);
	 }
	 return d;
  }
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * FactoryRegistrar objects can register a given type with a factory
 * when it is constructed; thus it can be used as a static class
 * member to ensure that the class is registered with an appropriate
 * factory.
 *
 * @memo Convenience class for registering types with a factory.
 **/
///////////////////////////////////////////////////////////////////////

template <class Base, class Derived>
class FactoryRegistrar {
public:
  FactoryRegistrar(Factory<Base>& theFactory)
  {
	 theFactory.registerType( (Derived*) 0 );
  }
};


static const char vcid_factory_h[] = "$Header$";
#endif // !FACTORY_H_DEFINED
