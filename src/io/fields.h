/** @file io/fields.h FieldContainer allows serializable objects to
    have "fields", or named object properties */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat Nov 11 15:25:00 2000
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_IO_FIELDS_H_UTC20051109000918_DEFINED
#define GROOVX_IO_FIELDS_H_UTC20051109000918_DEFINED

#include "io/reader.h"
#include "io/writer.h"

#include "nub/object.h"

#include "rutz/algo.h"
#include "rutz/fileposition.h"
#include "rutz/fstring.h"
#include "rutz/shared_ptr.h"
#include "rutz/stderror.h"
#include "rutz/traits.h"
#include "rutz/value.h"

#include <limits>

namespace rutz
{
  template <class T> class fwd_iter;
}

namespace io
{
  class reader;
  class writer;
}

namespace nub
{
  class signal0;
}

class FieldContainer;

namespace FieldAux
{
  /// A drop-in replacement for dynamic_cast for FieldContainers.
  /** The difference from dynamic_cast is that this cast function allows us
      to "cast" to a FieldContainer's subobject as returned by its child()
      function. */
  template <class C, class F>
  inline C& cast(F& p);

  void throwNotAllowed(const char* what, const rutz::file_pos& pos);
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * FieldImpl is used to encapsulate a pointer-to-member of a
 * FieldContainer in a typesafe way.
 *
 **/
///////////////////////////////////////////////////////////////////////

class FieldImpl
{
public:
  /// Virtual destructor.
  virtual ~FieldImpl();

  /// Read the value of the given object's referred-to field from the io::reader.
  virtual void readValueFrom(FieldContainer* obj,
                             io::reader& reader,
                             const rutz::fstring& name) const = 0;
  /// Write the value of the given object's referred-to field to the io::writer.
  virtual void writeValueTo(const FieldContainer* obj,
                            io::writer& writer,
                            const rutz::fstring& name) const = 0;
};

namespace
{
  template <class C, class T>
  struct MatchConst
  {
    typedef T Type;
  };

  template <class C, class T>
  struct MatchConst<const C, T>
  {
    typedef const T Type;
  };

  template <class C, class T>
  struct Deref
  {
    typedef typename MatchConst<C,T>::Type Type;

    static Type& get(C& obj, T C::* mptr) { return (obj.*mptr); }
  };

  template <class C, class T>
  struct Deref<C, T*>
  {
    typedef typename MatchConst<C,T>::Type Type;

    static Type& get(C& obj, T* C::* mptr) { return *(obj.*mptr); }
  };

  template <class C, class T>
  typename Deref<C,T>::Type&
  dereference(C& obj, T C::* mptr)
  {
    return Deref<C,T>::get(obj, mptr);
  }

  template <class C, class T>
  typename Deref<const C,T>::Type&
  const_dereference(const C& obj, T C::* mptr)
  {
    return Deref<const C,T>::get(obj, mptr);
  }

  template <class PM>
  struct PmTraits;

  template <class C, class T>
  struct PmTraits<T C::*>
  {
    typedef C ClassType;
    typedef T MemType;
  };
}



/// DataMemberFieldImpl.
template <class PM>
class DataMemberFieldImpl : public FieldImpl
{
public:
  // PM is a pointer-to-member-type of the form "T C::*"
  typedef typename PmTraits<PM>::ClassType C;
  typedef typename PmTraits<PM>::MemType T;

  /// Type returned when the field is dereferenced.
  typedef typename rutz::type_traits<T>::deref_t deref_t;

  /// Construct with a pointer-to-data-member.
  DataMemberFieldImpl(T C::* memptr) : itsDataMember(memptr) {}

  static rutz::shared_ptr<FieldImpl>
  make(T C::* memptr)
  {
    return rutz::shared_ptr<FieldImpl> (new DataMemberFieldImpl(memptr));
  }

  /// Read the value of the given object's pointed-to data member.
  virtual void readValueFrom(FieldContainer* obj,
                             io::reader& reader,
                             const rutz::fstring& name) const
  {
    C& cobj = FieldAux::cast<C>(*obj);

    reader.read_value(name, dereference(cobj, itsDataMember));
  }

  /// Write the value of the given object's pointed-to data member.
  virtual void writeValueTo(const FieldContainer* obj,
                            io::writer& writer,
                            const rutz::fstring& name) const
  {
    const C& cobj = FieldAux::cast<const C>(*obj);

    writer.write_value(name.c_str(), const_dereference(cobj, itsDataMember));
  }

private:
  DataMemberFieldImpl& operator=(const DataMemberFieldImpl&);
  DataMemberFieldImpl(const DataMemberFieldImpl&);

  T C::* itsDataMember;
};

/// CheckedDataMemberFieldImpl.
template <class PM>
class CheckedDataMemberFieldImpl : public FieldImpl
{
public:
  // PM is a pointer-to-member-type of the form "T C::*"
  typedef typename PmTraits<PM>::ClassType C;
  typedef typename PmTraits<PM>::MemType T;

  /// Type returned when the field is dereferenced.
  typedef typename rutz::type_traits<T>::deref_t deref_t;

  /// Construct with a pointer-to-data-member and a bounds checker.
  CheckedDataMemberFieldImpl(T C::* memptr,
                             const deref_t& min,
                             const deref_t& max) :
    itsDataMember(memptr),
    itsMin(min),
    itsMax(max)
  {}

  static rutz::shared_ptr<FieldImpl>
  make(T C::* memptr, const deref_t& min, const deref_t& max)
  {
    return rutz::shared_ptr<FieldImpl>
      (new CheckedDataMemberFieldImpl(memptr, min, max));
  }

  /// Read the value of the given object's pointed-to data member.
  virtual void readValueFrom(FieldContainer* obj,
                             io::reader& reader,
                             const rutz::fstring& name) const
  {
    C& cobj = FieldAux::cast<C>(*obj);

    deref_t temp;

    reader.read_value(name, temp);

    dereference(cobj, itsDataMember) = this->limit(temp);
  }

  /// Write the value of the given object's pointed-to data member.
  virtual void writeValueTo(const FieldContainer* obj,
                            io::writer& writer,
                            const rutz::fstring& name) const
  {
    const C& cobj = FieldAux::cast<const C>(*obj);

    writer.write_value(name.c_str(), const_dereference(cobj, itsDataMember));
  }

private:
  CheckedDataMemberFieldImpl(const CheckedDataMemberFieldImpl&);
  CheckedDataMemberFieldImpl& operator=(const CheckedDataMemberFieldImpl&);

  deref_t limit(const deref_t& raw) const
  {
    return rutz::clamp(raw, itsMin, itsMax);
  }

  T C::* itsDataMember;
  const deref_t itsMin;
  const deref_t itsMax;
};

/// ValueFieldImpl
template <class C, class V>
class ValueFieldImpl : public FieldImpl
{
public:
  /// Construct with a member pointer for the rutz::value field.
  ValueFieldImpl(V C::* memptr) : itsValueMember(memptr) {}

  virtual void readValueFrom(FieldContainer* obj,
                             io::reader& reader,
                             const rutz::fstring& name) const
  {
    C& cobj = FieldAux::cast<C>(*obj);

    reader.read_value_obj(name, dereference(cobj, itsValueMember));
  }

  virtual void writeValueTo(const FieldContainer* obj,
                            io::writer& writer,
                            const rutz::fstring& name) const
  {
    const C& cobj = FieldAux::cast<const C>(*obj);

    writer.write_value_obj(name.c_str(), const_dereference(cobj, itsValueMember));
  }

private:
  ValueFieldImpl& operator=(const ValueFieldImpl&);
  ValueFieldImpl(const ValueFieldImpl&);

  V C::* itsValueMember;
};

/// FuncMemberFieldImpl
template <class C, class T>
class FuncMemberFieldImpl : public FieldImpl
{
  typedef T (C::* Getter)() const;
  typedef void (C::* Setter)(T);

  Getter itsGetter;
  Setter itsSetter;

  FuncMemberFieldImpl& operator=(const FuncMemberFieldImpl&);
  FuncMemberFieldImpl(const FuncMemberFieldImpl&);

public:
  /// Construct with pointers to getter and setter member functions.
  FuncMemberFieldImpl(Getter g, Setter s) : itsGetter(g), itsSetter(s) {}

  virtual void readValueFrom(FieldContainer* obj,
                             io::reader& reader,
                             const rutz::fstring& name) const
  {
    if (itsSetter == 0) FieldAux::throwNotAllowed("read", SRC_POS);

    C& cobj = FieldAux::cast<C>(*obj);

    typedef typename rutz::type_traits<T>::stack_t stack_t;

    stack_t temp;
    reader.read_value(name, temp);
    (cobj.*itsSetter)(temp);
  }

  virtual void writeValueTo(const FieldContainer* obj,
                            io::writer& writer,
                            const rutz::fstring& name) const
  {
    if (itsGetter == 0) FieldAux::throwNotAllowed("write", SRC_POS);

    const C& cobj = FieldAux::cast<const C>(*obj);

    writer.write_value(name.c_str(), (cobj.*itsGetter)());
  }
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * Field. We make one Field object per property per class. This class
 * has no virtuals... it can be allocated in a builtin array, which
 * can then be used to construct a FieldMap.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Field
{
private:
  const rutz::fstring               itsName;
  const rutz::shared_ptr<FieldImpl> itsFieldImpl;
  const rutz::fstring               itsDefaultValue;
  const rutz::fstring               itsMin;
  const rutz::fstring               itsMax;
  const rutz::fstring               itsRes;
  const unsigned int                itsFlags;
  io::version_id                     itsMinVersion;
  io::version_id                     itsMaxVersion;

public:

  //
  // Attribute flags
  //

  static const unsigned int NEW_GROUP = 1 << 0; ///< field begins a new group
  static const unsigned int TRANSIENT = 1 << 1; ///< otherwise, persistent
  static const unsigned int STRING    = 1 << 2; ///< otherwise, numeric
  static const unsigned int MULTI     = 1 << 3; ///< otherwise, single-valued
  static const unsigned int CHECKED   = 1 << 4; ///< otherwise, unchecked
  static const unsigned int NO_GET    = 1 << 5; ///< otherwise, allow get
  static const unsigned int NO_SET    = 1 << 6; ///< otherwise, allow set
  static const unsigned int PRIVATE   = 1 << 7; ///< otherwise, public
  static const unsigned int BOOLEAN   = 1 << 8; ///< field is a boolean value

  /// Get all flags or'ed together.
  unsigned int flags()    const { return itsFlags; }

  /// Query for presence of NEW_GROUP flag.
  bool startsNewGroup()   const { return   itsFlags & NEW_GROUP; }

  /// Query for presence of TRANSIENT flag.
  bool isTransient()      const { return   itsFlags & TRANSIENT ; }
  /// Query for absence of TRANSIENT flag.
  bool isPersistent()     const { return !(itsFlags & TRANSIENT); }

  /// Query for presence of STRING flag.
  bool isString()         const { return   itsFlags & STRING ; }
  /// Query for absence of STRING flag.
  bool isNumeric()        const { return !(itsFlags & STRING); }

  /// Query for presence of MULTI flag.
  bool isMultiValued()    const { return   itsFlags & MULTI ; }
  /// Query for absence of MULTI flag.
  bool isSingleValued()   const { return !(itsFlags & MULTI); }

  /// Query for presence of CHECKED flag.
  bool isChecked()        const { return   itsFlags & CHECKED ; }
  /// Query for absence of CHECKED flag.
  bool isUnchecked()      const { return !(itsFlags & CHECKED); }

  /// Query for absence of NO_GET flag.
  bool allowGet()         const { return !(itsFlags & NO_GET); }
  /// Query for absence of NO_SET flag.
  bool allowSet()         const { return !(itsFlags & NO_SET); }

  /// Query for presence of PRIVATE flag.
  bool isPrivate()        const { return   itsFlags & PRIVATE ; }
  /// Query for absence of PRIVATE flag.
  bool isPublic()         const { return !(itsFlags & PRIVATE); }

  /// Query for presence of BOOLEAN flag.
  bool isBoolean()        const { return   itsFlags & BOOLEAN; }

  /// Symbol class for use with Field's constructors.
  struct ValueType {};

  /// Construct using a ValueFieldImpl.
  template <class C, class V>
  Field(const rutz::fstring& name, ValueType, V C::* value_ptr,
        const rutz::fstring& def, const rutz::fstring& min,
        const rutz::fstring& max, const rutz::fstring& res,
        unsigned int flags=0) :
    itsName(name),
    itsFieldImpl(new ValueFieldImpl<C,V>(value_ptr)),
    itsDefaultValue(def),
    itsMin(min),
    itsMax(max),
    itsRes(res),
    itsFlags(flags),
    itsMinVersion(0),
    itsMaxVersion(std::numeric_limits<io::version_id>::max())
  {}

  /// Construct using a pointer-to-member-data.
  template <class T, class PM>
  Field(const rutz::fstring& name, PM memptr,
        const T& def, const T& min, const T& max, const T& res,
        unsigned int flags=0) :
    itsName(name),
    itsFieldImpl((flags & CHECKED)
                 ? CheckedDataMemberFieldImpl<PM>::make(memptr, min, max)
                 : DataMemberFieldImpl<PM>::make(memptr)
                 ),
    itsDefaultValue(rutz::cat(def)),
    itsMin(rutz::cat(min)),
    itsMax(rutz::cat(max)),
    itsRes(rutz::cat(res)),
    itsFlags(flags),
    itsMinVersion(0),
    itsMaxVersion(std::numeric_limits<io::version_id>::max())
  {}

  /// Construct using a getter/setter pair of pointers-to-member-functions.
  template <class C, class T>
  Field(const rutz::fstring& name,
        T (C::* getter)() const,
        void (C::* setter)(T),
        const T& def, const T& min, const T& max, const T& res,
        unsigned int flags=0) :
    itsName(name),
    itsFieldImpl(new FuncMemberFieldImpl<C,T>(getter, setter)),
    itsDefaultValue(rutz::cat(def)),
    itsMin(rutz::cat(min)),
    itsMax(rutz::cat(max)),
    itsRes(rutz::cat(res)),
    itsFlags(flags),
    itsMinVersion(0),
    itsMaxVersion(std::numeric_limits<io::version_id>::max())
  {}

  Field& versions(io::version_id lo,
                  io::version_id hi
                  = std::numeric_limits<io::version_id>::max())
  {
    itsMinVersion = lo;
    itsMaxVersion = hi;
    return *this;
  }

  /// Get the Field's name.
  const rutz::fstring& name() const { return itsName; }
  /// Get the Field's default value.
  const rutz::fstring& defaultValue() const { return itsDefaultValue; }

  /// Get the Field's minimum value (only for numeric fields).
  const rutz::fstring& min() const { return itsMin; }
  /// Get the Field's maximum value (only for numeric fields).
  const rutz::fstring& max() const { return itsMax; }
  /// Get the Field's value quantization step (only for numeric fields).
  const rutz::fstring& res() const { return itsRes; }

  /// Get the minimum serial version id for this field.
  io::version_id minVersion() const { return itsMinVersion; }
  /// Get the maximum serial version id for this field.
  io::version_id maxVersion() const { return itsMaxVersion; }

  bool shouldSerialize(io::version_id svid) const
  {
    return (svid >= itsMinVersion && svid <= itsMaxVersion);
  }

  /// Read this field for \a obj from \a reader.
  void readValueFrom(FieldContainer* obj,
                     io::reader& reader) const
  {
    itsFieldImpl->readValueFrom(obj, reader, itsName);
  }

  /// Write this field for \a obj to \a writer.
  void writeValueTo(const FieldContainer* obj,
                    io::writer& writer) const
  {
    itsFieldImpl->writeValueTo(obj, writer, itsName);
  }
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * FieldMap is an associative container that returns a Field given the
 * correspnding name. We make one statically allocated property map
 * per class. This is a concrete class with no virtual functions.
 *
 **/
///////////////////////////////////////////////////////////////////////

class FieldMap
{
private:
  class Impl;
  Impl* rep;

  FieldMap(const FieldMap&);
  FieldMap& operator=(const FieldMap&);

  void init(const Field* begin, const Field* end,
            const FieldMap* parent);

public:
  /// Construct from a sequence of Field objects.
  FieldMap(const Field* begin, const Field* end,
           const FieldMap* parent=0) :
    rep(0)
  {
    init(begin, end, parent);
  }

  /// Construct from an array of Field objects.
  template<unsigned int N>
  FieldMap(Field const (&array)[N], const FieldMap* parent=0) :
    rep(0)
  {
    init(&array[0], &array[0]+N, parent);
  }

  /// Destructor.
  ~FieldMap();

  /// Get a pointer to an empty FieldMap.
  static const FieldMap* emptyFieldMap();

  /// Query whether this FieldMap has a parent.
  bool hasParent() const throw();

  /// Get this FieldMap's parent (or null if non-existent).
  const FieldMap* parent() const throw();

  /// Look up the field associated with the given name.
  /** Parent FieldMap objects will be searched recursively. An exception
      will be thrown if the named field is not found. */
  const Field& field(const rutz::fstring& name) const;

  /// Iterator type
  typedef rutz::fwd_iter<const Field> Iterator;

  /// Get an iterator that runs over the Field's in alphabetical order by name
  Iterator alphaFields() const;

  /// Get an iterator that runs over the Field's in their "native" order.
  /** The native order is the order they were listed in the sequence that
      was passed to the FieldMap constructor. */
  Iterator ioFields() const;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * FieldContainer is the base class for all classes that contain
 * fields. Typically a derived class will need to set up a file-scope
 * FieldMap in the implementation file, and then pass that FieldMap to
 * the FieldContainer constructor.
 *
 **/
///////////////////////////////////////////////////////////////////////

class FieldContainer : public virtual nub::object
{
private:
  const FieldMap* itsFieldMap;
public:
  nub::signal0* itsSignal;
private:
  FieldContainer(const FieldContainer&);
  FieldContainer& operator=(const FieldContainer&);

public:
  /// Construct with an optional signal to be triggered when a field changes.
  FieldContainer(nub::signal0* sig);

  /// Virtual destructor.
  virtual ~FieldContainer() throw();

  /// Set the field map for this object.
  void setFieldMap(const FieldMap& fields);

  /// Lookup the Field for a given name.
  /** An exception will be thrown if no such Field is found. */
  const Field& field(const rutz::fstring& name) const;

  /// Emit a signal saying that one of our values has changed.
  void touch() const;

  /// Read all fields from the io::reader.
  void readFieldsFrom(io::reader& reader, const FieldMap& fields);
  /// Write all fields to the io::writer.
  void writeFieldsTo(io::writer& writer, const FieldMap& fields,
                     io::version_id svid) const;

  /// Get the FieldMap for this object.
  const FieldMap& fields() const { return *itsFieldMap; }

  /// Get this container's child, if any. Default implementation returns null.
  virtual FieldContainer* child() const;
};

template <class C, class F>
inline C& FieldAux::cast(F& p)
{
  C* result = dynamic_cast<C*>(&p);

  if (result == 0)
    {
      F* child = p.child();
      if (child != 0)
        result = &(FieldAux::cast<C>(*child));
      else
        rutz::throw_bad_cast(typeid(C), typeid(F), SRC_POS);
    }

  return *result;
}

static const char __attribute__((used)) vcid_groovx_io_fields_h_utc20051109000918[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_FIELDS_H_UTC20051109000918DEFINED
