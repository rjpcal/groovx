///////////////////////////////////////////////////////////////////////
//
// fields.h
//
// Copyright (c) 2000-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sat Nov 11 15:25:00 2000
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

#ifndef FIELDS_H_DEFINED
#define FIELDS_H_DEFINED

#include "tcl/tclobjptr.h"

#include "util/algo.h"
#include "util/object.h"
#include "util/pointers.h"
#include "util/stderror.h"
#include "util/strings.h"
#include "util/traits.h"
#include "util/value.h"

namespace IO
{
  class Reader;
  class Writer;
}

class FieldContainer;

namespace Util
{
  template <class T> class FwdIter;
  class Signal0;
}

/// A template class for limiting values within a numeric range
template <class T>
class BoundsChecker
{
public:
  /// The type returned when the field is dereferenced.
  typedef typename Util::TypeTraits<T>::DerefT DerefT;

private:
  BoundsChecker(const DerefT& min, const DerefT& max) :
    itsMin(min), itsMax(max) {}

public:
  const DerefT itsMin;  ///< lower bound
  const DerefT itsMax;  ///< upper bound

  /// Factory function.
  static shared_ptr<BoundsChecker<T> >
  make(const DerefT& min, const DerefT& max, bool check)
  {
    return check ?
      shared_ptr<BoundsChecker<T> >(new BoundsChecker<T>(min, max)) :
      shared_ptr<BoundsChecker<T> >(0);
  }

  /// Bounds-limit the input value.
  DerefT limit(const DerefT& raw)
  {
    return Util::clamp(raw, itsMin, itsMax);
  }
};

// Have to use this local replacement for std::pair because #include'ing
// <utility> brings in std::rel_ops, which, with g++-2.96 on Mac OS X, match
// too greedily and mess up template instantiations with classes that already
// have their own relational operators.

namespace
{
  template <class T1, class T2>
  struct mypair
  {
    // Can't pass these by const reference or else Mac OS X g++-3.1 bugs.
    mypair(T1 t1, T2 t2) : first(t1), second(t2) {}

    template <class U1, class U2>
    mypair(const mypair<U1, U2>& p) : first(p.first), second(p.second) {}

    T1 first;
    T2 second;
  };

  // Can't pass these by const reference or else Mac OS X g++-3.1 bugs.
  template <class T1, class T2>
  inline mypair<T1, T2> make_mypair(T1 t1, T2 t2)
  {
    return mypair<T1,T2>(t1, t2);
  }
}

namespace FieldAux
{
  /// A drop-in replacement for dynamic_cast for FieldContainers.
  /** The difference from dynamic_cast is that this cast function allows us
      to "cast" to a FieldContainer's subobject as returned by its child()
      function. */
  template <class C, class F>
  inline C& cast(F& p);

  void throwNotAllowed(const char* what);
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

  /// Change the value of the given object's referred-to field.
  virtual void set(FieldContainer* obj, const Tcl::ObjPtr& new_val) const = 0;
  /// Get the value of the given object's referred-to field.
  virtual Tcl::ObjPtr get(const FieldContainer* obj) const = 0;

  /// Read the value of the given object's referred-to field from the IO::Reader.
  virtual void readValueFrom(FieldContainer* obj,
                             IO::Reader& reader, const fstring& name) const = 0;
  /// Write the value of the given object's referred-to field to the IO::Writer.
  virtual void writeValueTo(const FieldContainer* obj,
                            IO::Writer& writer, const fstring& name) const = 0;
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
}



/// DataMemberFieldImpl.
template <class C, class T>
class DataMemberFieldImpl : public FieldImpl
{
public:

  /// Type returned when the field is dereferenced.
  typedef typename Util::TypeTraits<T>::DerefT DerefT;

  /// Construct with a pointer-to-data-member.
  DataMemberFieldImpl(T C::* memptr) : itsDataMember(memptr), itsChecker(0) {}

  /// Construct with a pointer-to-data-member and a bounds checker.
  DataMemberFieldImpl(T C::* memptr, shared_ptr<BoundsChecker<T> > checker) :
    itsDataMember(memptr), itsChecker(checker) {}

  /// Change the value of the given object's pointed-to data member.
  virtual void set(FieldContainer* obj, const Tcl::ObjPtr& new_val) const
  {
    C& cobj = FieldAux::cast<C>(*obj);

    DerefT raw = new_val.template as<DerefT>();

    dereference(cobj, itsDataMember) =
      itsChecker.get() == 0 ? raw : itsChecker->limit(raw);
  }

  /// Get the value of the given object's pointed-to data member.
  virtual Tcl::ObjPtr get(const FieldContainer* obj) const
  {
    const C& cobj = FieldAux::cast<const C>(*obj);

    return Tcl::toTcl(const_dereference(cobj, itsDataMember));
  }

  /// Read the value of the given object's pointed-to data member.
  virtual void readValueFrom(FieldContainer* obj,
                             IO::Reader& reader, const fstring& name) const
  {
    C& cobj = FieldAux::cast<C>(*obj);

    reader.readValue(name, dereference(cobj, itsDataMember));
  }

  /// Write the value of the given object's pointed-to data member.
  virtual void writeValueTo(const FieldContainer* obj,
                            IO::Writer& writer, const fstring& name) const
  {
    const C& cobj = FieldAux::cast<const C>(*obj);

    writer.writeValue(name.c_str(), const_dereference(cobj, itsDataMember));
  }

private:
  DataMemberFieldImpl& operator=(const DataMemberFieldImpl&);
  DataMemberFieldImpl(const DataMemberFieldImpl&);

  T C::* itsDataMember;
  shared_ptr<BoundsChecker<T> > itsChecker;
};

/// ValueFieldImpl
template <class C, class V>
class ValueFieldImpl : public FieldImpl
{
public:
  /// Construct with a member pointer for the Value field.
  ValueFieldImpl(V C::* memptr) : itsValueMember(memptr) {}

  virtual void set(FieldContainer* obj, const Tcl::ObjPtr& new_val) const
  {
    C& cobj = FieldAux::cast<C>(*obj);

    fstring sval = new_val.template as<fstring>();

    dereference(cobj, itsValueMember).setFstring(sval);
  }

  virtual Tcl::ObjPtr get(const FieldContainer* obj) const
  {
    const C& cobj = FieldAux::cast<const C>(*obj);

    const Value& val = const_dereference(cobj, itsValueMember);

    return Tcl::toTcl(val);
  }

  virtual void readValueFrom(FieldContainer* obj,
                             IO::Reader& reader, const fstring& name) const
  {
    C& cobj = FieldAux::cast<C>(*obj);

    reader.readValueObj(name, dereference(cobj, itsValueMember));
  }

  virtual void writeValueTo(const FieldContainer* obj,
                            IO::Writer& writer, const fstring& name) const
  {
    const C& cobj = FieldAux::cast<const C>(*obj);

    writer.writeValueObj(name.c_str(), const_dereference(cobj, itsValueMember));
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

  virtual void set(FieldContainer* obj, const Tcl::ObjPtr& new_val) const
  {
    if (itsSetter == 0) FieldAux::throwNotAllowed("set");

    C& cobj = FieldAux::cast<C>(*obj);

    typedef typename Util::TypeTraits<T>::StackT StackT;

    (cobj.*itsSetter)(new_val.template as<StackT>());
  }

  virtual Tcl::ObjPtr get(const FieldContainer* obj) const
  {
    if (itsGetter == 0) FieldAux::throwNotAllowed("get");

    const C& cobj = FieldAux::cast<const C>(*obj);

    return Tcl::toTcl((cobj.*itsGetter)());
  }

  virtual void readValueFrom(FieldContainer* obj,
                             IO::Reader& reader, const fstring& name) const
  {
    if (itsSetter == 0) FieldAux::throwNotAllowed("read");

    C& cobj = FieldAux::cast<C>(*obj);

    typedef typename Util::TypeTraits<T>::StackT StackT;

    StackT temp;
    reader.readValue(name, temp);
    (cobj.*itsSetter)(temp);
  }

  virtual void writeValueTo(const FieldContainer* obj,
                            IO::Writer& writer, const fstring& name) const
  {
    if (itsGetter == 0) FieldAux::throwNotAllowed("write");

    const C& cobj = FieldAux::cast<const C>(*obj);

    writer.writeValue(name.c_str(), (cobj.*itsGetter)());
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
  const fstring itsName;
  shared_ptr<FieldImpl> itsFieldImpl;
  const fstring itsDefaultValue;
  const fstring itsMin;
  const fstring itsMax;
  const fstring itsRes;
  unsigned int itsFlags;

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

  /**
   * Make a FieldImpl from a member-pointer-to-data
   **/

  template <class C, class T, class T2>
  static shared_ptr<FieldImpl>
  makeImpl(T C::* member_ptr, const T2& min, const T2& max, bool check)
  {
    return shared_ptr<FieldImpl>
      (new DataMemberFieldImpl<C,T>
       (member_ptr, BoundsChecker<T>::make(min, max, check)));
  }

  /**
   * Make a FieldImpl from a getter/setter pair of
   * member-pointer-to-functions
   **/

  template <class C, class T, class T2>
  static shared_ptr<FieldImpl>
  makeImpl(mypair<T (C::*)() const, void (C::*)(T)> funcs,
           const T2& /* min */, const T2& /* max */, bool /* check */)
  {
    return shared_ptr<FieldImpl>
      (new FuncMemberFieldImpl<C,T>(funcs.first, funcs.second));
  }

  /**
   * Make a read-only FieldImpl from a getter member-pointer-to-function
   **/

  template <class C, class T, class T2>
  static shared_ptr<FieldImpl>
  makeImpl(T (C::* getter)() const,
           const T2& /* min */, const T2& /* max */, bool /* check */)
  {
    return shared_ptr<FieldImpl>
      (new FuncMemberFieldImpl<C,T>(getter, 0));
  }

  /**
   * Make a write-only FieldImpl from a setter member-pointer-to-function
   **/

  template <class C, class T, class T2>
  static shared_ptr<FieldImpl>
  makeImpl(void (C::* setter)(T),
           const T2& /* min */, const T2& /* max */, bool /* check */)
  {
    return shared_ptr<FieldImpl>
      (new FuncMemberFieldImpl<C,T>(0, setter));
  }

  /// The trivial factory function for when we already have a FieldImpl.
  static shared_ptr<FieldImpl> makeImpl(shared_ptr<FieldImpl> ptr)
  {
    return ptr;
  }

  /// Construct using a ValueFieldImpl.
  template <class C, class V>
  Field(const fstring& name, ValueType, V C::* value_ptr,
        const fstring& def, const fstring& min,
        const fstring& max, const fstring& res,
        unsigned int flags=0) :
    itsName(name),
    itsFieldImpl(new ValueFieldImpl<C,V>(value_ptr)),
    itsDefaultValue(def),
    itsMin(min),
    itsMax(max),
    itsRes(res),
    itsFlags(flags)
  {}

  /// Construct using a DataMemberFieldImpl.
  template <class T, class M>
  Field(const fstring& name, M member_ptr_init,
        const T& def, const T& min, const T& max, const T& res,
        unsigned int flags=0) :
    itsName(name),
    itsFieldImpl(makeImpl(member_ptr_init, min, max, (flags & CHECKED))),
    itsDefaultValue(def),
    itsMin(min),
    itsMax(max),
    itsRes(res),
    itsFlags(flags)
  {}

  /// Get the Field's name.
  const fstring& name() const { return itsName; }
  /// Get the Field's default value.
  const fstring& defaultValue() const { return itsDefaultValue; }

  /// Get the Field's minimum value (only for numeric fields).
  const fstring& min() const { return itsMin; }
  /// Get the Field's maximum value (only for numeric fields).
  const fstring& max() const { return itsMax; }
  /// Get the Field's value quantization step (only for numeric fields).
  const fstring& res() const { return itsRes; }

  /// Set the value of this field for \a obj.
  void setValue(FieldContainer* obj, const Tcl::ObjPtr& new_val) const
  {
    itsFieldImpl->set(obj, new_val);
  }

  /// Get the value of this field for \a obj.
  Tcl::ObjPtr getValue(const FieldContainer* obj) const
  {
    return itsFieldImpl->get(obj);
  }

  /// Read this field for \a obj from \a reader.
  void readValueFrom(FieldContainer* obj,
                     IO::Reader& reader, const fstring& name) const
  {
    itsFieldImpl->readValueFrom(obj, reader, name);
  }

  /// Write this field for \a obj to \a writer.
  void writeValueTo(const FieldContainer* obj,
                    IO::Writer& writer, const fstring& name) const
  {
    itsFieldImpl->writeValueTo(obj, writer, name);
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
  const Field& field(const fstring& name) const;

  /// Iterator type
  typedef Util::FwdIter<const Field> Iterator;

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

class FieldContainer : public virtual Util::Object
{
private:
  const FieldMap* itsFieldMap;
  Util::Signal0* itsSignal;

  FieldContainer(const FieldContainer&);
  FieldContainer& operator=(const FieldContainer&);

public:
  /// Construct with an optional signal to be triggered when a field changes.
  FieldContainer(Util::Signal0* sig);

  /// Virtual destructor.
  virtual ~FieldContainer() throw();

  /// Set the field map for this object.
  void setFieldMap(const FieldMap& fields);

  /// Get the value associated with the named field.
  Tcl::ObjPtr getField(const fstring& name) const;
  /// Get the value associated with the given field.
  Tcl::ObjPtr getField(const Field& field) const;

  /// Set the value associated with the named field.
  void setField(const fstring& name, const Tcl::ObjPtr& new_val);
  /// Set the value associated with the given field.
  void setField(const Field& field, const Tcl::ObjPtr& new_val);

  /// Read all fields from the IO::Reader.
  void readFieldsFrom(IO::Reader& reader, const FieldMap& fields);
  /// Write all fields to the IO::Writer.
  void writeFieldsTo(IO::Writer& writer, const FieldMap& fields) const;

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
        Util::throwBadCast(typeid(C), typeid(F));
    }

  return *result;
}

static const char vcid_fields_h[] = "$Header$";
#endif // !FIELDS_H_DEFINED
