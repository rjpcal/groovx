///////////////////////////////////////////////////////////////////////
//
// keanu.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Mar 12 12:23:11 2001
// written: Wed Mar 14 12:57:19 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef KEANU_H_DEFINED
#define KEANU_H_DEFINED

#include "datablock.h"
#include "num.h"

class Mtx;



///////////////////////////////////////////////////////////////////////
//
// ElemProxy class definition
//
///////////////////////////////////////////////////////////////////////


class ElemProxy {
  Mtx& m;
  int i;

  double& uniqueRef();

public:
  ElemProxy(Mtx& m_, int i_) : m(m_), i(i_) {}

  double& operator=(double d);
  double& operator+=(double d);
  double& operator-=(double d);
  double& operator*=(double d);
  double& operator/=(double d);

  operator double();
};


///////////////////////////////////////////////////////////////////////
//
// ConstSlice class definition
//
///////////////////////////////////////////////////////////////////////


class ConstSlice {
private:
  ConstSlice& operator=(const ConstSlice& other); // not implemented

protected:
  DataBlock*& itsStorage;
  ptrdiff_t itsOffset;
  int itsStride;
  int itsNelems;

  const double* data() const;

  const double* address(int i) const { return data() + itsStride*i; }

  friend class Mtx;

  ConstSlice(DataBlock* const& storage, const double* d, int s, int n);

public:

  // Forms a dummy empty slice
  ConstSlice();

  ConstSlice(const ConstSlice& other) :
	 itsStorage(other.itsStorage),
	 itsOffset(other.itsOffset),
	 itsStride(other.itsStride),
	 itsNelems(other.itsNelems)
  {}

  //
  // Data access
  //

  double operator[](int i) const { return *(address(i)); }

  int nelems() const { return itsNelems; }

  ConstSlice rightmost(int n) const
  {
	 int first = itsNelems - n;
	 if (first < 0) first = 0;

	 return ConstSlice(itsStorage, address(first), itsStride, n);
  }

  ConstSlice leftmost(int n) const
  {
	 return ConstSlice(itsStorage, data(), itsStride, n);
  }

  //
  // Functions
  //

  double sum() const
  {
	 double s = 0.0;
	 for (ConstIterator i = begin(), e = end(); i != e; ++i)
		s += *i;
	 return s;
  }

  double mean() const
    { return sum()/itsNelems; }

  static double dot(const ConstSlice& s1, const ConstSlice& s2)
  {
	 double result = 0.0;
	 const int n = s1.itsNelems;
	 for (int i = 0; i < n; ++i)
		result += s1[i] * s2[i];
	 return result;
  }

  //
  // Iteration
  //

  class ConstIterator {
	 const double* data;
	 int stride;

	 ConstIterator(const double* d, int s) : data(d), stride(s) {}

	 friend class ConstSlice;

  public:

	 double operator*() const { return *data; }

	 ConstIterator& operator++() { data += stride; return *this; }

	 bool operator==(const ConstIterator& other) const
	   { return data == other.data; }

	 bool operator!=(const ConstIterator& other) const
	   { return data != other.data; }
  };

  ConstIterator begin() const
    { return ConstIterator(data(), itsStride); }
  ConstIterator end() const
    { return ConstIterator(address(itsNelems), itsStride); }
};


///////////////////////////////////////////////////////////////////////
//
// Slice class definition
//
///////////////////////////////////////////////////////////////////////


class Slice : public ConstSlice {
  friend class Mtx;

  double* data();
  double* address(int i) { return data() + itsStride*i; }

  Slice(DataBlock*& m, double* d, int s, int n) :
	 ConstSlice(m,d,s,n) {}

public:

  double& operator[](int i) { return *(address(i)); }

  Slice rightmost(int n)
  {
	 int first = itsNelems - n;
	 if (first < 0) first = 0;

	 return Slice(itsStorage, address(first), itsStride, n);
  }

  Slice leftmost(int n)
  {
	 return Slice(itsStorage, data(), itsStride, n);
  }

  Slice& operator=(const ConstSlice& other);

  //
  // Functions
  //

  template <class F>
  void apply(F func)
  {
	 for (Iterator i = begin(), e = end(); i != e; ++i)
		*i = func(*i);
  }

  void operator/=(double div) { apply(Div(div)); }

  //
  // Iteration
  //

  class Iterator {
	 double* data;
	 int stride;

	 Iterator(double* d, int s) : data(d), stride(s) {}

	 friend class Slice;

  public:

	 double& operator*() { return *data; }

	 Iterator& operator++() { data += stride; return *this; }

	 bool operator==(const Iterator& other) const
	   { return data == other.data; }

	 bool operator!=(const Iterator& other) const
	   { return data != other.data; }
  };

  Iterator begin()
    { return Iterator(data(), itsStride); }
  Iterator end()
    { return Iterator(address(itsNelems), itsStride); }
};




///////////////////////////////////////////////////////////////////////
//
// Mtx class definition
//
///////////////////////////////////////////////////////////////////////


typedef struct mxArray_tag mxArray;

class Mtx {
public:
  enum StoragePolicy { COPY, BORROW };

private:
  template <class T>
  static void doswap(T& t1, T& t2)
  { T t2_ = t2; t2 = t1; t1 = t2_; }


  void swap(Mtx& other)
  {
	 doswap(storage_, other.storage_);
	 doswap(mrows_, other.mrows_);
	 doswap(ncols_, other.ncols_);
	 doswap(start_, other.start_);
  }

  // Sets up an appropriate DataBlock and increments its reference count
  void initialize(double* data, int mrows, int ncols, StoragePolicy s = COPY);

public:

  //
  // Constructors
  //

  Mtx(mxArray* a, StoragePolicy s = COPY);

  Mtx(double* data, int mrows, int ncols, StoragePolicy s = COPY)
    { initialize(data, mrows, ncols, s); }

  Mtx(int mrows, int ncols);

  Mtx(const ConstSlice& s);

  Mtx(const Mtx& other) :
	 storage_(other.storage_),
	 mrows_(other.mrows_),
	 ncols_(other.ncols_),
	 start_(other.start_)
  {
	 storage_->incrRefCount();
  }

  ~Mtx();

  Mtx& operator=(const Mtx& other)
  {
	 Mtx temp(other);
	 this->swap(temp);
  }

  mxArray* makeMxArray() const;


  //
  // I/O
  //

  void print() const;


  //
  // Data access
  //

  friend class ElemProxy;

  ElemProxy at(int row, int col) { return ElemProxy(*this, offsetFromStart(row, col)); }

  double at(int row, int col) const { return start_[offsetFromStart(row, col)]; }

  ElemProxy at(int elem) { return ElemProxy(*this, elem); }

  double at(int elem) const { return start_[elem]; }

  void reshape(int mrows, int ncols);

  int length() const { return (mrows_ > ncols_) ? mrows_ : ncols_; }

  int nelems() const { return mrows_*ncols_; }

  int mrows() const { return mrows_; }

  int ncols() const { return ncols_; }

  //
  // Slices, submatrices
  //

  Slice row(int r)
    { return Slice(storage_, address(r,0), mrows_, ncols_); }

  ConstSlice row(int r) const
    { return ConstSlice(storage_, address(r,0), mrows_, ncols_); }

  Slice column(int c)
    { return Slice(storage_, address(0,c), 1, mrows_); }

  ConstSlice column(int c) const
    { return ConstSlice(storage_, address(0,c), 1, mrows_); }

  Slice asSlice()
    { return Slice(storage_, start_, 1, nelems()); }


  //
  // Functions
  //

  void apply(double func(double))
    {
		double* p = start_;
		double* end = start_+nelems();
		for (; p < end; ++p)
		  *p = func(*p);
    }

  // result = vec * this;
  void leftMultAndAssign(const ConstSlice& vec, Slice& result) const;

private:
  void makeUnique();

  ptrdiff_t offsetFromStorage(int row, int col) const
    { return (start_ - storage_->itsData) + offsetFromStart(row, col); }

  int offsetFromStart(int row, int col) const
    { return row + (col*mrows_); }

  double* address(int row, int col)
    { return start_ + offsetFromStart(row, col); }

  const double* address(int row, int col) const
    { return start_ + offsetFromStart(row, col); }

  friend class ConstSlice;
  friend class Slice;

  DataBlock* storage_;
  int mrows_;
  int ncols_;
  double* start_;
};

///////////////////////////////////////////////////////////////////////
//
// Inline member function definitions
//
///////////////////////////////////////////////////////////////////////


inline double& ElemProxy::uniqueRef()
{ m.makeUnique(); return m.start_[i]; }

inline double& ElemProxy::operator=(double d)
{ return (uniqueRef() = d); }

inline double& ElemProxy::operator+=(double d)
{ return (uniqueRef() += d); }

inline double& ElemProxy::operator-=(double d)
{ return (uniqueRef() -= d); }

inline double& ElemProxy::operator*=(double d)
{ return (uniqueRef() *= d); }

inline double& ElemProxy::operator/=(double d)
{ return (uniqueRef() /= d); }

inline ElemProxy::operator double() { return m.start_[i]; }


inline const double* ConstSlice::data() const
{ return itsStorage->itsData + itsOffset; }

inline ConstSlice::ConstSlice(DataBlock* const& storage,
										const double* d, int s, int n) :
  itsStorage(const_cast<DataBlock*&>(storage)),
  itsOffset(d - storage->itsData),
  itsStride(s),
  itsNelems(n) {}

inline double* Slice::data() { return itsStorage->itsData + itsOffset; }

static const char vcid_keanu_h[] = "$Header$";
#endif // !KEANU_H_DEFINED
