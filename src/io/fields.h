///////////////////////////////////////////////////////////////////////
//
// fields.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Nov 11 15:25:00 2000
// written: Sat Aug 18 08:07:18 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDS_H_DEFINED
#define FIELDS_H_DEFINED

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

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VALUEx_H_DEFINED)
#include "util/value.h"
#endif

#include <utility>

namespace IO
{
  class Reader;
  class Writer;
}

class FieldContainer;

template <class T>
class BoundsChecker {
private:
  BoundsChecker(const T& min, const T& max) : itsMin(min), itsMax(max) {}

public:
  const T itsMin;
  const T itsMax;

  static shared_ptr<BoundsChecker<T> > make(const T& min, const T& max)
  {
    return shared_ptr<BoundsChecker<T> >(new BoundsChecker<T>(min, max));
  }

  T limit(const T& raw)
  {
    return Util::max(itsMin, Util::min(itsMax, raw));
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


class FieldImpl {
public:
  virtual ~FieldImpl();

  virtual void set(FieldContainer* obj, const Value& new_val) const = 0;
  virtual shared_ptr<Value> get(const FieldContainer* obj) const = 0;

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
class DataMemberFieldImpl : public FieldImpl {
public:

  typedef Deref<T>::Type DerefT;

  DataMemberFieldImpl(T C::* memptr) : itsDataMember(memptr), itsChecker(0) {}

  DataMemberFieldImpl(T C::* memptr, shared_ptr<BoundsChecker<DerefT> > checker) :
    itsDataMember(memptr), itsChecker(checker) {}

  virtual void set(FieldContainer* obj, const Value& new_val) const
  {
    C& cobj = dynamic_cast<C&>(*obj);

    DerefT raw = new_val.get(Util::TypeCue<DerefT>());

    dereference(cobj, itsDataMember) =
      itsChecker.get() == 0 ? raw : itsChecker->limit(raw);
  }

  virtual shared_ptr<Value> get(const FieldContainer* obj) const
  {
    const C& cobj = dynamic_cast<const C&>(*obj);

    return shared_ptr<Value>(new TValue<DerefT>
                             (dereference(cobj, itsDataMember)));
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
class ValueFieldImpl : public FieldImpl {
public:

  ValueFieldImpl(V C::* memptr) : itsValueMember(memptr) {}

  virtual void set(FieldContainer* obj, const Value& new_val) const
  {
    C& cobj = dynamic_cast<C&>(*obj);

    dereference(cobj, itsValueMember).assignFrom(new_val);
  }

  virtual shared_ptr<Value> get(const FieldContainer* obj) const
  {
    const C& cobj = dynamic_cast<const C&>(*obj);

    return shared_ptr<Value>(dereference(cobj, itsValueMember).clone());
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
class FuncMemberFieldImpl : public FieldImpl {
  typedef T (C::* Getter)() const;
  typedef void (C::* Setter)(T);

  Getter itsGetter;
  Setter itsSetter;

  FuncMemberFieldImpl& operator=(const FuncMemberFieldImpl&);
  FuncMemberFieldImpl(const FuncMemberFieldImpl&);

public:
  FuncMemberFieldImpl(Getter g, Setter s) : itsGetter(g), itsSetter(s) {}

  virtual void set(FieldContainer* obj, const Value& new_val) const
  {
    C& cobj = dynamic_cast<C&>(*obj);

    (cobj.*itsSetter)(new_val.get(Util::TypeCue<T>()));
  }

  virtual shared_ptr<Value> get(const FieldContainer* obj) const
  {
    const C& cobj = dynamic_cast<const C&>(*obj);

    return shared_ptr<Value>(new TValue<T>((cobj.*itsGetter)()));
  }

  virtual void readValueFrom(FieldContainer* obj,
                             IO::Reader* reader, const fstring& name) const
  {
    C& cobj = dynamic_cast<C&>(*obj);

    T temp;
    reader->readValue(name, temp);
    (cobj.*itsSetter)(temp);
  }

  virtual void writeValueTo(const FieldContainer* obj,
                            IO::Writer* writer, const fstring& name) const
  {
    const C& cobj = dynamic_cast<const C&>(*obj);

    writer->writeValue(name.c_str(), (cobj.*itsGetter)());
  }
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * Field. We make one info object per property per class. This
 * class has no virtuals... it can be allocated in a builtin array.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Field {
private:
  const fstring itsName;
  shared_ptr<FieldImpl> itsFieldImpl;
  shared_ptr<Value> itsDefaultValue;
  shared_ptr<Value> itsMin;
  shared_ptr<Value> itsMax;
  shared_ptr<Value> itsRes;
  bool itsStartsNewGroup;

public:

  struct BoundsCheck {};

  struct ValueType {};

  template <class C, class T>
  static shared_ptr<FieldImpl> makeImpl(T C::* member_ptr)
  {
    return shared_ptr<FieldImpl>(new DataMemberFieldImpl<C,T>(member_ptr));
  }

  template <class C, class T>
  static shared_ptr<FieldImpl> makeImpl(T C::* member_ptr,
                                        const T& min, const T& max)
  {
    return shared_ptr<FieldImpl>
      (new DataMemberFieldImpl<C,T>
       (member_ptr, BoundsChecker<Deref<T>::Type>::make(min, max)));
  }

  template <class C, class T>
  static shared_ptr<FieldImpl>
  makeImpl(std::pair<T (C::*)() const, void (C::*)(T)> funcs)
  {
    return shared_ptr<FieldImpl>
      (new FuncMemberFieldImpl<C,T>(funcs.first, funcs.second));
  }

  static shared_ptr<FieldImpl> makeImpl(shared_ptr<FieldImpl> ptr)
  {
    return ptr;
  }

  template <class T, class C, class V>
  Field(const fstring& name, ValueType, V C::* value_ptr,
        const T& def, const T& min, const T& max, const T& res,
        bool new_group=false) :
    itsName(name),
    itsFieldImpl(new ValueFieldImpl<C,V>(value_ptr)),
    itsDefaultValue(new TValue<T>(def)),
    itsMin(new TValue<T>(min)),
    itsMax(new TValue<T>(max)),
    itsRes(new TValue<T>(res)),
    itsStartsNewGroup(new_group)
  {}

  template <class T, class M>
  Field(const fstring& name, M member_ptr_init,
        const T& def, const T& min, const T& max, const T& res,
        bool new_group=false) :
    itsName(name),
    itsFieldImpl(makeImpl(member_ptr_init)),
    itsDefaultValue(new TValue<T>(def)),
    itsMin(new TValue<T>(min)),
    itsMax(new TValue<T>(max)),
    itsRes(new TValue<T>(res)),
    itsStartsNewGroup(new_group)
  {}

  template <class T, class M>
  Field(const fstring& name, BoundsCheck, M member_ptr_init,
        const T& def, const T& min, const T& max, const T& res,
        bool new_group=false) :
    itsName(name),
    itsFieldImpl(makeImpl(member_ptr_init, min, max)),
    itsDefaultValue(new TValue<T>(def)),
    itsMin(new TValue<T>(min)),
    itsMax(new TValue<T>(max)),
    itsRes(new TValue<T>(res)),
    itsStartsNewGroup(new_group)
  {}

  const fstring& name() const { return itsName; }
  const Value& defaultValue() const { return *itsDefaultValue; }

  const Value& min() const { return *itsMin; }
  const Value& max() const { return *itsMax; }
  const Value& res() const { return *itsRes; }

  bool startsNewGroup() const { return itsStartsNewGroup; }

  void setValue(FieldContainer* obj, const Value& new_val) const
  {
    itsFieldImpl->set(obj, new_val);
  }

  shared_ptr<Value> getValue(const FieldContainer* obj) const
  {
    return itsFieldImpl->get(obj);
  }

  void readValueFrom(FieldContainer* obj,
                     IO::Reader* reader, const fstring& name) const
  {
    itsFieldImpl->readValueFrom(obj, reader, name);
  }

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

class FieldMap {
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

  class ItrImpl;

  // Iterates over the Field's in alphabetical order by name
  class Iterator {
  private:
    ItrImpl* const itsImpl;
  public:
    Iterator(ItrImpl* impl) : itsImpl(impl) {}
    Iterator(const Iterator& other);
    Iterator& operator=(const Iterator& other);
    ~Iterator();

    const Field& operator*() const;
    const Field* operator->() const;
    Iterator& operator++();
    bool operator==(const Iterator& other);
    bool operator!=(const Iterator& other)
      { return !(operator==(other)); }
  };

  Iterator begin() const;
  Iterator end() const;

  // Iterates over the Field's in the order they were declared in
  // the sequence that was passed to the FieldMap constructor.
  typedef const Field* IoIterator;
  IoIterator ioBegin() const;
  IoIterator ioEnd() const;
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
  Util::Observable* itsObservable;

  FieldContainer(const FieldContainer&);
  FieldContainer& operator=(const FieldContainer&);

public:
  FieldContainer(Util::Observable* obs);
  virtual ~FieldContainer();

  void setFieldMap(const FieldMap& fields);

  void setField(const fstring& name, const Value& new_val);
  void setField(const Field& field, const Value& new_val);

  shared_ptr<Value> getField(const fstring& name) const;
  shared_ptr<Value> getField(const Field& field) const;

  void readFieldsFrom(IO::Reader* reader, const FieldMap& fields);
  void writeFieldsTo(IO::Writer* writer, const FieldMap& fields) const;

  const FieldMap& fields() const { return *itsFieldMap; }
};


static const char vcid_fields_h[] = "$Header$";
#endif // !FIELDS_H_DEFINED
