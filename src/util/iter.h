///////////////////////////////////////////////////////////////////////
//
// iter.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Aug 17 11:05:24 2001
// written: Wed Mar 19 17:58:54 2003
// $Id$
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

#ifndef ITER_H_DEFINED
#define ITER_H_DEFINED

#include "util/pointers.h"

#include <utility>

namespace Util
{
  /// Symbol class for representing generic "end of iteration".
  struct IterEnd_t {};

  const IterEnd_t IterEnd = IterEnd_t();

  template <class T> class FwdIter;
  template <class T> class BidirIter;
  template <class T> class RxsIter;

  template <class T> class FwdIterIfx;
  template <class T> class BidirIterIfx;
  template <class T> class RxsIterIfx;

  template <class Iter, class T> class FwdIterAdapter;
  template <class Iter, class T> class BidirIterAdapter;
  template <class Iter, class T> class RxsIterAdapter;

  template <class T, class Ifx> class ConcreteIter;

  template <class T>
  inline T& getref(T& t) { return t; }

  template <class T1, class T2>
  inline T2& getref(std::pair<T1, T2>& p) { return p.second; }

///////////////////////////////////////////////////////////////////////
//
// ConcreteIter class
//
///////////////////////////////////////////////////////////////////////


/// A template base class for all concrete iterator classes.
/** ConcreteIter provides a "fat" interface, but a compile-time error will
    be generated if part of the interface is used that is not supported by
    the implementation class. */
template <class T, class Ifx>
class ConcreteIter
{
  shared_ptr<Ifx> rep;

  void makeUnique()
  {
    if ( !rep.unique() )
      {
        rep.reset( rep->clone() );
      }
  }

public:
  ConcreteIter(const ConcreteIter& other) : rep(other.rep) {}

  ConcreteIter(shared_ptr<Ifx> impl)      : rep(impl) {}

  // Default assigment-oper OK

  void next()                    { makeUnique(); rep->next(); }
  void prev()                    { makeUnique(); rep->prev(); }
  void step(int n)               { makeUnique(); rep->step(n); }
  ConcreteIter& operator++()     { next(); return *this; }
  ConcreteIter operator++(int)   { ConcreteIter c(*this); next(); return c; }
  ConcreteIter& operator--()     { prev(); return *this; }
  ConcreteIter operator--(int)   { ConcreteIter c(*this); prev(); return c; }

  ConcreteIter operator+=(int n) { step(n); return *this; }
  ConcreteIter operator-=(int n) { step(-n); return *this; }

  int operator-(const ConcreteIter& other) const
    { return rep->minus(other.rep); }

  T*   operator->()                 const { return &(rep->get()); }
  T&   operator*()                  const { return rep->get(); }

  bool atEnd()                      const { return rep->atEnd(); }
  bool isValid()                    const { return !atEnd(); }
  int  fromEnd()                    const { return rep->fromEnd(); }
  bool operator==(const IterEnd_t&) const { return atEnd(); }
  bool operator!=(const IterEnd_t&) const { return !atEnd(); }
};


///////////////////////////////////////////////////////////////////////
//
// Forward iterators
//
///////////////////////////////////////////////////////////////////////


/// Abstract interface class for forward iterators.
template <class T>
class FwdIterIfx
{
public:
  typedef T ValueType;
  typedef FwdIterIfx<T> Interface;

  virtual ~FwdIterIfx() {}
  virtual FwdIterIfx<T>* clone() const = 0;
  virtual void next() = 0;
  virtual T& get() const = 0;
  virtual bool atEnd() const = 0;
};


/// Adapts forward iterators to the FwdIterIfx interface.
template <class Iter, class T>
class FwdIterAdapter : public FwdIterIfx<T>
{
  FwdIterAdapter<Iter, T>& operator=(const FwdIterAdapter<Iter, T>&);

  typedef FwdIterIfx<T> Base;

  Iter itsIter;
  Iter itsEnd;

  FwdIterAdapter<Iter, T>(const FwdIterAdapter<Iter, T>& that) :
    Base(), itsIter(that.itsIter), itsEnd(that.itsEnd) {}

public:
  FwdIterAdapter<Iter, T>(Iter iter, Iter end) :
    Base(), itsIter(iter), itsEnd(end) {}

  virtual Base* clone() const { return new FwdIterAdapter(*this); }
  virtual void   next()       { ++itsIter; }
  virtual T&      get() const { return getref(*itsIter); }
  virtual bool  atEnd() const { return itsIter == itsEnd; }
};


/// Concrete forward iterator class.
template <class T>
class FwdIter :
  public ConcreteIter<T, FwdIterIfx<T> >
{
  template <class It>
  shared_ptr<FwdIterIfx<T> >
  adapt(It iter, It end)
  {
    return shared_ptr<FwdIterIfx<T> >
      (new FwdIterAdapter<It, T>(iter, end));
  }

public:
  typedef FwdIterIfx<T> Interface;
  typedef ConcreteIter<T, Interface> Base;

  FwdIter(const Base& other) : Base(other) {}

  FwdIter(shared_ptr<Interface> impl) : Base(impl) {}

  template <class It>
  FwdIter(It iter, It end) : Base(adapt(iter, end)) {}
};

///////////////////////////////////////////////////////////////////////
//
// Bidirectional iterators
//
///////////////////////////////////////////////////////////////////////


/// Abstract interface class for bidirectional iterators.
template <class T>
class BidirIterIfx : public FwdIterIfx<T>
{
public:
  typedef BidirIterIfx<T> Interface;

  virtual BidirIterIfx<T>* clone() const = 0;
  virtual void prev() = 0;
};


/// Adapts bidirectional iterators to the BidirIterIfx interface.
template <class Iter, class T>
class BidirIterAdapter : public BidirIterIfx<T>
{
  BidirIterAdapter<Iter, T>& operator=(const BidirIterAdapter<Iter, T>&);

  typedef BidirIterIfx<T> Base;

  Iter itsIter;
  Iter itsEnd;

  BidirIterAdapter<Iter, T>(const BidirIterAdapter<Iter, T>& that) :
    Base(), itsIter(that.itsIter), itsEnd(that.itsEnd) {}

public:
  BidirIterAdapter<Iter, T>(Iter iter, Iter end) :
    Base(), itsIter(iter), itsEnd(end) {}

  virtual Base* clone() const { return new BidirIterAdapter(*this); }
  virtual void   next()       { ++itsIter; }
  virtual void   prev()       { --itsIter; }
  virtual T&      get() const { return getref(*itsIter); }
  virtual bool  atEnd() const { return itsIter == itsEnd; }
};


/// Concrete bidirectional iterator class.
template <class T>
class BidirIter :
  public ConcreteIter<T, BidirIterIfx<T> >
{
  template <class It>
  shared_ptr<BidirIterIfx<T> >
  adapt(It iter, It end)
  {
    return shared_ptr<BidirIterIfx<T> >
      (new BidirIterAdapter<It, T>(iter, end));
  }

public:
  typedef BidirIterIfx<T> Interface;
  typedef ConcreteIter<T, Interface> Base;

  BidirIter(const Base& other) : Base(other) {}

  BidirIter(shared_ptr<Interface> impl) : Base(impl) {}

  template <class It>
  BidirIter(It iter, It end) : Base(adapt(iter, end)) {}
};


///////////////////////////////////////////////////////////////////////
//
// Random Access iterators
//
///////////////////////////////////////////////////////////////////////


/// Abstract interface class for random-access iterators.
template <class T>
class RxsIterIfx : public BidirIterIfx<T>
{
public:
  typedef RxsIterIfx<T> Interface;

  virtual RxsIterIfx<T>* clone() const = 0;
  virtual void step(int n) = 0;
  virtual int minus(RxsIterIfx<T>& other) const = 0;
  virtual int fromEnd() const = 0;
};


/// Adapts random-access iterators to the RxsIterIfx interface.
template <class Iter, class T>
class RxsIterAdapter : public RxsIterIfx<T>
{
  RxsIterAdapter<Iter, T>& operator=(const RxsIterAdapter<Iter, T>&);

  typedef RxsIterIfx<T> Base;

  Iter itsIter;
  Iter itsEnd;

  RxsIterAdapter<Iter, T>(const RxsIterAdapter<Iter, T>& that) :
    Base(), itsIter(that.itsIter), itsEnd(that.itsEnd) {}

public:
  RxsIterAdapter<Iter, T>(Iter iter, Iter end) :
    Base(), itsIter(iter), itsEnd(end) {}

  virtual Base*  clone()       const { return new RxsIterAdapter(*this); }
  virtual void    next()             { ++itsIter; }
  virtual void    prev()             { --itsIter; }
  virtual void    step(int n)        { itsIter += n; }
  virtual T&       get()       const { return getref(*itsIter); }
  virtual bool   atEnd()       const { return itsIter == itsEnd; }
  virtual int  fromEnd()       const { return itsEnd - itsIter; }

  virtual int minus(RxsIterIfx<T>& other_) const
  {
    RxsIterAdapter<Iter, T>& other =
      dynamic_cast<RxsIterAdapter<Iter, T>& >(other_);

    return itsIter - other.itsIter;
  }
};


/// Concrete random-access iterator class.
template <class T>
class RxsIter :
  public ConcreteIter<T, RxsIterIfx<T> >
{
  template <class It>
  shared_ptr<RxsIterIfx<T> >
  adapt(It iter, It end)
  {
    return shared_ptr<RxsIterIfx<T> >
      (new RxsIterAdapter<It, T>(iter, end));
  }

public:
  typedef RxsIterIfx<T> Interface;
  typedef ConcreteIter<T, Interface> Base;

  RxsIter(const Base& other) : Base(other) {}

  RxsIter(shared_ptr<Interface> impl) : Base(impl) {}

  template <class It>
  RxsIter(It iter, It end) : Base(adapt(iter, end)) {}
};

} // end namespace Util

static const char vcid_iter_h[] = "$Header$";
#endif // !ITER_H_DEFINED
