///////////////////////////////////////////////////////////////////////
//
// factory.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Jun 26 23:40:55 1999
// written: Wed Jun 30 17:04:44 1999
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

class FactoryError : public ErrorWithMsg {
public:
  FactoryError() : ErrorWithMsg() {}
  FactoryError(const string& str) : ErrorWithMsg(str) {}
};

namespace {
  const string bad_create_msg = "unable to create object of type ";
}

template <class Base>
class CreatorBase {
public:
  virtual Base* create() = 0;
};

template <class Base, class Derived>
class Creator : public CreatorBase<Base> {
public:
  virtual Base* create() { return new Derived; }
};

template <class Base>
class Factory {
private:
  typedef map<string, CreatorBase<Base>* > MapType;

protected:
  Factory() {}
  virtual ~Factory() { clear(); }

public:
  template <class Derived>
  void registerType(Derived* dummy) {
	 itsMap[typeid(Derived).name()] = new Creator<Base, Derived>;
  }

  Base* newObject(const string& type) {
	 CreatorBase<Base>* creator = itsMap[type];
	 if (creator == 0) return 0;
	 return creator->create();
  }

  Base* newCheckedObject(const string& type) {
	 Base* p = newObject(type);
	 if (p == 0) throw FactoryError(bad_create_msg + type);
	 return p;
  }

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

private:
  MapType itsMap;

  void clear() {
#if 0
  	 for (MapType::const_iterator ii = itsMap.begin();
 			ii != itsMap.end(); 
 			++ii) {
  		delete (ii->second);
  		ii->second = 0;
  	 }
#endif
  }
};

// FactoryRegistrar objects can register a given type with a factory
// when it is constructed; thus it can be used as a static class
// member to ensure that the class is registered with an appropriate
// factory.
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
