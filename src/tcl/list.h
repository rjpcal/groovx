///////////////////////////////////////////////////////////////////////
//
// tcllistobj.h
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jul 11 12:00:17 2001
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_TCL_LIST_H_UTC20050628162420_DEFINED
#define GROOVX_TCL_LIST_H_UTC20050628162420_DEFINED

#include "tcl/obj.h"

#include "rutz/sharedptr.h"

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

class Tcl::List
{
public:
  /// Default constructor makes an empty list
  List();

  List(const Tcl::Obj& listObj);

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

  Tcl::Obj asObj() const { return itsList; }

  /// Checked access to element at \a index.
  Tcl_Obj* at(unsigned int index) const;

  /// Unchecked access to element at \a index.
  Tcl_Obj* operator[](unsigned int index) const
    {
      update(); return itsElements[index];
    }

  Tcl_Obj* const* elements() const
    {
      update(); return itsElements;
    }

  template <class T>
  inline T get(unsigned int index, T* /*dummy*/=0) const;

  unsigned int size() const { update(); return itsLength; }
  unsigned int length() const { update(); return itsLength; }

  template <class T>
  void append(T t) { doAppend(Tcl::toTcl(t), 1); }

  template <class T>
  void append(T t, unsigned int times)
    {
      doAppend(Tcl::toTcl(t), times);
    }

  template <class Itr>
  void appendRange(Itr itr, Itr end)
    {
      while (itr != end)
        {
          append(*itr);
          ++itr;
        }
    }

  class IteratorBase;
  template <class T> class Iterator;

  template <class T>
  Iterator<T> begin(T* /*dummy*/=0);

  template <class T>
  Iterator<T> end(T* /*dummy*/=0);

  /// A back-insert iterator for Tcl::List.
  class Appender
  {
    Tcl::List& itsList;
  public:
    Appender(Tcl::List& aList) : itsList(aList) {}

    template <class T>
    Appender& operator=(const T& val)
    { itsList.append(val); return *this; }

    Appender& operator*() { return *this; }
    Appender& operator++() { return *this; }
    Appender operator++(int) { return *this; }
  };

  Appender appender() { return Appender(*this); }

  /// Utility function to return the list length of a Tcl object
  static unsigned int getLength(Tcl_Obj* obj);

private:
  void doAppend(const Tcl::Obj& obj, unsigned int times);

  void update() const
    {
      if (itsElements==0)
        split();
    }

  void split() const;

  void invalidate() { itsElements = 0; itsLength = 0; }

  mutable Tcl::Obj itsList;
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

class Tcl::List::IteratorBase
{
protected:
  // Make protected to prevent people from instantiating IteratorBase directly.
  ~IteratorBase() {}

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
class Tcl::List::Iterator : public Tcl::List::IteratorBase
{
  // Keep a copy of the current value here so that operator*() can
  // return a reference rather than by value.
  mutable rutz::shared_ptr<const T> itsCurrent;

public:
  Iterator(const List& owner, Pos pos = BEGIN) :
    IteratorBase(owner, pos), itsCurrent() {}

  Iterator(Tcl_Obj* listObj, Pos pos = BEGIN) :
    IteratorBase(listObj, pos), itsCurrent() {}

  typedef T value_type;

  const T& operator*() const
  {
    itsCurrent.reset(new T(Tcl::toNative<T>(current())));
    return *itsCurrent;
  }
};


///////////////////////////////////////////////////////////////////////
//
// Inline member definitions
//
///////////////////////////////////////////////////////////////////////

#include "tcl/conversions.h"

template <class T>
inline T Tcl::List::get(unsigned int index, T* /*dummy*/) const
{
  return Tcl::toNative<T>(at(index));
}

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

static const char vcid_groovx_tcl_list_h_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_LIST_H_UTC20050628162420_DEFINED
