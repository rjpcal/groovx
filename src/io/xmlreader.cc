///////////////////////////////////////////////////////////////////////
//
// xmlreader.cc
//
// Copyright (c) 2003-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 20 16:09:33 2003
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

#ifndef XMLREADER_CC_DEFINED
#define XMLREADER_CC_DEFINED

#include "io/xmlreader.h"

#include "io/reader.h"
#include "io/readobjectmap.h"
#include "io/xmlparser.h"

#include "util/demangle.h"
#include "util/error.h"
#include "util/pointers.h"
#include "util/ref.h"
#include "util/strings.h"
#include "util/value.h"

#include <cstring>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <typeinfo>
#include <vector>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER


using Util::Ref;
using Util::SoftRef;

class XmlElement;

typedef shared_ptr<XmlElement> ElPtr;

class XmlElement
{
public:
  virtual ~XmlElement();

  virtual void addChild(const char* /*name*/, ElPtr /*elp*/) { Assert(0); }

  virtual void print(std::ostream& os,
                     int depth, const char* name) const = 0;

  virtual void finish() {}
};

XmlElement::~XmlElement() {}

template <class T>
T& elementCast(XmlElement* elp, const fstring& name)
{
  const char* demangled_name(const std::type_info& info);
  if (elp == 0)
    throw Util::Error(fstring("no element with name: ", name));
  T* t = dynamic_cast<T*>(elp);
  if (t == 0)
    throw Util::Error(fstring("wrong element type; expected ",
                              demangled_name(typeid(T)),
                              ", got ",
                              demangled_name(typeid(*elp))));
  return *t;
}

template <class T>
class BasicElement : public XmlElement
{
public:
  BasicElement(T t) : itsVal(t) {}
  virtual ~BasicElement() {}

  virtual void print(std::ostream& os,
                     int depth, const char* name) const
  {
    for (int i = 0; i < depth; ++i) os << "  ";
    os << name << "(" << typeid(T).name() << ") value=" << itsVal << "\n";
  }

  T itsVal;
};

typedef BasicElement<double> DoubleElement;
typedef BasicElement<int> IntElement;
typedef BasicElement<bool> BoolElement;
typedef BasicElement<fstring> StringElement;

class ValueElement : public XmlElement
{
public:
  ValueElement(const char* val) : itsVal(val) {}
  virtual ~ValueElement() {}

  virtual void print(std::ostream& os,
                     int depth, const char* name) const
  {
    for (int i = 0; i < depth; ++i) os << "  ";
    os << name << "(valobj) value=" << itsVal << "\n";
  }

  fstring itsVal;
};

class ObjrefElement : public XmlElement
{
public:
  ObjrefElement(const char** attr, shared_ptr<IO::ObjectMap> objmap) :
    itsType(""),
    itsId(-1),
    itsObjects(objmap)
  {
    for (int i = 0; attr[i] != 0; i += 2)
      {
        if (strcmp(attr[i], "id") == 0)
          itsId = atoi(attr[i+1]);
        else if (strcmp(attr[i], "type") == 0)
          itsType = attr[i+1];
      }

    Assert(itsId >= 0);
    Assert(!itsType.empty());
  }

  virtual ~ObjrefElement() {}

  virtual void print(std::ostream& os,
                     int depth, const char* name) const
  {
    for (int i = 0; i < depth; ++i) os << "  ";
    os << name << "(objref:" << itsType << ") id=" << itsId << "\n";
  }

  Ref<IO::IoObject> getObject()
  {
    return itsObjects->getObject(itsId);
  }

  fstring itsType;
  int itsId;
  shared_ptr<IO::ObjectMap> itsObjects;
};

class GroupElement : public ObjrefElement, public IO::Reader
{
public:
  GroupElement(const char** attr, shared_ptr<IO::ObjectMap> objmap) :
    ObjrefElement(attr, objmap),
    itsVersion(-1),
    itsAttribCount(-1),
    itsElems()
  {
    for (int i = 0; attr[i] != 0; i += 2)
      {
        if (strcmp(attr[i], "version") == 0)
          itsVersion = atoi(attr[i+1]);
        else if (strcmp(attr[i], "attribCount") == 0)
          itsAttribCount = atoi(attr[i+1]);
      }

    Assert(itsVersion >= 0);
    Assert(itsAttribCount >= 0);
  }

  virtual ~GroupElement() throw() {}

  virtual void addChild(const char* name, ElPtr elp)
  {
    itsElems[name] = elp;
  }

  virtual void print(std::ostream& os,
                     int depth, const char* name) const
  {
    for (int i = 0; i < depth; ++i) os << "  ";
    os << name << "(object:" << itsType << "):\n";
    for (MapType::const_iterator
           itr = itsElems.begin(),
           stop = itsElems.end();
         itr != stop;
         ++itr)
      {
        (*itr).second->print(os, depth+1, (*itr).first.c_str());
      }
  }

  virtual IO::VersionId readSerialVersionId()
  {
    return itsVersion;
  }

  virtual char readChar(const fstring& /*name*/) { Assert(0); return '\0'; }

  virtual int readInt(const fstring& name)
  {
    ElPtr el = itsElems[name];
    IntElement& ilp = elementCast<IntElement>(el.get(), name);
    return ilp.itsVal;
  }

  virtual bool readBool(const fstring& name)
  {
    ElPtr el = itsElems[name];
    BoolElement& blp = elementCast<BoolElement>(el.get(), name);
    return blp.itsVal;
  }

  virtual double readDouble(const fstring& name)
  {
    ElPtr el = itsElems[name];
    DoubleElement& dlp = elementCast<DoubleElement>(el.get(), name);
    return dlp.itsVal;
  }

  virtual void readValueObj(const fstring& name, Value& value)
  {
    ElPtr el = itsElems[name];
    ValueElement& vlp = elementCast<ValueElement>(el.get(), name);
    value.setFstring(vlp.itsVal);
  }

  virtual Ref<IO::IoObject> readObject(const fstring& name)
  {
    return readMaybeObject(name);
  }

  virtual SoftRef<IO::IoObject> readMaybeObject(const fstring& name);

  virtual void readOwnedObject(const fstring& name,
                               Ref<IO::IoObject> obj)
  {
    ElPtr el = itsElems[name];
    GroupElement& glp = elementCast<GroupElement>(el.get(), name);
    glp.inflate(*obj);
  }

  virtual void readBaseClass(const fstring& name,
                             Ref<IO::IoObject> basePart)
  {
    ElPtr el = itsElems[name];
    GroupElement& glp = elementCast<GroupElement>(el.get(), name);
    glp.inflate(*basePart);
  }

  virtual Ref<IO::IoObject> readRoot(IO::IoObject* /*root*/)
  {
    Assert(0); return Ref<IO::IoObject>((IO::IoObject*)0);
  }

  void inflate(IO::IoObject& obj)
  {
    obj.readFrom(*this);
  }

protected:
  virtual fstring readStringImpl(const fstring& name)
  {
    ElPtr el = itsElems[name];
    StringElement& slp = elementCast<StringElement>(el.get(), name);
    return slp.itsVal;
  }

public:
  int itsVersion;
  int itsAttribCount;
  typedef std::map<fstring, ElPtr> MapType;
  MapType itsElems;
};

class ObjectElement : public GroupElement
{
public:
  ObjectElement(const char** attr, shared_ptr<IO::ObjectMap> objmap) :
    GroupElement(attr, objmap)
  {
    // Return the object for this id, creating a new object if necessary:
    itsObject = objmap->fetchObject(itsType.c_str(), itsId);
  }

  SoftRef<IO::IoObject> itsObject;

  virtual void finish()
  {
    if (itsObject.isValid())
      inflate(*itsObject);
  }
};

SoftRef<IO::IoObject> GroupElement::readMaybeObject(const fstring& name)
{
  ElPtr el = itsElems[name];
  ObjrefElement& olp = elementCast<ObjrefElement>(el.get(), name);
  return olp.getObject();
}

class RootElement
{
public:
  RootElement() : itsChild(0) {}

  void addChild(const char*, ElPtr elp)
  {
    Assert(itsChild.get() == 0);
    itsChild = elp;
  }

  void print(std::ostream& os, int depth, const char* name) const
  {
    if (itsChild.get() == 0) return;
    Assert(itsChild.get() != 0);
    itsChild->print(os, depth, name);
  }

  ElPtr itsChild;
};

ElPtr makeBasicElement(const char* el, const char** attr)
{
  Assert(strcmp(el, "object") != 0);

  const char* name = 0;
  const char* val = 0;

  for (int i = 0; attr[i] != 0; i += 2)
    {
      if (strcmp(attr[i], "name") == 0)
        name = attr[i+1];
      else if (strcmp(attr[i], "value") == 0)
        val = attr[i+1];
    }

  if (val == 0 || name == 0)
    {
      fprintf(stderr, "el = %s\n", el);
    }

  Assert(name != 0);
  Assert(val != 0);

  if (strcmp(el, "double") == 0)
    {
      double d = 0.0;
      sscanf(val, "%lf", &d);
      return ElPtr(new DoubleElement(d));
    }
  else if (strcmp(el, "int") == 0)
    {
      int i = 0;
      sscanf(val, "%d", &i);
      return ElPtr(new IntElement(i));
    }
  else if (strcmp(el, "bool") == 0)
    {
      int i = 0;
      sscanf(val, "%d", &i);
      return ElPtr(new BoolElement(bool(i)));
    }
  else if (strcmp(el, "cstring") == 0)
    {
      return ElPtr(new StringElement(val));
    }
  else if (strcmp(el, "valobj") == 0)
    {
      return ElPtr(new ValueElement(val));
    }
  else
    {
      throw Util::Error(fstring("unknown element type: ", el));
    }
}

ElPtr makeElement(const char* el, const char** attr,
                  shared_ptr<IO::ObjectMap> objmap)
{
  if (strcmp(el, "object") == 0)
    {
      return ElPtr(new ObjectElement(attr, objmap));
    }
  else if (strcmp(el, "ownedobj") == 0)
    {
      return ElPtr(new GroupElement(attr, objmap));
    }
  else if (strcmp(el, "baseclass") == 0)
    {
      return ElPtr(new GroupElement(attr, objmap));
    }
  else if (strcmp(el, "objref") == 0)
    {
      return ElPtr(new ObjrefElement(attr, objmap));
    }
  else
    {
      return makeBasicElement(el, attr);
    }
}

class TreeBuilder : public XmlParser
{
public:
  TreeBuilder(std::istream& is) :
    XmlParser(is),
    itsRoot(new RootElement),
    itsStack(),
    itsDepth(0),
    itsStartCount(0),
    itsEndCount(0),
    itsObjects(new IO::ObjectMap),
    itsElCount(0)
  {}

  virtual ~TreeBuilder() {}

  shared_ptr<RootElement> getRoot() const
  {
    if (itsStack.size() != 0)
      {
        dbgEvalNL(0, itsDepth);
        dbgEvalNL(0, itsStack.size());
        dbgEvalNL(0, typeid(*itsStack.back()).name());
      }
    Assert(itsStack.size() == 0);
    return itsRoot;
  }

protected:
  virtual void elementStart(const char* el, const char** attr);
  virtual void elementEnd(const char* el);

private:
  shared_ptr<RootElement> itsRoot;
  std::vector<ElPtr> itsStack;
  int itsDepth;
  int itsStartCount;
  int itsEndCount;
  shared_ptr<IO::ObjectMap> itsObjects;
  int itsElCount;
};

void TreeBuilder::elementStart(const char* el, const char** attr)
{
  ++itsElCount;
  ++itsStartCount;
  ++itsDepth;

  const char* name = 0;

  for (int i = 0; attr[i] != 0; i += 2)
    {
      if (strcmp(attr[i], "name") == 0)
        {
          name = attr[i+1];
          break;
        }
    }

  Assert(name != 0);

  ElPtr elp = makeElement(el, attr, itsObjects);

  if (itsStack.size() == 0)
    itsRoot->addChild(name, elp);
  else
    itsStack.back()->addChild(name, elp);

  itsStack.push_back(elp);

  if (GET_DBG_LEVEL() >= 3)
    {
      dbgEval(3, itsElCount);
      dbgEval(3, el);
      dbgEval(3, name);
      dbgEval(3, elp.get());
      dbgEvalNL(3, itsStack.size());
    }
}

void TreeBuilder::elementEnd(const char* /*el*/)
{
  --itsEndCount;
  Assert(itsStack.size() > 0);
  Assert(itsStack.back().get() != 0);
  itsStack.back()->finish();
  itsStack.pop_back();
  --itsDepth;
}


#include <iostream>
#include <fstream>

Util::Ref<IO::IoObject> IO::loadXML(const char* filename)
{
  std::ifstream ifs(filename);
  TreeBuilder x(ifs);
  x.parse();

  shared_ptr<RootElement> root = x.getRoot();
  XmlElement* elp = root->itsChild.get();
  if (elp == 0)
    throw Util::Error("no child");
  ObjectElement* olm = dynamic_cast<ObjectElement*>(elp);
  return olm->itsObject;
}

void IO::xmlDebug(const char* filename)
{
  std::ifstream ifs(filename);
  TreeBuilder x(ifs);
  x.parse();

  shared_ptr<RootElement> root = x.getRoot();
  root->print(std::cout, 0, "root");
}

static const char vcid_xmlreader_cc[] = "$Header$";
#endif // !XMLREADER_CC_DEFINED
