///////////////////////////////////////////////////////////////////////
//
// fields.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Nov 11 15:25:00 2000
// written: Mon Sep  3 13:54:32 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDS_H_DEFINED
#define FIELDS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLOBJPTR_H_DEFINED)
#include "tcl/tclobjptr.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLVALUE_H_DEFINED)
#include "tcl/tclvalue.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ALGO_H_DEFINED)
#include "util/algo.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(POINTERS_H_DEFINED)
#include "util/pointers.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H_DEFINED)
#include "util/strings.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VALUE_H_DEFINED)
#include "util/value.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(UTILITY_DEFINED)
#include <utility>
#define UTILITY_DEFINED
#endif

namespace IO
{
  class Reader;
  class Writer;
}

class FieldContainer;

namespace Util
{
  template <class T> class FwdIter;
  class Signal;
}

template <class T>
class BoundsChecker
{
private:
  BoundsChecker(const T& min, const T& max) : itsMin(min), itsMax(max) {}

public:
  const T itsMin;
  const T itsMax;

  static shared_ptr<BoundsChecker<T> > make(const T& min, const T& max,
                                            bool check)
  {
    return check ?
      shared_ptr<BoundsChecker<T> >(new BoundsChecker<T>(min, max)) :
      shared_ptr<BoundsChecker<T> >(0);
  }

  T limit(const T& raw)
  {
    return Util::clamp(raw, itsMin, itsMax);
  }
};

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
protected:
  static void throwNotAllowed(const char* what);

public:
  virtual ~FieldImpl();

  virtual void set(FieldContainer* obj, Tcl::ObjPtr& new_val) const = 0;
  virtual Tcl::ObjPtr get(const FieldContainer* obj) const = 0;

  virtual void readValueFrom(FieldContainer* obj,
                             IO::Reader* reader, const fstring& name) const = 0;
  virtual void writeValueTo(const FieldContainer* obj,
                            IO::Writer* writer, const fstring& name) const = 0;
};

namespace
{
  template <class C, class T>
  T& dereference(C& obj, T C::* memptr)
  {
    return (obj.*memptr);
  }

  template <class C, class T>
  const T& dereference(const C& obj, T C::* memptr)
  {
    return (obj.*memptr);
  }

  template <class C, class T>
  T& dereference(C& obj, T* C::* memptr)
  {
    return *(obj.*memptr);
  }

  template <class C, class T>
  const T& dereference(const C& obj, T* C::* memptr)
  {
    return *(obj.*memptr);
  }

  template <class T>
  struct Deref
  {
    typedef T Type;
  };

  template <class T>
  struct Deref<T*>
  {
    typedef T Type;
  };
}



/** DataMemberFieldImpl */
template <class C, class T>
class DataMemberFieldImpl : public FieldImpl
{
public:

  typedef Deref<T>::Type DerefT;

  DataMemberFieldImpl(T C::* memptr) : itsDataMember(memptr), itsChecker(0) {}

  DataMemberFieldImpl(T C::* memptr, shared_ptr<BoundsChecker<DerefT> > checker) :
    itsDataMember(memptr), itsChecker(checker) {}

  virtual void set(FieldContainer* obj, Tcl::ObjPtr& new_val) const
  {
    C& cobj = dynamic_cast<C&>(*obj);

    DerefT raw = new_val.as(Util::TypeCue<DerefT>());

    dereference(cobj, itsDataMember) =
      itsChecker.get() == 0 ? raw : itsChecker->limit(raw);
  }

  virtual Tcl::ObjPtr get(const FieldContainer* obj) const
  {
    const C& cobj = dynamic_cast<const C&>(*obj);

    return Tcl::Convert<DerefT>::toTcl(dereference(cobj, itsDataMember));
  }

  virtual void readValueFrom(FieldContainer* obj,
                             IO::Reader* reader, const fstring& name) const
  {
    C& cobj = dynamic_cast<C&>(*obj);

    reader->readValue(name, dereference(cobj, itsDataMember));
  }

  virtual void writeValueTo(const FieldContainer* obj,
                            IO::Writer* writer, const fstring& name) const
  {
    const C& cobj = dynamic_cast<const C&>(*obj);

    writer->writeValue(name.c_str(), dereference(cobj, itsDataMember));
  }

private:
  DataMemberFieldImpl& operator=(const DataMemberFieldImpl&);
  DataMemberFieldImpl(const DataMemberFieldImpl&);

  T C::* itsDataMember;
  shared_ptr<BoundsChecker<DerefT> > itsChecker;
};

/** ValueFieldImpl */
template <class C, class V>
class ValueFieldImpl : public FieldImpl
{
public:
  ValueFieldImpl(V C::* memptr) : itsValueMember(memptr) {}

  virtual void set(FieldContainer* obj, Tcl::ObjPtr& new_val) const
  {
    C& cobj = dynamic_cast<C&>(*obj);

    Tcl::TclValue tval(new_val);

    dereference(cobj, itsValueMember).assignFrom(tval);
  }

  virtual Tcl::ObjPtr get(const FieldContainer* obj) const
  {
    const C& cobj = dynamic_cast<const C&>(*obj);

    const Value& val = dereference(cobj, itsValueMember);

    return Tcl::Convert<const Value&>::toTcl(val);
  }

  virtual void readValueFrom(FieldContainer* obj,
                             IO::Reader* reader, const fstring& name) const
  {
    C& cobj = dynamic_cast<C&>(*obj);

    reader->readValueObj(name, dereference(cobj, itsValueMember));
  }

  virtual void writeValueTo(const FieldContainer* obj,
                            IO::Writer* writer, const fstring& name) const
  {
    const C& cobj = dynamic_cast<const C&>(*obj);

    writer->writeValueObj(name.c_str(), dereference(cobj, itsValueMember));
  }

private:
  ValueFieldImpl& operator=(const ValueFieldImpl&);
  ValueFieldImpl(const ValueFieldImpl&);

  V C::* itsValueMember;
};

/** FuncMemberFieldImpl */
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
  FuncMemberFieldImpl(Getter g, Setter s) : itsGetter(g), itsSetter(s) {}

  virtual void set(FieldContainer* obj, Tcl::ObjPtr& new_val) const
  {
    if (itsSetter == 0) throwNotAllowed("set");

    C& cobj = dynamic_cast<C&>(*obj);

    (cobj.*itsSetter)(new_val.as(Util::TypeCue<T>()));
  }

  virtual Tcl::ObjPtr get(const FieldContainer* obj) const
  {
    if (itsGetter == 0) throwNotAllowed("get");

    const C& cobj = dynamic_cast<const C&>(*obj);

    return Tcl::Convert<T>::toTcl((cobj.*itsGetter)());
  }

  virtual void readValueFrom(FieldContainer* obj,
                             IO::Reader* reader, const fstring& name) const
  {
    if (itsSetter == 0) throwNotAllowed("read");

    C& cobj = dynamic_cast<C&>(*obj);

    T temp;
    reader->readValue(name, temp);
    (cobj.*itsSetter)(temp);
  }

  virtual void writeValueTo(const FieldContainer* obj,
                            IO::Writer* writer, const fstring& name) const
  {
    if (itsGetter == 0) throwNotAllowed("write");

    const C& cobj = dynamic_cast<const C&>(*obj);

    writer->writeValue(name.c_str(), (cobj.*itsGetter)());
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

  static const unsigned int NEW_GROUP = 1 << 0;
  static const unsigned int TRANSIENT = 1 << 1; // otherwise, persistent
  static const unsigned int STRING    = 1 << 2; // otherwise, numeric
  static const unsigned int MULTI     = 1 << 3; // otherwise, single-valued
  static const unsigned int CHECKED   = 1 << 4; // otherwise, unchecked
  static const unsigned int NO_GET    = 1 << 5; // otherwise, allow get
  static const unsigned int NO_SET    = 1 << 6; // otherwise, allow set

  unsigned int flags() const    { return itsFlags; }

  bool startsNewGroup() const   { return   itsFlags & NEW_GROUP; }

  bool isTransient() const      { return   itsFlags & TRANSIENT ; }
  bool isPersistent() const     { return !(itsFlags & TRANSIENT); }

  bool isString() const         { return   itsFlags & STRING ; }
  bool isNumeric() const        { return !(itsFlags & STRING); }

  bool isMultiValued() const    { return   itsFlags & MULTI ; }
  bool isSingleValued() const   { return !(itsFlags & MULTI); }

  bool isChecked() const        { return   itsFlags & CHECKED ; }
  bool isUnchecked() const      { return !(itsFlags & CHECKED); }

  bool allowGet() const         { return !(itsFlags & NO_GET); }
  bool allowSet() const         { return !(itsFlags & NO_SET); }

  struct ValueType {};

  /**
   * Make a FieldImpl from a member-pointer-to-data
   **/

  template <class C, class T>
  static shared_ptr<FieldImpl>
  makeImpl(T C::* member_ptr,
           const Deref<T>::Type& min, const Deref<T>::Type& max, bool check)
  {
    return shared_ptr<FieldImpl>
      (new DataMemberFieldImpl<C,T>
       (member_ptr, BoundsChecker<Deref<T>::Type>::make(min, max, check)));
  }

  /**
   * Make a FieldImpl from a getter/setter pair of
   * member-pointer-to-functions
   **/

  template <class C, class T>
  static shared_ptr<FieldImpl>
  makeImpl(std::pair<T (C::*)() const, void (C::*)(T)> funcs,
           const T& /* min */, const T& /* max */, bool /* check */)
  {
    return shared_ptr<FieldImpl>
      (new FuncMemberFieldImpl<C,T>(funcs.first, funcs.second));
  }

  /**
   * Make a read-only FieldImpl from a getter member-pointer-to-function
   **/

  template <class C, class T>
  static shared_ptr<FieldImpl>
  makeImpl(T (C::* getter)() const,
           const T& /* min */, const T& /* max */, bool /* check */)
  {
    return shared_ptr<FieldImpl>
      (new FuncMemberFieldImpl<C,T>(getter, 0));
  }

  /**
   * Make a write-only FieldImpl from a setter member-pointer-to-function
   **/

  template <class C, class T>
  static shared_ptr<FieldImpl>
  makeImpl(void (C::* setter)(T),
           const T& /* min */, const T& /* max */, bool /* check */)
  {
    return shared_ptr<FieldImpl>
      (new FuncMemberFieldImpl<C,T>(0, setter));
  }

  static shared_ptr<FieldImpl> makeImpl(shared_ptr<FieldImpl> ptr)
  {
    return ptr;
  }

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

  const fstring& name() const { return itsName; }
  const fstring& defaultValue() const { return itsDefaultValue; }

  const fstring& min() const { return itsMin; }
  const fstring& max() const { return itsMax; }
  const fstring& res() const { return itsRes; }

  /// Set the value of this field for \a obj.
  void setValue(FieldContainer* obj, Tcl::ObjPtr& new_val) const
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
                     IO::Reader* reader, const fstring& name) const
  {
    itsFieldImpl->readValueFrom(obj, reader, name);
  }

  /// Write this field for \a obj to \a writer.
  void writeValueTo(const FieldContainer* obj,
                    IO::Writer* writer, const fstring& name) const
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
  Impl* itsImpl;

  FieldMap(const FieldMap&);
  FieldMap& operator=(const FieldMap&);

  void init(const Field* begin, const Field* end,
            const FieldMap* parent);

public:
  FieldMap(const Field* begin, const Field* end,
           const FieldMap* parent=0) :
    itsImpl(0)
  {
    init(begin, end, parent);
  }

  template<std::size_t N>
  FieldMap(Field const (&array)[N], const FieldMap* parent=0) :
    itsImpl(0)
  {
    init(&array[0], &array[0]+N, parent);
  }

  ~FieldMap();

  static const FieldMap* emptyFieldMap();

  bool hasParent() const;
  const FieldMap* parent() const;

  const Field& field(const fstring& name) const;

  typedef Util::FwdIter<const Field> Iterator;

  // Iterates over the Field's in alphabetical order by name
  Iterator alphaFields() const;

  // Iterates over the Field's in the order they were listed in
  // the sequence that was passed to the FieldMap constructor.
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
  Util::Signal* itsSignal;

  FieldContainer(const FieldContainer&);
  FieldContainer& operator=(const FieldContainer&);

public:
  FieldContainer(Util::Signal* sig);
  virtual ~FieldContainer();

  void setFieldMap(const FieldMap& fields);

  Tcl::ObjPtr getField(const fstring& name) const;
  Tcl::ObjPtr getField(const Field& field) const;

  void setField(const fstring& name, Tcl::ObjPtr& new_val);
  void setField(const Field& field, Tcl::ObjPtr& new_val);

  void readFieldsFrom(IO::Reader* reader, const FieldMap& fields);
  void writeFieldsTo(IO::Writer* writer, const FieldMap& fields) const;

  const FieldMap& fields() const { return *itsFieldMap; }
};


static const char vcid_fields_h[] = "$Header$";
#endif // !FIELDS_H_DEFINED
