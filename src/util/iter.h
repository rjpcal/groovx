///////////////////////////////////////////////////////////////////////
//
// iter.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Aug 17 11:05:24 2001
// written: Fri Aug 17 15:15:52 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ITER_H_DEFINED
#define ITER_H_DEFINED

#include "util/pointers.h"

#include "util/utilfwd.h"

///////////////////////////////////////////////////////////////////////
//
// ConcreteIter class
//
///////////////////////////////////////////////////////////////////////


template <class T, class Ifx>
class Util::ConcreteIter
{
  shared_ptr<Ifx> itsImpl;

  void makeUnique()
  {
    if ( !itsImpl.unique() )
      {
        itsImpl.reset( itsImpl->clone() );
      }
  }

public:
  ConcreteIter(const ConcreteIter& other) : itsImpl(other.itsImpl) {}

  template <class It>
  ConcreteIter(It iter, It end)           : itsImpl(Ifx::adapt(iter, end)) {}

  // Default assn-oper OK

  void next()                    { makeUnique(); itsImpl->next(); }
  void prev()                    { makeUnique(); itsImpl->prev(); }
  void step(int n)               { makeUnique(); itsImpl->step(n); }
  ConcreteIter& operator++()     { next(); return *this; }
  ConcreteIter operator++(int)   { ConcreteIter c(*this); next(); return c; }
  ConcreteIter& operator--()     { prev(); return *this; }
  ConcreteIter operator--(int)   { ConcreteIter c(*this); prev(); return c; }

  ConcreteIter operator+=(int n) { step(n); return *this; }
  ConcreteIter operator-=(int n) { step(-n); return *this; }

  int operator-(const ConcreteIter& other) const
    { return itsImpl->minus(other.itsImpl); }

  T*   operator->()                 const { return &(itsImpl->get()); }
  T&   operator*()                  const { return itsImpl->get(); }

  bool atEnd()                      const { return itsImpl->atEnd(); }
  bool isValid()                    const { return !atEnd(); }
  int  fromEnd()                    const { return itsImpl->fromEnd(); }
  bool operator==(const IterEnd_t&) const { return atEnd(); }
  bool operator!=(const IterEnd_t&) const { return !atEnd(); }
};


///////////////////////////////////////////////////////////////////////
//
// Forward iterators
//
///////////////////////////////////////////////////////////////////////


template <class T>
class Util::FwdIterIfx
{
public:
  virtual ~FwdIterIfx() {}
  virtual FwdIterIfx<T>* clone() const = 0;
  virtual void next() = 0;
  virtual T& get() const = 0;
  virtual bool atEnd() const = 0;

  template <class It>
  static shared_ptr<Util::FwdIterIfx<T> > adapt(It iter, It end);
};


template <class Iter, class T>
class Util::FwdIterAdapter : public Util::FwdIterIfx<T>
{
  FwdIterAdapter<Iter, T>& operator=(const FwdIterAdapter<Iter, T>&);

  typedef Util::FwdIterIfx<T> Base;

  Iter itsIter;
  Iter itsEnd;

  FwdIterAdapter<Iter, T>(const FwdIterAdapter<Iter, T>& that) :
    Base(), itsIter(that.itsIter), itsEnd(that.itsEnd) {}

public:
  FwdIterAdapter<Iter, T>(Iter iter, Iter end) :
    Base(), itsIter(iter), itsEnd(end) {}

  virtual Base* clone() const { return new FwdIterAdapter(*this); }
  virtual void   next()       { ++itsIter; }
  virtual T&      get() const { return *itsIter; }
  virtual bool  atEnd() const { return itsIter == itsEnd; }
};


template <class T>
template <class It>
inline shared_ptr<Util::FwdIterIfx<T> >
Util::FwdIterIfx<T>::adapt(It iter, It end)
{
  return shared_ptr<Util::FwdIterIfx<T> >
    (new FwdIterAdapter<It, T>(iter, end));
};


template <class T>
class Util::FwdIter :
  public Util::ConcreteIter<T, Util::FwdIterIfx<T> >
{
public:
  typedef Util::ConcreteIter<T, Util::FwdIterIfx<T> > Base;

  FwdIter(const Base& other) : Base(other) {}

  template <class It>
  FwdIter(It iter, It end) : Base(iter, end) {}
};

///////////////////////////////////////////////////////////////////////
//
// Bidirectional iterators
//
///////////////////////////////////////////////////////////////////////


template <class T>
class Util::BidirIterIfx : public Util::FwdIterIfx<T>
{
public:
  virtual BidirIterIfx<T>* clone() const = 0;
  virtual void prev() = 0;

  template <class It>
  static shared_ptr<Util::BidirIterIfx<T> > adapt(It iter, It end);
};

template <class Iter, class T>
class Util::BidirIterAdapter : public Util::BidirIterIfx<T>
{
  BidirIterAdapter<Iter, T>& operator=(const BidirIterAdapter<Iter, T>&);

  typedef Util::BidirIterIfx<T> Base;

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
  virtual T&      get() const { return *itsIter; }
  virtual bool  atEnd() const { return itsIter == itsEnd; }
};


template <class T>
template <class It>
inline shared_ptr<Util::BidirIterIfx<T> >
Util::BidirIterIfx<T>::adapt(It iter, It end)
{
  return shared_ptr<Util::BidirIterIfx<T> >
    (new BidirIterAdapter<It, T>(iter, end));
};


template <class T>
class Util::BidirIter :
  public Util::ConcreteIter<T, Util::BidirIterIfx<T> >
{
public:
  typedef Util::ConcreteIter<T, Util::BidirIterIfx<T> > Base;

  BidirIter(const Base& other) : Base(other) {}

  template <class It>
  BidirIter(It iter, It end) : Base(iter, end) {}
};


///////////////////////////////////////////////////////////////////////
//
// Random Access iterators
//
///////////////////////////////////////////////////////////////////////

template <class T>
class Util::RxsIterIfx : public Util::BidirIterIfx<T>
{
public:
  virtual RxsIterIfx<T>* clone() const = 0;
  virtual void step(int n) = 0;
  virtual int minus(RxsIterIfx<T>& other) const = 0;
  virtual int fromEnd() const = 0;

  template <class It>
  static shared_ptr<Util::RxsIterIfx<T> > adapt(It iter, It end);
};

template <class Iter, class T>
class Util::RxsIterAdapter : public Util::RxsIterIfx<T>
{
  RxsIterAdapter<Iter, T>& operator=(const RxsIterAdapter<Iter, T>&);

  typedef Util::RxsIterIfx<T> Base;

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
  virtual T&       get()       const { return *itsIter; }
  virtual bool   atEnd()       const { return itsIter == itsEnd; }
  virtual int  fromEnd()       const { return itsEnd - itsIter; }

  virtual int minus(RxsIterIfx<T>& other_) const
  {
    RxsIterAdapter<Iter, T>& other =
      dynamic_cast<RxsIterAdapter<Iter, T>& >(other_);

    return itsIter - other.itsIter;
  }
};

template <class T>
template <class It>
shared_ptr<Util::RxsIterIfx<T> >
Util::RxsIterIfx<T>::adapt(It iter, It end)
{
  return shared_ptr<Util::RxsIterIfx<T> >
    (new RxsIterAdapter<It, T>(iter, end));
}

template <class T>
class Util::RxsIter :
  public Util::ConcreteIter<T, Util::RxsIterIfx<T> >
{
public:
  typedef Util::ConcreteIter<T, Util::RxsIterIfx<T> > Base;

  RxsIter(const Base& other) : Base(other) {}

  template <class It>
  RxsIter(It iter, It end) : Base(iter, end) {}
};

static const char vcid_iter_h[] = "$Header$";
#endif // !ITER_H_DEFINED
