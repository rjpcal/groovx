///////////////////////////////////////////////////////////////////////
//
// xmlreader.cc
//
// Copyright (c) 2003-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 20 16:09:33 2003
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

#ifndef XMLREADER_CC_DEFINED
#define XMLREADER_CC_DEFINED

#include "io/xmlreader.h"

#include "io/reader.h"
#include "io/readobjectmap.h"
#include "io/xmlparser.h"

#include "util/demangle.h"
#include "util/error.h"
#include "util/gzstreambuf.h"
#include "util/pointers.h"
#include "util/ref.h"
#include "util/strings.h"
#include "util/value.h"

#include <cstring>           // for strcmp()
#include <iostream>          // for cout in xmlDebug()
#include <istream>           // for TreeBuilder constructor
#include <map>               // for GroupElement
#include <ostream>           // for XmlElement::trace()
#include <typeinfo>          // for error reporting and XmlElement::trace()
#include <vector>            // for stack in TreeBuilder

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using Util::Ref;
using Util::SoftRef;

namespace
{
  void invalidAttr(const char* attname, const char* eltype,
                   const char* elname, const FilePosition& pos)
  {
    throw Util::Error(fstring("invalid '", attname,
                              "' attribute for <",
                              eltype, "> element with name: ", elname),
                      pos);
  }

  const char* findAttr(const char** attr, const char* attname,
                       const char* eltype, const char* elname,
                       const FilePosition& pos)
  {
    for (int i = 0; attr[i] != 0; i += 2)
      {
        if (strcmp(attr[i], attname) == 0)
          return attr[i+1];
      }

    throw Util::Error(fstring("missing '", attname,
                              "' attribute for <",
                              eltype, "> element with name: ", elname),
                      pos);

    Assert(0);
    return 0; // can't happen
  }

  class XmlElement;

  typedef shared_ptr<XmlElement> ElPtr;

  class XmlElement
  {
  public:
    virtual ~XmlElement();

    virtual void addChild(const char* /*name*/, ElPtr /*elp*/)
    {
      throw Util::Error(fstring("child elements not allowed "
                                "within elements of type: ",
                                demangled_name(typeid(*this))),
                        SRC_POS);
    }

    virtual void characterData(const char* /*text*/, int /*len*/) { }

    virtual void trace(std::ostream& os,
                       int depth, const char* name) const = 0;

    virtual void finish() {}
  };

  XmlElement::~XmlElement() {}

  template <class T>
  T& elementCast(XmlElement* elp, const fstring& name,
                 const FilePosition& pos)
  {
    if (elp == 0)
      throw Util::Error(fstring("no element with name: ", name), SRC_POS);
    T* t = dynamic_cast<T*>(elp);
    if (t == 0)
      throw Util::Error(fstring("wrong element type; expected ",
                                demangled_name(typeid(T)),
                                ", got ",
                                demangled_name(typeid(*elp))),
                        pos);
    return *t;
  }

  template <class T>
  class BasicElement : public XmlElement
  {
  public:
    BasicElement(const char* str, const char* name);
    virtual ~BasicElement() {}

    virtual void trace(std::ostream& os,
                       int depth, const char* name) const
    {
      for (int i = 0; i < depth; ++i) os << "  ";
      os << name << "(" << typeid(T).name() << ") value=" << itsVal << "\n";
    }

    T itsVal;
  };

  template <>
  BasicElement<double>::BasicElement(const char* str, const char* name) :
    itsVal(0.0)
  {
    int n = sscanf(str, "%lf", &itsVal);
    if (n != 1)
      {
        invalidAttr("value", "double", name, SRC_POS);
      }
  }

  template <>
  BasicElement<int>::BasicElement(const char* str, const char* name) :
    itsVal(0)
  {
    int n = sscanf(str, "%d", &itsVal);
    if (n != 1)
      {
        invalidAttr("value", "int", name, SRC_POS);
      }
  }

  template <>
  BasicElement<bool>::BasicElement(const char* str, const char* name) :
    itsVal(true)
  {
    int i = 0;
    int n = sscanf(str, "%d", &i);
    if (n != 1)
      {
        invalidAttr("value", "bool", name, SRC_POS);
      }
    itsVal = bool(i);
  }

  class StringElement : public XmlElement
  {
  public:
    StringElement() : itsVal() {}
    virtual ~StringElement() {}

    virtual void trace(std::ostream& os,
                       int depth, const char* name) const
    {
      for (int i = 0; i < depth; ++i) os << "  ";
      os << name << "(string) value=" << itsVal << "\n";
    }

    virtual void characterData(const char* text, int len)
    {
      itsVal.append_range(text, len);
    }

    fstring itsVal;
  };

  typedef BasicElement<double> DoubleElement;
  typedef BasicElement<int> IntElement;
  typedef BasicElement<bool> BoolElement;

  class ValueElement : public XmlElement
  {
  public:
    ValueElement(const char* val, const char* /*name*/) : itsVal(val) {}
    virtual ~ValueElement() {}

    virtual void trace(std::ostream& os,
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
    ObjrefElement(const char** attr, const char* eltype, const char* name,
                  shared_ptr<IO::ObjectMap> objmap) :
      itsType(""),
      itsId(-1),
      itsObjects(objmap)
    {
      itsId = atoi(findAttr(attr, "id", eltype, name, SRC_POS));
      itsType = findAttr(attr, "type", eltype, name, SRC_POS);

      if (itsId < 0)
        {
          invalidAttr("id", eltype, name, SRC_POS);
        }

      Assert(itsId >= 0);

      if (itsType.empty())
        {
          invalidAttr("type", eltype, name, SRC_POS);
        }

      Assert(!itsType.empty());
    }

    virtual ~ObjrefElement() {}

    virtual void trace(std::ostream& os,
                       int depth, const char* name) const
    {
      for (int i = 0; i < depth; ++i) os << "  ";
      os << name << "(objref:" << itsType << ") id=" << itsId << "\n";
    }

    SoftRef<IO::IoObject> getObject()
    {
      if (itsId == 0)
        return SoftRef<IO::IoObject>();
      // else...
      return itsObjects->getObject(itsId);
    }

    fstring itsType;
    int itsId;
    shared_ptr<IO::ObjectMap> itsObjects;
  };

  class GroupElement : public ObjrefElement, public IO::Reader
  {
  public:
    GroupElement(const char** attr, const char* eltype, const char* name,
                 shared_ptr<IO::ObjectMap> objmap) :
      ObjrefElement(attr, eltype, name, objmap),
      itsVersion(-1),
      itsElems()
    {
      itsVersion = atoi(findAttr(attr, "version", eltype, name, SRC_POS));

      if (itsVersion < 0)
        {
          invalidAttr("version", eltype, name, SRC_POS);
        }
      Assert(itsVersion >= 0);
    }

    virtual ~GroupElement() throw() {}

    virtual void addChild(const char* name, ElPtr elp)
    {
      itsElems[name] = elp;
    }

    virtual void trace(std::ostream& os,
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
          (*itr).second->trace(os, depth+1, (*itr).first.c_str());
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
      IntElement& ilp = elementCast<IntElement>(el.get(), name, SRC_POS);
      return ilp.itsVal;
    }

    virtual bool readBool(const fstring& name)
    {
      ElPtr el = itsElems[name];
      BoolElement& blp = elementCast<BoolElement>(el.get(), name, SRC_POS);
      return blp.itsVal;
    }

    virtual double readDouble(const fstring& name)
    {
      ElPtr el = itsElems[name];
      DoubleElement& dlp = elementCast<DoubleElement>(el.get(), name, SRC_POS);
      return dlp.itsVal;
    }

    virtual void readValueObj(const fstring& name, Value& value)
    {
      ElPtr el = itsElems[name];
      ValueElement& vlp = elementCast<ValueElement>(el.get(), name, SRC_POS);
      value.setFstring(vlp.itsVal);
    }

    virtual void readRawData(const fstring& name, rutz::byte_array& data)
    {
      defaultReadRawData(name, data);
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
      GroupElement& glp = elementCast<GroupElement>(el.get(), name, SRC_POS);
      glp.inflate(*obj);
    }

    virtual void readBaseClass(const fstring& name,
                               Ref<IO::IoObject> basePart)
    {
      ElPtr el = itsElems[name];
      GroupElement& glp = elementCast<GroupElement>(el.get(), name, SRC_POS);
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
      StringElement& slp = elementCast<StringElement>(el.get(), name, SRC_POS);
      return slp.itsVal;
    }

  public:
    int itsVersion;
    typedef std::map<fstring, ElPtr> MapType;
    MapType itsElems;
  };

  class ObjectElement : public GroupElement
  {
  public:
    ObjectElement(const char** attr, const char* name,
                  shared_ptr<IO::ObjectMap> objmap) :
      GroupElement(attr, "object", name, objmap)
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
    ObjrefElement& olp = elementCast<ObjrefElement>(el.get(), name, SRC_POS);
    return olp.getObject();
  }

  ElPtr makeElement(const char* el, const char** attr, const char* name,
                    shared_ptr<IO::ObjectMap> objmap)
  {
    if (strcmp(el, "object") == 0)
      {
        return ElPtr(new ObjectElement(attr, name, objmap));
      }
    else if (strcmp(el, "ownedobj") == 0)
      {
        return ElPtr(new GroupElement(attr, "ownedobj", name, objmap));
      }
    else if (strcmp(el, "baseclass") == 0)
      {
        return ElPtr(new GroupElement(attr, "baseclass", name, objmap));
      }
    else if (strcmp(el, "objref") == 0)
      {
        return ElPtr(new ObjrefElement(attr, "objref", name, objmap));
      }
    else if (strcmp(el, "string") == 0)
      {
        return ElPtr(new StringElement);
      }
    else
      {
        const char* val = findAttr(attr, "value", el, name, SRC_POS);

        if (strcmp(el, "double") == 0)
          {
            return ElPtr(new DoubleElement(val, name));
          }
        else if (strcmp(el, "int") == 0)
          {
            return ElPtr(new IntElement(val, name));
          }
        else if (strcmp(el, "bool") == 0)
          {
            return ElPtr(new BoolElement(val, name));
          }
        else if (strcmp(el, "valobj") == 0)
          {
            return ElPtr(new ValueElement(val, name));
          }
        else
          {
            throw Util::Error(fstring("unknown element type: ", el), SRC_POS);
          }
      }
  }

  class TreeBuilder : public XmlParser
  {
  public:
    TreeBuilder(std::istream& is) :
      XmlParser(is),
      itsRoot(),
      itsStack(),
      itsDepth(0),
      itsStartCount(0),
      itsEndCount(0),
      itsObjects(new IO::ObjectMap),
      itsElCount(0)
    {}

    virtual ~TreeBuilder() {}

    ObjectElement& getRoot() const
    {
      if (itsRoot.get() == 0)
        {
          throw Util::Error("no root element found", SRC_POS);
        }

      return elementCast<ObjectElement>(itsRoot.get(), "root", SRC_POS);
    }

  protected:
    virtual void elementStart(const char* el, const char** attr);
    virtual void elementEnd(const char* el);
    virtual void characterData(const char* text, int length);

  private:
    ElPtr itsRoot;
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

    const char* name = findAttr(attr, "name", el, "(noname)", SRC_POS);

    Assert(name != 0);

    ElPtr elp = makeElement(el, attr, name, itsObjects);

    if (itsStack.size() > 0)
      itsStack.back()->addChild(name, elp);
    else
      itsRoot = elp;

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

  void TreeBuilder::characterData(const char* text, int length)
  {
    Assert(itsStack.size() > 0);
    Assert(itsStack.back().get() != 0);
    itsStack.back()->characterData(text, length);
  }

} // end anonymous namespace


Util::Ref<IO::IoObject> IO::loadGVX(const char* filename)
{
DOTRACE("IO::loadGVX");
  shared_ptr<std::istream> ifs(Util::igzopen(filename));
  TreeBuilder x(*ifs);
  x.parse();

  ObjectElement& root = x.getRoot();
  return root.itsObject;
}

void IO::xmlDebug(const char* filename)
{
DOTRACE("IO::xmlDebug");
  shared_ptr<std::istream> ifs(Util::igzopen(filename));
  TreeBuilder x(*ifs);
  x.parse();

  ObjectElement& root = x.getRoot();
  root.trace(std::cout, 0, "root");
}

static const char vcid_xmlreader_cc[] = "$Header$";
#endif // !XMLREADER_CC_DEFINED
