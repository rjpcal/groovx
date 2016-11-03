/** @file io/xmlreader.cc io::reader implementation for reading XML
    files in the GVX format */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jun 20 16:09:33 2003
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#include "io/xmlreader.h"

#include "io/reader.h"
#include "io/readobjectmap.h"
#include "io/xmlparser.h"

#include "nub/ref.h"

#include "rutz/demangle.h"
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/gzstreambuf.h"
#include "rutz/sfmt.h"
#include "rutz/shared_ptr.h"
#include "rutz/value.h"

#include <cstdio>            // for sscanf()
#include <cstring>           // for strcmp()
#include <iostream>          // for cout in xml_debug()
#include <istream>           // for tree_builder constructor
#include <map>               // for group_element
#include <ostream>           // for xml_element::trace()
#include <string>            // for string_element implementation
#include <typeinfo>          // for error reporting and xml_element::trace()
#include <vector>            // for stack in tree_builder

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;
using rutz::shared_ptr;

using nub::ref;
using nub::soft_ref;

namespace
{
  [[noreturn]] void invalid_attr(const char* attname, const char* eltype,
                                 const char* elname, const rutz::file_pos& pos)
  {
    throw rutz::error(rutz::sfmt("invalid '%s' attribute for <%s> "
                                 "element with name: %s",
                                 attname, eltype, elname),
                      pos);
  }

  const char* find_attr(const char** attr, const char* attname,
                        const char* eltype, const char* elname,
                        const rutz::file_pos& pos)
  {
    for (int i = 0; attr[i] != nullptr; i += 2)
      {
        if (strcmp(attr[i], attname) == 0)
          return attr[i+1];
      }

    throw rutz::error(rutz::sfmt("missing '%s' attribute for <%s> "
                                 "element with name: %s",
                                 attname, eltype, elname),
                      pos);

    GVX_ASSERT(0);
    return 0; // can't happen
  }

  class xml_element;

  typedef shared_ptr<xml_element> el_ptr;

  class xml_element
  {
  public:
    virtual ~xml_element();

    virtual void add_child(const char* /*name*/, el_ptr /*elp*/)
    {
      throw rutz::error(rutz::sfmt("child elements not allowed "
                                   "within elements of type: %s",
                                   rutz::demangled_name(typeid(*this))),
                        SRC_POS);
    }

    virtual void character_data(const char* /*text*/, size_t /*len*/) { }

    virtual void trace(std::ostream& os,
                       int depth, const char* name) const = 0;

    virtual void finish() {}
  };

  xml_element::~xml_element() {}

  template <class T>
  T& element_cast(xml_element* elp, const fstring& name,
                 const rutz::file_pos& pos)
  {
    if (elp == nullptr)
      throw rutz::error(rutz::sfmt("no element with name: %s",
                                   name.c_str()),
                        SRC_POS);
    T* t = dynamic_cast<T*>(elp);
    if (t == nullptr)
      throw rutz::error(rutz::sfmt("wrong element type; expected %s, got %s",
                                   rutz::demangled_name(typeid(T)),
                                   rutz::demangled_name(typeid(*elp))),
                        pos);
    return *t;
  }

  template <class T>
  class basic_element : public xml_element
  {
  public:
    basic_element(const char* str, const char* name);
    virtual ~basic_element() {}

    virtual void trace(std::ostream& os,
                       int depth, const char* name) const
    {
      for (int i = 0; i < depth; ++i) os << "  ";
      os << name << "(" << typeid(T).name() << ") value=" << m_value << "\n";
    }

    T m_value;
  };

  template <>
  basic_element<double>::basic_element(const char* str, const char* name) :
    m_value(0.0)
  {
    int n = sscanf(str, "%lf", &m_value);
    if (n != 1)
      {
        invalid_attr("value", "double", name, SRC_POS);
      }
  }

  template <>
  basic_element<int>::basic_element(const char* str, const char* name) :
    m_value(0)
  {
    int n = sscanf(str, "%d", &m_value);
    if (n != 1)
      {
        invalid_attr("value", "int", name, SRC_POS);
      }
  }

  template <>
  basic_element<bool>::basic_element(const char* str, const char* name) :
    m_value(true)
  {
    int i = 0;
    int n = sscanf(str, "%d", &i);
    if (n != 1)
      {
        invalid_attr("value", "bool", name, SRC_POS);
      }
    m_value = bool(i);
  }

  class string_element : public xml_element
  {
  public:
    string_element() : m_value() {}
    virtual ~string_element() {}

    virtual void trace(std::ostream& os,
                       int depth, const char* name) const override
    {
      for (int i = 0; i < depth; ++i) os << "  ";
      os << name << "(string) value=" << m_value << "\n";
    }

    virtual void character_data(const char* text, size_t len) override
    {
      m_value.append(text, len);
    }

    std::string m_value;
  };

  typedef basic_element<double> double_element;
  typedef basic_element<int> int_element;
  typedef basic_element<bool> bool_element;

  class value_element : public xml_element
  {
  public:
    value_element(const char* val, const char* /*name*/) : m_value(val) {}
    virtual ~value_element() {}

    virtual void trace(std::ostream& os,
                       int depth, const char* name) const
    {
      for (int i = 0; i < depth; ++i) os << "  ";
      os << name << "(valobj) value=" << m_value << "\n";
    }

    fstring m_value;
  };

  class objref_element : public xml_element
  {
  public:
    objref_element(const char** attr, const char* eltype, const char* name,
                  shared_ptr<io::object_map> objmap) :
      m_type(""),
      m_id(0),
      m_objects(objmap)
    {
      const int id = atoi(find_attr(attr, "id", eltype, name, SRC_POS));

      if (id < 0)
        {
          invalid_attr("id", eltype, name, SRC_POS);
        }

      GVX_ASSERT(id >= 0);
      m_id = nub::uid(id);

      m_type = find_attr(attr, "type", eltype, name, SRC_POS);

      if (m_type.empty())
        {
          invalid_attr("type", eltype, name, SRC_POS);
        }

      GVX_ASSERT(!m_type.empty());
    }

    virtual ~objref_element() {}

    virtual void trace(std::ostream& os,
                       int depth, const char* name) const
    {
      for (int i = 0; i < depth; ++i) os << "  ";
      os << name << "(objref:" << m_type << ") id=" << m_id << "\n";
    }

    soft_ref<io::serializable> get_object()
    {
      if (m_id == 0)
        return soft_ref<io::serializable>();
      // else...
      return m_objects->get_existing_object(m_id);
    }

    fstring m_type;
    nub::uid m_id;
    shared_ptr<io::object_map> m_objects;
  };

  class group_element : public objref_element, public io::reader
  {
  public:
    group_element(const char** attr, const char* eltype, const char* name,
                 shared_ptr<io::object_map> objmap) :
      objref_element(attr, eltype, name, objmap),
      m_version(-1),
      m_elems()
    {
      m_version = atoi(find_attr(attr, "version", eltype, name, SRC_POS));

      if (m_version < 0)
        {
          invalid_attr("version", eltype, name, SRC_POS);
        }
      GVX_ASSERT(m_version >= 0);
    }

    virtual ~group_element() noexcept {}

    virtual void add_child(const char* name, el_ptr elp) override
    {
      m_elems[name] = elp;
    }

    virtual void trace(std::ostream& os,
                       int depth, const char* name) const override
    {
      for (int i = 0; i < depth; ++i) os << "  ";
      os << name << "(object:" << m_type << "):\n";
      for (map_type::const_iterator
             itr = m_elems.begin(),
             stop = m_elems.end();
           itr != stop;
           ++itr)
        {
          (*itr).second->trace(os, depth+1, (*itr).first.c_str());
        }
    }

    virtual io::version_id input_version_id() override
    {
      return m_version;
    }

    virtual char read_char(const fstring& /*name*/) override { GVX_ASSERT(0); return '\0'; }

    virtual int read_int(const fstring& name) override
    {
      el_ptr el = m_elems[name];
      int_element& ilp = element_cast<int_element>(el.get(), name, SRC_POS);
      return ilp.m_value;
    }

    virtual bool read_bool(const fstring& name) override
    {
      el_ptr el = m_elems[name];
      bool_element& blp = element_cast<bool_element>(el.get(), name, SRC_POS);
      return blp.m_value;
    }

    virtual double read_double(const fstring& name) override
    {
      el_ptr el = m_elems[name];
      double_element& dlp = element_cast<double_element>(el.get(), name, SRC_POS);
      return dlp.m_value;
    }

    virtual void read_value_obj(const fstring& name, rutz::value& v) override
    {
      el_ptr el = m_elems[name];
      value_element& vlp = element_cast<value_element>(el.get(), name, SRC_POS);
      v.set_string(vlp.m_value);
    }

    virtual void read_byte_array(const fstring& name, rutz::byte_array& data) override
    {
      default_read_byte_array(name, data);
    }

    virtual ref<io::serializable> read_object(const fstring& name) override
    {
      return read_weak_object(name);
    }

    virtual soft_ref<io::serializable> read_weak_object(const fstring& name) override;

    virtual void read_owned_object(const fstring& name,
                                 ref<io::serializable> obj) override
    {
      el_ptr el = m_elems[name];
      group_element& glp = element_cast<group_element>(el.get(), name, SRC_POS);
      glp.inflate(*obj);
    }

    virtual void read_base_class(const fstring& name,
                                 ref<io::serializable> base_part) override
    {
      el_ptr el = m_elems[name];
      group_element& glp = element_cast<group_element>(el.get(), name, SRC_POS);
      glp.inflate(*base_part);
    }

    virtual ref<io::serializable> read_root(io::serializable* /*root*/) override
    {
      GVX_ASSERT(0); return ref<io::serializable>(static_cast<io::serializable*>(0));
    }

    void inflate(io::serializable& obj)
    {
      obj.read_from(*this);
    }

  protected:
    virtual fstring read_string_impl(const fstring& name) override
    {
      el_ptr el = m_elems[name];
      string_element& slp = element_cast<string_element>(el.get(), name, SRC_POS);
      return fstring(slp.m_value.c_str());
    }

  public:
    int m_version;
    typedef std::map<fstring, el_ptr> map_type;
    map_type m_elems;
  };

  class object_element : public group_element
  {
  public:
    object_element(const char** attr, const char* name,
                  shared_ptr<io::object_map> objmap) :
      group_element(attr, "object", name, objmap)
    {
      // Return the object for this id, creating a new object if
      // necessary:
      m_object = objmap->fetch_object(m_type.c_str(), m_id);
    }

    soft_ref<io::serializable> m_object;

    virtual void finish()
    {
      if (m_object.is_valid())
        inflate(*m_object);
    }
  };

  soft_ref<io::serializable> group_element::read_weak_object(const fstring& name)
  {
    el_ptr el = m_elems[name];
    objref_element& olp = element_cast<objref_element>(el.get(), name, SRC_POS);
    return olp.get_object();
  }

  el_ptr make_element(const char* el, const char** attr,
                      const char* name,
                      shared_ptr<io::object_map> objmap)
  {
    if (strcmp(el, "object") == 0)
      {
        return el_ptr(new object_element(attr, name, objmap));
      }
    else if (strcmp(el, "ownedobj") == 0)
      {
        return el_ptr(new group_element(attr, "ownedobj", name, objmap));
      }
    else if (strcmp(el, "baseclass") == 0)
      {
        return el_ptr(new group_element(attr, "baseclass", name, objmap));
      }
    else if (strcmp(el, "objref") == 0)
      {
        return el_ptr(new objref_element(attr, "objref", name, objmap));
      }
    else if (strcmp(el, "string") == 0)
      {
        return el_ptr(new string_element);
      }
    else
      {
        const char* val = find_attr(attr, "value", el, name, SRC_POS);

        if (strcmp(el, "double") == 0)
          {
            return el_ptr(new double_element(val, name));
          }
        else if (strcmp(el, "int") == 0)
          {
            return el_ptr(new int_element(val, name));
          }
        else if (strcmp(el, "bool") == 0)
          {
            return el_ptr(new bool_element(val, name));
          }
        else if (strcmp(el, "valobj") == 0)
          {
            return el_ptr(new value_element(val, name));
          }
        else
          {
            throw rutz::error(rutz::sfmt("unknown element type: %s", el),
                              SRC_POS);
          }
      }
  }

  class tree_builder : public io::xml_parser
  {
  public:
    tree_builder(std::istream& is) :
      io::xml_parser(is),
      m_root(),
      m_stack(),
      m_depth(0),
      m_start_count(0),
      m_end_count(0),
      m_objects(new io::object_map),
      m_el_count(0)
    {}

    virtual ~tree_builder() {}

    object_element& get_root() const
    {
      if (m_root.get() == nullptr)
        {
          throw rutz::error("no root element found", SRC_POS);
        }

      return element_cast<object_element>(m_root.get(), "root", SRC_POS);
    }

  protected:
    virtual void element_start(const char* el, const char** attr) override;
    virtual void element_end(const char* el) override;
    virtual void character_data(const char* text, size_t length) override;

  private:
    el_ptr m_root;
    std::vector<el_ptr> m_stack;
    int m_depth;
    int m_start_count;
    int m_end_count;
    shared_ptr<io::object_map> m_objects;
    int m_el_count;
  };

  void tree_builder::element_start(const char* el, const char** attr)
  {
    ++m_el_count;
    ++m_start_count;
    ++m_depth;

    const char* name = find_attr(attr, "name", el, "(noname)", SRC_POS);

    GVX_ASSERT(name != nullptr);

    el_ptr elp = make_element(el, attr, name, m_objects);

    if (m_stack.size() > 0)
      m_stack.back()->add_child(name, elp);
    else
      m_root = elp;

    m_stack.push_back(elp);

    if (GVX_DBG_LEVEL() >= 3)
      {
        dbg_eval(3, m_el_count);
        dbg_eval(3, el);
        dbg_eval(3, name);
        dbg_eval(3, elp.get());
        dbg_eval_nl(3, m_stack.size());
      }
  }

  void tree_builder::element_end(const char* /*el*/)
  {
    --m_end_count;
    GVX_ASSERT(m_stack.size() > 0);
    GVX_ASSERT(m_stack.back().get() != nullptr);
    m_stack.back()->finish();
    m_stack.pop_back();
    --m_depth;
  }

  void tree_builder::character_data(const char* text, size_t length)
  {
    GVX_ASSERT(m_stack.size() > 0);
    GVX_ASSERT(m_stack.back().get() != nullptr);
    m_stack.back()->character_data(text, length);
  }

} // end anonymous namespace


nub::ref<io::serializable> io::load_gvx(const char* filename)
{
GVX_TRACE("io::load_gvx");
  shared_ptr<std::istream> ifs(rutz::igzopen(filename));
  tree_builder x(*ifs);
  x.parse();

  object_element& root = x.get_root();
  return root.m_object;
}

void io::xml_debug(const char* filename)
{
GVX_TRACE("io::xml_debug");
  shared_ptr<std::istream> ifs(rutz::igzopen(filename));
  tree_builder x(*ifs);
  x.parse();

  object_element& root = x.get_root();
  root.trace(std::cout, 0, "root");
}
