///////////////////////////////////////////////////////////////////////
//
// tcllistobj.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jul 11 12:00:17 2001
// written: Thu Jul 12 12:22:15 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLLISTOBJ_H_DEFINED
#define TCLLISTOBJ_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CONVERT_H_DEFINED)
#include "tcl/convert.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLOBJPTR_H_DEFINED)
#include "tcl/tclobjptr.h"
#endif

namespace Tcl
{
  class List;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::List class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::List {
public:
  /// Default constructor makes an empty list
  List();

  List(Tcl_Obj* listObj);

  List(const List& other) :
    itsList(other.itsList),
    itsElements(other.itsElements),
    itsLength(other.itsLength)
  {}

  List& operator=(const List& other)
  {
    itsList = other.itsList;
    itsElements = other.itsElements;
    itsLength = other.itsLength;
    return *this;
  }

  Tcl::ObjPtr asObj() const { return itsList; }

  /// Checked access to element at \a index.
  Tcl_Obj* at(unsigned int index) const;

  /// Unchecked access to element at \a index.
  Tcl_Obj* operator[](unsigned int index) const
    {
      update(); return itsElements[index];
    }

  template <class T>
  T get(unsigned int index) const
    {
      return Tcl::fromTcl<T>(at(index));
    }

  unsigned int size() const { update(); return itsLength; }
  unsigned int length() const { update(); return itsLength; }

  template <class T>
  void append(T t) { doAppend(Tcl::toTcl<T>(t), 1); }

  template <class T>
  void append(T t, unsigned int times) { doAppend(Tcl::toTcl<T>(t), times); }

  class IteratorBase;
  template <class T> class Iterator;

  template <class T>
  Iterator<T> begin(T* /*dummy*/=0);

  template <class T>
  Iterator<T> end(T* /*dummy*/=0);

  /// Utility function to return the list length of a Tcl object
  static unsigned int getLength(Tcl_Obj* obj);

private:
  void doAppend(Tcl_Obj* obj, unsigned int times);

  void update() const
    {
      if (itsElements==0)
        split();
    }

  void split() const;

  void invalidate() { itsElements = 0; itsLength = 0; }

  mutable Tcl::ObjPtr itsList;
  mutable Tcl_Obj** itsElements;
  mutable unsigned int itsLength;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::List::IteratorBase class definition
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::List::IteratorBase {
public:
  typedef int difference_type;

  enum Pos { BEGIN, END };

  IteratorBase(const List& owner, Pos pos = BEGIN) :
    itsList(owner),
    itsIndex(pos == BEGIN ? 0 : owner.length())
  {}

  IteratorBase(Tcl_Obj* listObj, Pos pos = BEGIN) :
    itsList(listObj),
    itsIndex(pos == BEGIN ? 0 : itsList.length())
  {}

  // default copy-constructor, assignment operator OK

  IteratorBase& operator++()
    { ++itsIndex; return *this; }

  IteratorBase operator++(int)
    { IteratorBase temp(*this); ++itsIndex; return temp; }

  difference_type operator-(const IteratorBase& other) const
    {
      if (this->itsIndex > other.itsIndex)
        return int(this->itsIndex - other.itsIndex);
      else
        return -(int(other.itsIndex - this->itsIndex));
    }

  bool operator==(const IteratorBase& other) const
    { return itsIndex == other.itsIndex; }

  bool operator!=(const IteratorBase& other) const
    { return !operator==(other); }

  bool isValid() const
    { return itsIndex < itsList.length(); }

  bool hasMore() const
    { return itsIndex < (itsList.length()-1); }

  bool nelems() const
    { return itsList.length(); }

protected:
  Tcl_Obj* current() const
    { return itsList.at(itsIndex); }

private:
  List itsList;
  unsigned int itsIndex;

};


///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Tcl::List::Iterator is an adapter that provides an STL-style
 * iterator interface to Tcl list objects. \c Tcl::List::Iterator is a
 * model of \c input \c iterator.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class Tcl::List::Iterator : public Tcl::List::IteratorBase {
public:
  Iterator(const List& owner, Pos pos = BEGIN) :
    IteratorBase(owner, pos) {}

  Iterator(Tcl_Obj* listObj, Pos pos = BEGIN) :
    IteratorBase(listObj, pos) {}

  typedef T value_type;

  T operator*() const
  {
    return Tcl::fromTcl<T>(current());
  }
};


///////////////////////////////////////////////////////////////////////
//
// Inline member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
inline Tcl::List::Iterator<T> Tcl::List::begin(T* /*dummy*/)
{
  return Iterator<T>(*this, IteratorBase::BEGIN);
}

template <class T>
inline Tcl::List::Iterator<T> Tcl::List::end(T* /*dummy*/)
{
  return Iterator<T>(*this, IteratorBase::END);
}

static const char vcid_tcllistobj_h[] = "$Header$";
#endif // !TCLLISTOBJ_H_DEFINED
