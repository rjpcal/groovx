///////////////////////////////////////////////////////////////////////
//
// fields.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Nov 11 15:25:00 2000
// written: Tue Nov 14 08:06:38 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDS_H_DEFINED
#define FIELDS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBSERVABLE_H_DEFINED)
#include "util/observable.h"
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
  virtual Field& dereference(FieldContainer* obj) const = 0;
};

/** CFieldMemberPtr  */
template <class C, class T>
class CFieldMemberPtr : public FieldMemberPtr {
private:
  T C::* itsPtr;

public:
  CFieldMemberPtr(T C::* ptr) : itsPtr(ptr) {}

  virtual Field& dereference(FieldContainer* obj) const;
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
  const fixed_string itsName;
  FieldMemberPtr* itsMemberPtr;
  Value* itsDefaultValue;
  Value* itsMin;
  Value* itsMax;
  Value* itsRes;
  bool itsStartsNewGroup;

public:
  template <class C, class T, class F>
  FieldInfo(const fixed_string& name, F C::* member_ptr,
				const T& def, const T& min_, const T& max_, const T& res_,
				bool new_group=true) :
	 itsName(name),
	 itsMemberPtr(new CFieldMemberPtr<C,F>(member_ptr)),
	 itsDefaultValue(new TValue<T>(def)),
	 itsMin(new TValue<T>(min_)),
	 itsMax(new TValue<T>(max_)),
	 itsRes(new TValue<T>(res_)),
	 itsStartsNewGroup(new_group)
	 {}

  const fixed_string& name() const { return itsName; }
  const Value& defaultValue() const { return *itsDefaultValue; }

  const Value& min() const { return *itsMin; }
  const Value& max() const { return *itsMax; }
  const Value& res() const { return *itsRes; }

  bool startsNewGroup() const { return itsStartsNewGroup; }

  Field& dereference(FieldContainer* obj) const
	 { return itsMemberPtr->dereference(obj); }
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * Field base class.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Field {
private:
  FieldContainer* itsOwner;

protected:
  virtual void doSetValue(const Value& new_val) = 0;

public:
  Field(FieldContainer* owner);
  virtual ~Field();

  virtual const Value& value() const = 0;
  void setValue(const Value& new_val);
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

public:
  TField(FieldContainer* owner, const T& val = T());
  virtual ~TField();

  virtual const Value& value() const;

  void setNative(T new_val) { itsVal.itsVal = new_val; }
  T getNative() const { return itsVal.itsVal; }  

  T& operator()() { return itsVal.itsVal; }
  const T& operator()() const { return itsVal.itsVal; }

  TValue<T> itsVal;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * TBoundedField template implementation of Field.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T, int min, int max, int div>
class TBoundedField : public TField<T> {
protected:
  virtual void doSetValue(const Value& new_val)
	 {
		T temp; new_val.get(temp);
		setNative(temp);
	 }

public:
  TBoundedField(FieldContainer* owner, const T& val = T()) :
	 TField<T>(owner, val) {}

  void setNative(T new_val)
	 {
		if (new_val >= min/T(div) && new_val <= max/T(div))
		  TField<T>::itsVal.itsVal = new_val;
	 }
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

public:
  TPtrField(FieldContainer* owner, T& valRef);
  virtual ~TPtrField();

  void reseat(T& valRef) { itsVal.reseat(valRef); }

  virtual const Value& value() const;

  ///
  void setNative(T new_val) { itsVal() = new_val; }
  ///
  T getNative() const { return itsVal(); }

  ///
  T& operator()() { return itsVal(); }
  ///
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

public:
  FieldMap(const FieldInfo* begin, const FieldInfo* end);
  ~FieldMap();

  const FieldInfo& info(const fixed_string& name) const;

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
							  public virtual Observable
{
private:
  const FieldMap& itsFieldMap;

  FieldContainer(const FieldContainer&);
  FieldContainer& operator=(const FieldContainer&);

public:
  FieldContainer(const FieldMap& pmap);
  virtual ~FieldContainer();

  Field& field(const fixed_string& name);
  Field& field(const FieldInfo& pinfo);

  const Field& field(const fixed_string& name) const;
  const Field& field(const FieldInfo& pinfo) const;

  void readFieldsFrom(IO::Reader* reader);
  void writeFieldsTo(IO::Writer* writer) const;

  const FieldMap& fields() const { return itsFieldMap; }
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
