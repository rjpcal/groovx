///////////////////////////////////////////////////////////////////////
//
// fields.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Nov 11 15:25:00 2000
// written: Tue Aug 14 18:57:34 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDS_H_DEFINED
#define FIELDS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBSERVABLE_H_DEFINED)
#include "util/observable.h"
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

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

class Field;
class FieldInfo;
class FieldContainer;

///////////////////////////////////////////////////////////////////////
/**
 *
 * FieldMemberPtr is used to encapsulate a pointer-to-member of a
 * FieldContainer in a typesafe way.
 *
 **/
///////////////////////////////////////////////////////////////////////


class FieldMemberPtr {
public:
  virtual ~FieldMemberPtr();

private:
  virtual Field& dereference(FieldContainer* obj) const = 0;

  friend class FieldInfo;
};

/** CFieldMemberPtr  */
template <class C, class T>
class CFieldMemberPtr : public FieldMemberPtr {
private:
  T C::* itsPtr;

  virtual Field& dereference(FieldContainer* obj) const;

public:
  CFieldMemberPtr(T C::* ptr) : itsPtr(ptr) {}

  CFieldMemberPtr(const CFieldMemberPtr& other) : itsPtr(other.itsPtr) {}

  CFieldMemberPtr& operator=(const CFieldMemberPtr& other)
    { itsPtr = other.itsPtr; return *this; }
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * FieldInfo. We make one info object per property per class. This
 * class has no virtuals... it can be allocated in a builtin array.
 *
 **/
///////////////////////////////////////////////////////////////////////

class FieldInfo {
private:
  const fstring itsName;
  shared_ptr<FieldMemberPtr> itsMemberPtr;
  shared_ptr<Value> itsDefaultValue;
  shared_ptr<Value> itsMin;
  shared_ptr<Value> itsMax;
  shared_ptr<Value> itsRes;
  bool itsStartsNewGroup;

public:
  template <class C, class T, class F>
  FieldInfo(const fstring& name, F C::* member_ptr,
            const T& def, const T& min_, const T& max_, const T& res_,
            bool new_group=false) :
    itsName(name),
    itsMemberPtr(new CFieldMemberPtr<C,F>(member_ptr)),
    itsDefaultValue(new TValue<T>(def)),
    itsMin(new TValue<T>(min_)),
    itsMax(new TValue<T>(max_)),
    itsRes(new TValue<T>(res_)),
    itsStartsNewGroup(new_group)
    {}

  const fstring& name() const { return itsName; }
  const Value& defaultValue() const { return *itsDefaultValue; }

  const Value& min() const { return *itsMin; }
  const Value& max() const { return *itsMax; }
  const Value& res() const { return *itsRes; }

  bool startsNewGroup() const { return itsStartsNewGroup; }

private:
  friend class FieldContainer;

  Field& dereference(FieldContainer* obj) const
    { return itsMemberPtr->dereference(obj); }
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * Field base class. Value's provide an interface between the untyped
 * world of Tcl, and the strongly typed world of C++. The abstract
 * Value interface provides operations to get() basic value types from
 * the object. Field's are intended to be members of C++ classes, and
 * expose value()/setValue() functions that take or return
 * Value's. C++ classes can inherit from FieldContainer to maintain a
 * list of FieldInfo's that act as pointer-to-members for the
 * contained Field's. This FieldMap is then sufficient for a template
 * to be able to generate all the necessary Tcl::TclCmd's to expose a
 * class's fields to Tcl. A C++ class may actually store Value
 * subclasses that provide efficient access to their native type to
 * clients (such as the owner of the Value) who know the real type of
 * the Value. For example, a simple template TValue stores an object
 * of a basic type, provides access to that object via get/setNative()
 * procedures, but also implements the Value interface. Any call to a
 * get() function of the wrong type will throw a run-time exception.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Field {
private:
  Field& operator=(const Field& other);

protected:
  virtual void doSetValue(const Value& new_val) = 0;

public:
  Field();

  Field(const Field&) {}

  virtual ~Field();

  virtual void readValueFrom(IO::Reader* reader, const fstring& name) = 0;
  virtual void writeValueTo(IO::Writer* writer, const fstring& name) const = 0;

  virtual shared_ptr<Value> value() const = 0;
  void setValue(const Value& new_val, FieldContainer& owner);
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * TField template implementation of Field.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class TField : public Field {
protected:
  virtual void doSetValue(const Value& new_val);

  TField(const TField&);        // not implemented
  TField& operator=(const TField&); // not implemented

public:
  TField(const T& val = T());
  virtual ~TField();

  virtual void readValueFrom(IO::Reader* reader, const fstring& name);
  virtual void writeValueTo(IO::Writer* writer, const fstring& name) const;

  virtual shared_ptr<Value> value() const;

  void setNative(T new_val) { itsVal = new_val; }

  const T& operator()() const { return itsVal; }

private:
  T itsVal;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * TBoundedField template implementation of Field.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class TBoundedField : public Field {
protected:
  virtual void doSetValue(const Value& new_val);

  TBoundedField(const TBoundedField&); // not implemented
  TBoundedField& operator=(const TBoundedField&); // not implemented

public:
  TBoundedField(const T& val, const T& min, const T& max);

  virtual ~TBoundedField();

  virtual void readValueFrom(IO::Reader* reader, const fstring& name);
  virtual void writeValueTo(IO::Writer* writer, const fstring& name) const;

  virtual shared_ptr<Value> value() const;

  const T& operator()() const { return itsVal; }

private:
  T itsVal;
  const T itsMin;
  const T itsMax;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * TPtrField template implementation of Field.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class TPtrField : public Field {
protected:
  virtual void doSetValue(const Value& new_val);

  TPtrField(const TPtrField&);  // not implemented
  TPtrField& operator=(const TPtrField&); // not implemented

public:
  TPtrField(T& valRef);
  virtual ~TPtrField();

  virtual void readValueFrom(IO::Reader* reader, const fstring& name);
  virtual void writeValueTo(IO::Writer* writer, const fstring& name) const;

  void reseat(T& valRef) { itsVal.reseat(valRef); }

  virtual shared_ptr<Value> value() const;

  const T& operator()() const { return itsVal(); }

private:
  TValuePtr<T> itsVal;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * FieldMap is an associative container that returns a FieldInfo given
 * the correspnding name. We make one statically allocated property
 * map per class. This is a concrete class with no virtual functions.
 *
 **/
///////////////////////////////////////////////////////////////////////

class FieldMap {
private:
  class Impl;
  Impl* const itsImpl;

  FieldMap(const FieldMap&);
  FieldMap& operator=(const FieldMap&);

public:
  FieldMap(const FieldInfo* begin, const FieldInfo* end,
           const FieldMap* parent=0);
  ~FieldMap();

  static const FieldMap* emptyFieldMap();

  bool hasParent() const;
  const FieldMap* parent() const;

  const FieldInfo& info(const fstring& name) const;

  class ItrImpl;

  // Iterates over the FieldInfo's in alphabetical order by name
  class Iterator {
  private:
    ItrImpl* const itsImpl;
  public:
    Iterator(ItrImpl* impl) : itsImpl(impl) {}
    Iterator(const Iterator& other);
    Iterator& operator=(const Iterator& other);
    ~Iterator();

    const FieldInfo& operator*() const;
    const FieldInfo* operator->() const;
    Iterator& operator++();
    bool operator==(const Iterator& other);
    bool operator!=(const Iterator& other)
      { return !(operator==(other)); }
  };

  Iterator begin() const;
  Iterator end() const;

  // Iterates over the FieldInfo's in the order they were declared in
  // the sequence that was passed to the FieldMap constructor.
  typedef const FieldInfo* IoIterator;
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

class FieldContainer : public virtual IO::IoObject,
                       public virtual Util::Observable
{
private:
  const FieldMap* itsFieldMap;

  FieldContainer(const FieldContainer&);
  FieldContainer& operator=(const FieldContainer&);

public:
  FieldContainer();
  virtual ~FieldContainer();

  void setFieldMap(const FieldMap& fields);

  void setField(const fstring& name, const Value& new_val);
  void setField(const FieldInfo& pinfo, const Value& new_val);

  shared_ptr<Value> getField(const fstring& name) const;
  shared_ptr<Value> getField(const FieldInfo& pinfo) const;

  void readFieldsFrom(IO::Reader* reader, const FieldMap& fields);
  void writeFieldsTo(IO::Writer* writer, const FieldMap& fields) const;

  const FieldMap& fields() const { return *itsFieldMap; }
};


///////////////////////////////////////////////////////////////////////
//
// out-of-line method implementations
//
///////////////////////////////////////////////////////////////////////

template <class C, class T>
Field& CFieldMemberPtr<C, T>::dereference(FieldContainer* obj) const
{
  C& p = dynamic_cast<C&>(*obj);
  return p.*itsPtr;
}

static const char vcid_fields_h[] = "$Header$";
#endif // !FIELDS_H_DEFINED
