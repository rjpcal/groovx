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

#include "io/xmlparser.h"

#include "util/error.h"

#include <cstring>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

class XmlElement;

typedef shared_ptr<XmlElement> ElPtr;

class XmlElement
{
public:
  virtual ~XmlElement();

  virtual void addChild(const char* /*name*/, ElPtr /*elp*/) { Assert(0); }

  virtual void print(std::ostream& os,
                     int depth, const char* name) const = 0;
};

XmlElement::~XmlElement() {}

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
typedef BasicElement<std::string> StringElement;

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

  std::string itsVal;
};

class ObjrefElement : public XmlElement
{
public:
  ObjrefElement(const char** attr) :
    itsType(""),
    itsId(-1)
  {
    for (int i = 0; attr[i] != 0; i += 2)
      {
        if (strcmp(attr[i], "id") == 0)
          itsId = atoi(attr[i+1]);
        else if (strcmp(attr[i], "type") == 0)
          itsType = attr[i+1];
      }

    Assert(itsId != -1);
    Assert(!itsType.empty());
  }

  virtual ~ObjrefElement() {}

  virtual void print(std::ostream& os,
                     int depth, const char* name) const
  {
    for (int i = 0; i < depth; ++i) os << "  ";
    os << name << "(objref:" << itsType << ") id=" << itsId << "\n";
  }

  std::string itsType;
  int itsId;
};

class ObjectElement : public XmlElement
{
public:
  ObjectElement(const char** attr) :
    itsType(""),
    itsId(-1),
    itsVersion(-1),
    itsAttribCount(-1),
    itsElements()
  {
    for (int i = 0; attr[i] != 0; i += 2)
      {
        if (strcmp(attr[i], "id") == 0)
          itsId = atoi(attr[i+1]);
        else if (strcmp(attr[i], "version") == 0)
          itsVersion = atoi(attr[i+1]);
        else if (strcmp(attr[i], "attribCount") == 0)
          itsAttribCount = atoi(attr[i+1]);
        else if (strcmp(attr[i], "type") == 0)
          itsType = attr[i+1];
      }

    Assert(itsId != -1);
    Assert(itsVersion != -1);
    Assert(itsAttribCount != -1);
    Assert(!itsType.empty());
  }

  virtual ~ObjectElement() {}

  virtual void addChild(const char* name, ElPtr elp)
  {
    itsElements[name] = elp;
  }

  virtual void print(std::ostream& os,
                     int depth, const char* name) const
  {
    for (int i = 0; i < depth; ++i) os << "  ";
    os << name << "(object:" << itsType << "):\n";
    for (MapType::const_iterator
           itr = itsElements.begin(),
           stop = itsElements.end();
         itr != stop;
         ++itr)
      {
        (*itr).second->print(os, depth+1, (*itr).first.c_str());
      }
  }

  std::string itsType;
  int itsId;
  int itsVersion;
  int itsAttribCount;
  typedef std::map<std::string, ElPtr> MapType;
  MapType itsElements;
};

class RootElement : public XmlElement
{
public:
  RootElement() : itsChild(0) {}

  virtual void addChild(const char*, ElPtr elp)
  {
    Assert(itsChild.get() == 0);
    itsChild = elp;
  }

  virtual void print(std::ostream& os,
                     int depth, const char* name) const
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

ElPtr makeElement(const char* el, const char** attr)
{
  if (strcmp(el, "object") == 0)
    {
      return ElPtr(new ObjectElement(attr));
    }
  else if (strcmp(el, "objref") == 0)
    {
      return ElPtr(new ObjrefElement(attr));
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
    itsStack(),
    itsElCount(0)
  {
    itsStack.push_back(ElPtr(new RootElement));
  }

  virtual ~TreeBuilder()
  {
    Assert(itsStack.size() == 1);
  }

  ElPtr getRoot() const
  {
    Assert(itsStack.size() == 1);

    return itsStack.back();
  }

protected:
  virtual void elementStart(const char* el, const char** attr);
  virtual void elementEnd(const char* el);

private:
  std::vector<ElPtr> itsStack;
  int itsElCount;
};

void TreeBuilder::elementStart(const char* el, const char** attr)
{
  ++itsElCount;

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

  ElPtr elp = makeElement(el, attr);

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
  itsStack.pop_back();
}


#include <iostream>
#include <fstream>

void IO::xmlDebug(const char* filename)
{
  std::ifstream ifs(filename);
  TreeBuilder x(ifs);
  x.parse();

  ElPtr root = x.getRoot();
  root->print(std::cout, 0, "root");
}

#if 0
using Util::Ref;
using Util::SoftRef;

class XMLReader : public IO::Reader
{
public:
  XMLReader(STD_IO::istream& is);

  virtual ~XMLReader() throw();

  virtual IO::VersionId readSerialVersionId();

  virtual char readChar(const fstring& name);
  virtual int readInt(const fstring& name);
  virtual bool readBool(const fstring& name);
  virtual double readDouble(const fstring& name);
  virtual void readValueObj(const fstring& name, Value& value);

  virtual Ref<IO::IoObject> readObject(const fstring& name);
  virtual SoftRef<IO::IoObject> readMaybeObject(const fstring& name);

  virtual void readOwnedObject(const fstring& name,
                               Ref<IO::IoObject> obj);
  virtual void readBaseClass(const fstring& baseClassName,
                             Ref<IO::IoObject> basePart);

  virtual Ref<IO::IoObject> readRoot(IO::IoObject* root=0);

protected:
  virtual fstring readStringImpl(const fstring& name);
};

XMLReader::XMLReader(STD_IO::istream& is);

XMLReader::~XMLReader() throw();

IO::VersionId XMLReader::readSerialVersionId();

char XMLReader::readChar(const fstring& name);
int XMLReader::readInt(const fstring& name);
bool XMLReader::readBool(const fstring& name);
double XMLReader::readDouble(const fstring& name);
void XMLReader::readValueObj(const fstring& name, Value& value);

Ref<IO::IoObject> XMLReader::readObject(const fstring& name);
SoftRef<IO::IoObject> XMLReader::readMaybeObject(const fstring& name);

void XMLReader::readOwnedObject(const fstring& name,
                                Ref<IO::IoObject> obj);
void XMLReader::readBaseClass(const fstring& baseClassName,
                              Ref<IO::IoObject> basePart);

Ref<IO::IoObject> XMLReader::readRoot(IO::IoObject* root=0);

fstring XMLReader::readStringImpl(const fstring& name);
#else
#endif
static const char vcid_xmlreader_cc[] = "$Header$";
#endif // !XMLREADER_CC_DEFINED
